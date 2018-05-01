#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_
#include <iostream>

namespace Response
{
	const std::string ok = " 200 OK\r\n";
	const std::string created = " 201 Created\r\n";
	const std::string accepted = " 202 Accepted\r\n";
	const std::string no_content = " 204 No Content\r\n";
	const std::string multiple_choices = " 300 Multiple Choices\r\n";
	const std::string moved_permanently = " 301 Moved Permanently\r\n";
	const std::string moved_temporarily = " 302 Moved Temporarily\r\n";
	const std::string not_modified = " 304 Not Modified\r\n";
	const std::string bad_request = " 400 Bad Request\r\n";
	const std::string unauthorized = " 401 Unauthorized\r\n";
	const std::string forbidden = " 403 Forbidden\r\n";
	const std::string not_found = " 404 Not Found\r\n";
	const std::string internal_server_error = " 500 Internal Server Error\r\n";
	const std::string not_implemented = " 501 Not Implemented\r\n";
	const std::string bad_gateway = " 502 Bad Gateway\r\n";
	const std::string service_unavailable = " 503 Service Unavailable\r\n";


	enum Response_status
	{
		status_ok = 200,
		status_created = 201,
		status_accepted = 202,
		status_no_content = 204,
		status_multiple_choices = 300,
		status_moved_permanently = 301,
		status_moved_temporarily = 302,
		status_not_modified = 304,
		status_bad_request = 400,
		status_unauthorized = 401,
		status_forbidden = 403,
		status_not_found = 404,
		status_internal_server_error = 500,
		status_not_implemented = 501,
		status_bad_gateway = 502,
		status_service_unavailable = 503
	};


	class HTTPResponse
	{
	public:
		HTTPResponse();
		~HTTPResponse();

	public:
		typedef std::vector<std::pair<std::string, std::string>> HTTPResponseHeader;

		std::string m_version;
		int m_rescode;

		std::string status_string;

		HTTPResponseHeader m_header; //响应头

		std::string m_content; //响应报文
	};

	typedef std::shared_ptr<HTTPResponse> ptrHTTPResponse;

}
#endif  //_HTTP_RESPONSE_H_