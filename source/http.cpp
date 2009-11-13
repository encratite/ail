#include <iostream>
#include <istream>
#include <ostream>

#include <ail/http.hpp>
#include <ail/string.hpp>
#include <ail/zlib.hpp>
#include <ail/file.hpp>
#include <ail/net.hpp>

namespace ail
{
	bool process_chunked_data(std::string const & data, std::string & content, std::size_t offset)
	{
		std::string const target = "\r\n";
		while(offset < data.size())
		{
			std::size_t newline_offset = data.find(target, offset);
			if(newline_offset == std::string::npos)
				return false;
			std::string number_string = data.substr(offset, newline_offset - offset);
			long chunk_size;
			if(!ail::string_to_number<long>(number_string, chunk_size, std::ios_base::hex))
				return false;
			std::size_t chunk_offset = newline_offset + target.length();
			std::size_t end_of_chunk = chunk_offset + chunk_size;
			std::string chunk = data.substr(chunk_offset, end_of_chunk - chunk_offset);
			content += chunk;
			offset = end_of_chunk + target.length();
		}
		return true;
	}

	http_client::http_client(boost::asio::io_service & io_service):
		io_service(io_service),
		resolver(io_service),
		socket(io_service),
		has_error_handler(false),
		has_download_handler(false),
		has_download_finished_handler(false),
		user_agent(http_user_agent_firefox),
		method("GET")
	{
	}

	void http_client::use_post()
	{
		method = "POST";
	}

	void http_client::add_post_data(std::string const & field, std::string const & value)
	{
		form_data[field] = value;
	}

	void http_client::start_download(std::string const & new_url)
	{
		std::string const
			http_string = "http",
			https_string = "https",
			protocol_separator = "://";

		std::size_t host_offset;

		url = new_url;

		std::size_t separator_offset = url.find(protocol_separator);
		if(separator_offset == std::string::npos)
		{
			use_ssl = false;
			host_offset = 0;
		}
		else
		{
			host_offset = separator_offset + protocol_separator.length();
			std::string protocol = url.substr(0, separator_offset);
			if(protocol == http_string)
				use_ssl = false;
			else if(protocol == https_string)
				use_ssl = true;
			else
				throw ail::exception("Invalid protocol specified for HTTP client");
		}

		std::size_t path_offset = url.find('/', host_offset);
		if(path_offset == std::string::npos)
		{
			host = url.substr(host_offset);
			path = "/";
		}
		else
		{
			host = url.substr(host_offset, path_offset - host_offset);
			path = url.substr(path_offset);
		}

		boost::asio::ip::tcp::resolver::query query(host, "http");
		resolver.async_resolve(query, boost::bind(&http_client::dns_event, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
		io_service.stop();
	}

	bool http_client::start_download(std::string const & new_url, std::string const & file_name)
	{
		if(!file_output.open_create(file_name))
			return false;
		start_download(new_url);
		return true;
	}

	void http_client::dns_event(boost::system::error_code const & error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
	{
		//std::cout << "DNS event" << std::endl;
		if(!error)
		{
			boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			endpoint_iterator++;
			socket.async_connect(endpoint, boost::bind(&http_client::connect_event, this, boost::asio::placeholders::error, endpoint_iterator));
			io_service.stop();
		}
		else
			error_occured(http_error_dns);
	}

	void http_client::connect_event(boost::system::error_code const & error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
	{
		if(!error)
		{
			std::ostream request_stream(&request);
			request_stream << method << " " << path << " HTTP/1.1\r\n";
			switch(user_agent)
			{
				case http_user_agent_firefox:
					request_stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.0.8) Gecko/2009032609 Firefox/3.0.8 (.NET CLR 3.5.30729)\r\n";
					break;
			}
			request_stream << "Host: " << host << "\r\n";
			//request_stream << "Accept: */*\r\n";
			request_stream << "Accept: text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1\r\n";
			request_stream << "Accept-Language: en-us,en;q=0.5\r\n";
			request_stream << "Accept-Charset: iso-8859-1, utf-8, utf-16, *;q=0.1\r\n";
			request_stream << "Accept-Encoding: deflate, gzip, x-gzip, identity, *;q=0\r\n";
			//request_stream << "Connection: close\r\n";
			//request_stream << "Connection: Keep-Alive, TE\r\n";
			request_stream << "Connection: close, TE\r\n";
			request_stream << "TE: deflate, gzip, chunked, identity, trailers\r\n";
			if(!referrer.empty())
				request_stream << "Referer: " << referrer << "\r\n";
			if(!cookies.empty())
			{
				request_stream << "Cookie: ";
				bool first = true;
				for(cookie_map::iterator i = cookies.begin(), end = cookies.end(); i != end; i++)
				{
					if(first)
						first = false;
					else
						request_stream << "; ";
					request_stream << i->first << "=" << i->second;
				}
				request_stream << "\r\n";
			}

			std::string post_data;

			if(!form_data.empty())
			{
				std::string const boundary_tag = "------------PDq3XENV0Hx17uo1Qpe5qN";
				for(form_map::iterator i = form_data.begin(), end = form_data.end(); i != end; i++)
					post_data += boundary_tag + "\r\nContent-Disposition: form-data; name=\"" + i->first + "\"\r\n\r\n" + i->second + "\r\n";
				post_data += boundary_tag + "--\r\n";

				std::cout << ail::consolify(post_data) << std::endl;

				request_stream << "Content-Length: " << post_data.size() << "\r\n";
				request_stream << "Content-Type: multipart/form-data; boundary=" << boundary_tag << "\r\n";
				request_stream << "\r\n";
				request_stream << post_data;
			}
			else
				request_stream << "\r\n";

			boost::asio::async_write(socket, request, boost::bind(&http_client::write_event, this, boost::asio::placeholders::error));
			io_service.stop();
		}
		else if(endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
		{
			socket.close();
			boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			endpoint_iterator++;
			socket.async_connect(endpoint, boost::bind(&http_client::connect_event, this, boost::asio::placeholders::error, endpoint_iterator));
			io_service.stop();
		}
		else
		{
			std::cout << "Connect: " << error.message() << std::endl;
			error_occured(http_error_connect);
		}
	}

	void http_client::write_event(boost::system::error_code const & error)
	{
		if(!error)
			read_header_data();
		else
			error_occured(http_error_write);
	}

	void http_client::read_header_data()
	{
		boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&http_client::read_header_event, this, boost::asio::placeholders::error));
		io_service.stop();
	}

	void http_client::read_partial_data()
	{
		boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&http_client::partial_read_event, this, boost::asio::placeholders::error));
		io_service.stop();
	}

	void http_client::read_full_data()
	{
		boost::asio::async_read(socket, response, boost::asio::transfer_all(), boost::bind(&http_client::full_read_event, this, boost::asio::placeholders::error));
		io_service.stop();
	}

	void http_client::read_header_event(boost::system::error_code const & error)
	{
		long const
			ok_code = 200,
			moved_code = 301,
			found_code = 302;

		bool download_finished = error == boost::asio::error::eof;
		if(error && !download_finished)
		{
			error_occured(http_error_read);
			return;
		}

		extract_data(response, buffer);

		std::string const end_of_header_string = "\r\n\r\n";

		std::size_t end_of_header = buffer.find(end_of_header_string);
		if(end_of_header == std::string::npos)
		{
			if(download_finished)
				error_occured(http_error_end_of_header);
			else
				read_header_data();
			return;
		}
		std::string header = buffer.substr(0, end_of_header);
		buffer.erase(0, end_of_header + end_of_header_string.size());

		string_vector tokens = tokenise(header, "\r\n");
		if(tokens.empty())
		{
			error_occured(http_error_header_lacks_lines);
			return;
		}

		//std::cout << "Header: " << header << std::endl;

		std::string const & status_line = tokens[0];

		//std::cout << status_line << std::endl;

		std::string http_code_string;
		if(!extract_string(status_line, " ", " ", http_code_string))
		{
			error_occured(http_error_invalid_status_line);
			return;
		}

		//std::cout << "Code: " << http_code_string << std::endl;

		long http_code;
		if(!string_to_number<long>(http_code_string, http_code))
		{
			error_occured(http_error_invalid_code_string);
			return;
		}

		if(http_code != ok_code && http_code != moved_code && http_code != found_code)
		{
			error_occured(http_error_status_code_error);
			std::cout << buffer << std::endl;
			return;
		}

		encoding = http_encoding_none;

		chunked = false;

		for(std::size_t i = 1, end = tokens.size(); i < end; i++)
		{
			string_vector line_tokens = tokenise(tokens[i], ": ");
			if(line_tokens.size() != 2)
			{
				error_occured(http_error_header_token_error);
				return;
			}
			std::string const & field = line_tokens[0];
			std::string const & value = line_tokens[1];
			if(http_code == ok_code && field == "Content-Encoding")
			{
				if(value == "gzip")
					encoding = http_encoding_gzip;
				else if(value == "deflate")
					encoding = http_encoding_deflate;
			}
			else if(http_code == ok_code && field == "Transfer-Encoding")
			{
				if(value == "chunked")
					chunked = true;
			}
			else if((http_code == moved_code || http_code == found_code) && field == "Location")
			{
				socket.close();

				std::string new_url;

				if(!value.empty() && value[0] == '/')
					new_url = (use_ssl ? "http" : "https") + std::string("://") + host + value;
				else
					new_url = value;
				start_download(new_url);
				return;
			}
		}

		if(download_finished)
			process_data();
		else
		{
			if(encoding == http_encoding_none && has_download_handler)
			{
				process_partial_data();
				read_partial_data();
			}
			else
				read_full_data();
		}
	}

	void http_client::full_read_event(boost::system::error_code const & error)
	{
		if(error != boost::asio::error::eof)
		{
			error_occured(http_error_read);
			return;
		}

		extract_data(response, buffer);
		process_data();
	}

	void http_client::process_partial_data()
	{
		if(file_output.open())
			file_output.write(buffer);

		if(has_download_handler)
			download_handler(*this, buffer);

		buffer.clear();
	}

	void http_client::partial_read_event(boost::system::error_code const & error)
	{
		bool download_finished = error == boost::asio::error::eof;
		if(error && !download_finished)
		{
			error_occured(http_error_read);
			return;
		}

		extract_data(response, buffer);

		process_partial_data();
		
		if(download_finished)
		{
			if(file_output.open())
				file_output.close();

			if(has_download_finished_handler)
				download_finished_handler(*this, buffer);
		}
		else
			read_partial_data();
	}

	void http_client::process_data()
	{
		std::string
			content,
			dechunked_data,

			* input_pointer = &buffer;

		if(chunked)
		{
			if(!process_chunked_data(buffer, dechunked_data, 0))
			{
				error_occured(http_error_chunk);
				return;
			}
			input_pointer = &dechunked_data;
		}

		std::string & input = *input_pointer;

		switch(encoding)
		{
			case http_encoding_none:
				content = input;
				break;

			case http_encoding_gzip:
			case http_encoding_deflate:
				try
				{
					decompress_gzip(input, content);
				}
				catch(exception &)
				{
					error_occured(http_error_compression);
					return;
				}
				break;
		}

		if(has_download_handler)
			download_handler(*this, content);

		if(has_download_finished_handler)
			download_finished_handler(*this, content);

		buffer.clear();
	}

	void http_client::error_occured(http_error_type the_error_code)
	{
		//std::cout << "http_client Error: " << the_error_code << std::endl;
		if(has_error_handler)
			error_handler(*this, the_error_code);
	}

	void http_client::set_error_handler(error_handler_type new_error_handler)
	{
		error_handler = new_error_handler;
		has_error_handler = true;
	}
	
	void http_client::set_download_handler(download_handler_type new_download_handler)
	{
		download_handler = new_download_handler;
		has_download_handler = true;
	}

	void http_client::set_download_finished_handler(download_finished_handler_type new_download_finished_handler)
	{
		download_finished_handler = new_download_finished_handler;
		has_download_finished_handler = true;
	}

	void http_client::set_referrer(std::string const & new_referrer)
	{
		referrer = new_referrer;
	}

	std::string http_client::get_referrer() const
	{
		return referrer;
	}

	std::string http_client::get_url() const
	{
		return url;
	}
}
