#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_
#include <iostream>
#include <string>
#include <unordered_map>
#include <regex>
#include "HTTPServer.h"

namespace Request
{


	//http 协议请求报文的封装
	class HTTPRequest
	{
	public:
		HTTPRequest(HttpServer *ptrHTTPSev);
		~HTTPRequest();

		void parse_request();
		void parse_request(std::string *url);

		bool KeepAlive();
		std::string RequestMethod()
		{
			return m_method;
		}

	public:
		std::string m_method;     //请求方法  GET / POST
		std::string m_version;    //http协议版本
		std::string m_path;        //请求的uri
		std::string m_host;       //请求的主机名

		std::unordered_map<std::string, std::string> m_heard;      //请求头
		std::string m_connection;  //连接状态

		std::shared_ptr<std::istream> m_content; //请求报文
	};
	typedef std::shared_ptr<HTTPRequest> ptrHTTPRequest;
}

#endif //_HTTP_REQUEST_H_
