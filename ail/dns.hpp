#ifndef AIL_DNS_HPP
#define AIL_DNS_HPP

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

#include <ail/types.hpp>
#include <ail/memory.hpp>

namespace ail
{
	struct dns_question
	{
		std::string name;
		ulong
			type,
			dns_class;
	};

	struct dns_answer
	{
		std::string name;
		ulong
			type,
			dns_class;

		ulong time_to_live;

		bool is_ip, is_unknown;
		std::string
			address,
			other_data;
	};

	typedef std::vector<dns_question> question_vector;
	typedef std::vector<dns_answer> answer_vector;

	struct dns_lookup_result
	{
		bool success;
		std::string
			name,
			error_message,
			address;

		question_vector questions;
		answer_vector
			answers,
			authoritative_nameservers,
			additional_records;
	};

	typedef boost::function<void (dns_lookup_result & result)> dns_lookup_handler_type;

	struct dns_request
	{
		ushort transaction_id;
		std::string name;
		dns_lookup_handler_type lookup_handler;
		char * buffer;

		bool operator==(dns_request const & other) const;
	};

	class dns_client
	{
	public:
		dns_client(boost::asio::io_service & io_service, std::string const & server, ushort port);

		void lookup(std::string const & name, dns_lookup_handler_type lookup_handler);

	private:
		std::string server;
		ulong port;

		boost::asio::io_service & io_service;

		boost::asio::ip::udp::resolver resolver;
		boost::asio::ip::udp::socket socket;

		bool got_endpoint;
		boost::asio::ip::udp::endpoint endpoint;

		boost::mutex mutex;

		ushort current_id;

		typedef std::vector<dns_request> request_vector;

		request_vector pending_requests;

		void resolve_event(boost::system::error_code const & error, boost::asio::ip::udp::resolver::iterator endpoint_iterator, dns_request & request);
		void receive_event(boost::system::error_code const & error, std::size_t bytes_received, char * receive_buffer);
		void send_event(boost::system::error_code const & error, std::size_t bytes_received, dns_request & request);

		void global_error(std::string const & message);
		void error_occured(dns_request & request, std::string const & message);

		void receive_data();
		void send_dns_request(dns_request & request);
		void erase_request(dns_request & request);
		bool get_request(ushort transaction_id, request_vector::iterator & output);

		dns_question read_question(bit_reader & reader);
		dns_answer read_answer(bit_reader & reader);
		std::string read_name(bit_reader & reader);
		void continue_processing(char * receive_buffer);
	};
}

#endif
