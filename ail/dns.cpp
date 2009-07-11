#include <ail/dns.hpp>
#include <ail/string.hpp>
#include <ail/net.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace ail
{
	namespace
	{
		std::size_t const dns_buffer_size = 1024;
	}

	bool dns_request::operator==(dns_request const & other) const
	{
		return transaction_id == other.transaction_id && name == other.name;
	}

	dns_client::dns_client(boost::asio::io_service & io_service, std::string const & server, ushort port):
		io_service(io_service),
		resolver(io_service),
		server(server),
		port(port),
		current_id(0),
		socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
		got_endpoint(false)
	{
		receive_data();
	}

	void dns_client::lookup(std::string const & name, dns_lookup_handler_type lookup_handler)
	{
		boost::mutex::scoped_lock scoped_lock(mutex);

		//boost::asio::ip::udp::resolver::query query(server, "dns");
		boost::asio::ip::udp::resolver::query query(server, "53");

		dns_request request;
		request.transaction_id = current_id;
		request.name = name;
		request.lookup_handler = lookup_handler;

		pending_requests.push_back(request);

		if(got_endpoint)
			send_dns_request(request);
		else
		{
			resolver.async_resolve(query, boost::bind(&dns_client::resolve_event, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator, request));
			io_service.stop();
		}

		current_id++;
	}

	void dns_client::resolve_event(boost::system::error_code const & error, boost::asio::ip::udp::resolver::iterator endpoint_iterator, dns_request & request)
	{
		if(!error)
		{
			boost::mutex::scoped_lock scoped_lock(mutex);

			//std::cout << "Retrieved a DNS endpoint" << std::endl;
			got_endpoint = true;
			endpoint = *endpoint_iterator;

			send_dns_request(request);
		}
		else
			error_occured(request, "Unable to resolve DNS server address: " + error.message());
	}

	void dns_client::continue_processing(char * receive_buffer)
	{
		delete receive_buffer;
		receive_data();
	}

	void dns_client::receive_event(boost::system::error_code const & error, std::size_t bytes_received, char * receive_buffer)
	{
		boost::mutex::scoped_lock scoped_lock(mutex);

		std::string malformed_packet_message = "DNS server returned a malformed packet (" + number_to_string<std::size_t>(bytes_received) + " byte(s))";

		if(!error)
		{
			//std::cout << "Received " << bytes_received << " byte(s)" << std::endl;

			std::string packet(receive_buffer, bytes_received);
			bit_reader reader(packet);

			std::size_t word = 16;

			ushort transaction_id;

			try
			{
				transaction_id = static_cast<ushort>(reader.read(word));
			}
			catch(exception &)
			{
				global_error(malformed_packet_message);
				continue_processing(receive_buffer);
				return;
			}

			request_vector::iterator iterator;
			if(get_request(transaction_id, iterator))
			{
				dns_lookup_result output;
				dns_request & request = *iterator;
				output.name = request.name;

				try
				{
					bool is_response = reader.read_bool();
					ulong opcode = reader.read(4);
					bool authoritative = reader.read_bool();
					bool truncated = reader.read_bool();
					bool recursion_desired = reader.read_bool();
					bool recursion_available = reader.read_bool();
					reader.read_bool();
					bool answer_authenticated = reader.read_bool();
					reader.read_bool();
					ulong reply_code = reader.read(4);
					
					ulong questions = reader.read(word);
					ulong answer_records = reader.read(word);
					ulong authority_records = reader.read(word);
					ulong additional_records = reader.read(word);

					for(ulong i = 0; i < questions; i++)
						output.questions.push_back(read_question(reader));

					for(ulong i = 0; i < answer_records; i++)
						output.answers.push_back(read_answer(reader));

					output.success = (answer_records > 0);
					if(output.success)
					{
						BOOST_FOREACH(dns_answer & answer, output.answers)
						{
							if(answer.is_ip)
							{
								output.address = answer.address;
								break;
							}
						}
					}
					else
					{
						error_occured(request, "DNS reply does not contain any answers");
						continue_processing(receive_buffer);
						return;
					}

					for(ulong i = 0; i < authority_records; i++)
						output.authoritative_nameservers.push_back(read_answer(reader));

					for(ulong i = 0; i < additional_records; i++)
						output.additional_records.push_back(read_answer(reader));

					request.lookup_handler(output);
				}
				catch(exception &)
				{
					error_occured(request, malformed_packet_message);
				}
			}
			else
				global_error("Invalid transaction ID returned by DNS server: 0x" + number_to_string<ushort>(transaction_id, std::ios_base::hex));
		}
		else
			global_error("Receive error: " + error.message());

		continue_processing(receive_buffer);
	}

	void dns_client::send_event(boost::system::error_code const & error, std::size_t bytes_received, dns_request & request)
	{
		boost::mutex::scoped_lock scoped_lock(mutex);

		if(!error)
		{
			//std::cout << "Send event (" << bytes_received << " bytes), deallocating buffer" << std::endl;
		}
		else
			error_occured(request, "Unable to send data to DNS server");

		delete request.buffer;
	}

	void dns_client::receive_data()
	{
		char * receive_buffer = new char[dns_buffer_size];
		socket.async_receive_from(boost::asio::buffer(receive_buffer, dns_buffer_size), endpoint, boost::bind(&dns_client::receive_event, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, receive_buffer));
		io_service.stop();
	}

	void dns_client::send_dns_request(dns_request & request)
	{
		string_vector tokens = ail::tokenise(request.name, ".");

		std::string packet =
			big_endian_string(request.transaction_id, 2) +
			std::string("\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10);

		BOOST_FOREACH(std::string const & current_token, tokens)
			packet += big_endian_string(static_cast<ulong>(current_token.size()), 1) + current_token;

		packet += std::string("\x00\x00\x01\x00\x01", 5);

		std::size_t packet_size = packet.size();
		request.buffer = new char[packet_size];
		std::memcpy(request.buffer, packet.c_str(), packet_size);
		
		socket.async_send_to(boost::asio::buffer(request.buffer, packet_size), endpoint, boost::bind(&dns_client::send_event, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, request));
		io_service.stop();
	}

	void dns_client::global_error(std::string const & message)
	{
		BOOST_FOREACH(dns_request & request, pending_requests)
		{
			dns_lookup_result result;
			result.success = false;
			result.error_message = message;
			request.lookup_handler(result);
		}
		pending_requests.clear();
	}
	
	void dns_client::error_occured(dns_request & request, std::string const & message)
	{
		dns_lookup_result result;
		result.success = false;
		result.name = request.name;
		result.error_message = message;
		request.lookup_handler(result);
		erase_request(request);
	}

	void dns_client::erase_request(dns_request & request)
	{
		for(request_vector::iterator i = pending_requests.begin(), end = pending_requests.end(); i != end; i++)
		{
			if(*i == request)
			{
				pending_requests.erase(i);
				break;
			}
		}
	}

	bool dns_client::get_request(ushort transaction_id, request_vector::iterator & output)
	{
		for(request_vector::iterator i = pending_requests.begin(), end = pending_requests.end(); i != end; i++)
		{
			if(i->transaction_id == transaction_id)
			{
				output = i;
				return true;
			}
		}
		return false;
	}

	dns_question dns_client::read_question(bit_reader & reader)
	{
		dns_question output;
		output.name = read_name(reader);
		output.type = reader.read_bytes(2);
		output.dns_class = reader.read_bytes(2);
		//std::cout << "Question: " << output.name << std::endl;
		return output;
	}

	dns_answer dns_client::read_answer(bit_reader & reader)
	{
		dns_answer output;
		output.name = read_name(reader);
		output.type = reader.read_bytes(2);
		output.dns_class = reader.read_bytes(2);
		output.time_to_live = reader.read_bytes(4);
		output.is_unknown = false;

		ulong data_length = reader.read_bytes(2);
		switch(output.type)
		{
			//A, IPv4 address
			case 1:
			{
				output.is_ip = true;
				ulong numeric_ip = reader.read_bytes(4);
				output.address = convert_ipv4(numeric_ip);
				break;
			}

			//AAAA, IPv6 address
			case 28:
				output.is_ip = true;
				output.address = convert_ipv6(reader.string(16));
				break;

			//NS, nameserver, name
			case 2:

			//CNAME, canonical name, name
			case 5:
				output.address = read_name(reader);
				break;

			//unknown
			default:
				output.is_unknown = true;
				output.other_data = reader.string(data_length);
				break;

		}
		return output;
	}

	std::string dns_client::read_name(bit_reader & reader)
	{
		std::string address;

		bool fix_offset = false;
		std::size_t final_offset;

		bool is_first = true;
		while(true)
		{
			ulong length = reader.read_bytes(1);
			if(length == 0)
				break;
			if(length >= 0xc0)
			{
				ulong subtrahend = 0xc000;
				ulong new_offset = ((length << bits_per_byte) | reader.read_bytes(1)) - subtrahend;
				if(!fix_offset)
				{
					fix_offset = true;
					final_offset = reader.get_offset();
				}
				reader.set_offset(bits_per_byte * new_offset);
				continue;
			}
			if(is_first)
				is_first = false;
			else
				address += ".";
			address += reader.string(length);
		}

		if(fix_offset)
			reader.set_offset(final_offset);

		//std::cout << "Read name: " << address << std::endl;

		return address;
	}
}
