#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_
#include <iostream>
#include <string>
#include <unordered_map>
#include <regex>
#include "HTTPServer.h"

namespace Request
{


	//http Э�������ĵķ�װ
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
		std::string m_method;     //���󷽷�  GET / POST
		std::string m_version;    //httpЭ��汾
		std::string m_path;        //�����uri
		std::string m_host;       //�����������

		std::unordered_map<std::string, std::string> m_heard;      //����ͷ
		std::string m_connection;  //����״̬

		std::shared_ptr<std::istream> m_content; //������
	};
	typedef std::shared_ptr<HTTPRequest> ptrHTTPRequest;
}

#endif //_HTTP_REQUEST_H_
