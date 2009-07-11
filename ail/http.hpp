#ifndef AIL_HTTP_HPP
#define AIL_HTTP_HPP

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <ail/exception.hpp>
#include <ail/types.hpp>
#include <ail/file.hpp>

namespace ail
{
	enum http_error_type
	{
		http_error_dns,
		http_error_connect,
		http_error_read,
		http_error_write,
		http_error_end_of_header,
		http_error_header_lacks_lines,
		http_error_invalid_status_line,
		http_error_invalid_code_string,
		http_error_status_code_error,
		http_error_header_token_error,
		http_error_chunk,
		http_error_compression,
		http_error_header_size,
		http_error_encoding_error
	};

	enum http_encoding_type
	{
		http_encoding_none,
		http_encoding_gzip,
		http_encoding_deflate
	};

	enum http_user_agent_type
	{
		http_user_agent_none,
		http_user_agent_firefox
	};

	class http_client: boost::noncopyable
	{
	public:
		typedef boost::function<void (http_client &, http_error_type)> error_handler_type;
		typedef boost::function<void (http_client &, std::string const &)> download_handler_type;
		typedef boost::function<void (http_client &, std::string const &)> download_finished_handler_type;
		typedef std::map<std::string, std::string> cookie_map;
		typedef std::map<std::string, std::string> form_map;

		http_client();
		http_client(boost::asio::io_service & io_service);
		void use_post();
		void add_post_data(std::string const & field, std::string const & value);
		void start_download(std::string const & new_url);
		bool start_download(std::string const & new_url, std::string const & file_name);

		void set_error_handler(error_handler_type new_error_handler);
		void set_download_handler(download_handler_type new_download_handler);
		void set_download_finished_handler(download_finished_handler_type new_download_finished_handler);
		void set_referrer(std::string const & new_referrer);

		std::string get_referrer() const;

		std::string get_url() const;

	private:
		boost::asio::io_service & io_service;

		boost::asio::ip::tcp::resolver resolver;
		boost::asio::ip::tcp::socket socket;
		boost::asio::streambuf
			request,
			response;

		http_encoding_type encoding;

		bool
			use_ssl,
			chunked;

		std::string
			url,
			host,
			path,
			referrer,

			buffer,

			method;

		file file_output;

		cookie_map cookies;
		form_map form_data;

		error_handler_type error_handler;
		download_handler_type download_handler;
		download_finished_handler_type download_finished_handler;
		http_user_agent_type user_agent;

		bool
			has_error_handler,
			has_download_handler,
			has_download_finished_handler;

		void dns_event(boost::system::error_code const & error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
		void connect_event(boost::system::error_code const & error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
		void write_event(boost::system::error_code const & error);
		void read_header_event(boost::system::error_code const & error);
		void full_read_event(boost::system::error_code const & error);
		void partial_read_event(boost::system::error_code const & error);
		void error_occured(http_error_type the_error_code);

		void read_header_data();
		void read_partial_data();
		void read_full_data();

		void process_data();
		void process_partial_data();
	};
}

#endif
