#include <boost\asio.hpp>
#include <regex>
#include <fstream>
#include <boost\regex.hpp>
#include "HTTPServer.h"


using namespace Response;
using namespace Request;

HttpServer::HttpServer(unsigned int port)
	:m_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port),
	m_acceptor(m_io_service)
{
	init_resource();

}

HttpServer::~HttpServer()
{

}

void HttpServer::start()
{
	m_RootPath = "F:\\GitHub\\git\\Web_Server\\www";
	accept();
	m_io_service.run(); //�¼�ѭ��
}

//��������
ptrHTTPRequest HttpServer::PraseRequest(std::istream &read_stream)
{
	ptrHTTPRequest request = std::make_shared<HTTPRequest>();

	/*ʹ�������������н��� (GET)
	*�����У�
	*����ͷ����
	*�����ģ�
	*/
	static boost::regex request_line = boost::regex("^([^ ]* ([^ ]*) HTTP/([^ ]*)$)"); 	//������ƥ�����
	static boost::regex request_header = boost::regex("^([^:]*): ?(.*)$");    //����ͷƥ�����

	boost::smatch request_match;  //ƥ����

	std::string line;
	bool MatchFlag = true;
	int lineNo = 0;
	while (MatchFlag)
	{
		std::getline(read_stream, line);  //��gcc / g++�ж��뻻�з�
		line.pop_back(); // �Ƴ����з�
		lineNo++;

		//����������
		if (1 == lineNo)
		{
			MatchFlag = boost::regex_match(line, request_match, request_line);
			if (MatchFlag)
			{
				request->m_method = request_match[1];
				request->m_path = request_match[2];
				request->m_version = request_match[3];

				if (request->m_path.find('?') != std::string::npos)
				{
					request->m_path = (request->m_path).substr(0, request->m_path.find('?'));
				}
				//url���һλ�ַ�Ϊ/ʱ���Զ�����index.html
				if (request->m_path[request->m_path.size() - 1] == '/')
				{
					request->m_path += "index.html";
				}
			}
			else
			{
				MatchFlag = false;
				std::cout << "Requset error!" << std::endl;
				return nullptr;
			}
		}
		//��������ͷ
		else
		{
			//����
			if (!line.compare("\r\n\r\n"))
			{
				MatchFlag = boost::regex_match(line, request_match, request_header);
				request->m_heard[request_match[1]] = request_match[2];
			}
			else
			{
				MatchFlag = false;
				std::cout << "Requset header read out!" << std::endl;
			}
		}
	}

	request->m_content = std::make_shared<std::istream>(read_stream.get());

	return request;
}

//������Ӧ
ptrHTTPResponse HttpServer::GetResponse(ptrHTTPRequest& request)
{
	ptrHTTPResponse response = std::make_shared<HTTPResponse>();
	std::string name;
	std::string value;
	std::pair<std::string, std::string> response_header;
	response->m_version = request->m_version;

	if (request->m_path.empty())
	{
		response->m_rescode = status_bad_request;
		response->status_string = "HTTP/" + response->m_version + bad_request;
	}

	//��չ��
	std::size_t last_slash_pos = request->m_path.find_last_of("/"); //���һ��/
	std::size_t last_dot_pos = request->m_path.find_last_of(".");  //����Ը�.
	std::string extension;
	if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
	{
		extension = request->m_path.substr(last_dot_pos + 1);
	}

	//��Ӧ����
	int length;
	std::string strFullPath = m_RootPath + request->m_path;
	std::ifstream is(strFullPath.c_str(), std::ios::in | std::ios::binary);
	std::stringstream buffer;
	buffer << is.rdbuf();
	response->m_content = buffer.str();

	//��Ӧ��
	response->m_rescode = status_ok;
	response->status_string = "HTTP/" + response->m_version + ok;

	//��Ӧͷ
	response->m_header[0].first = "Content-Length";
	response->m_header[0].second = response->m_content.size();
	response->m_header[1].first = "Content-Type";
	response->m_header[1].second = extension;

	return response;

} 

//����Ӧת����boost::asio::buffer
std::vector<boost::asio::const_buffer> HttpServer::to_buffer(ptrHTTPResponse& response)
{
	std::vector<boost::asio::const_buffer> buffers;

	//��Ӧ��
	buffers.push_back(boost::asio::buffer(response->status_string));

	//��Ӧͷ
	for (std::size_t i = 0; i < response->m_header.size(); i++)
	{
		std::string response_header;
		response_header = response->m_header[i].first + ":" + response->m_header[i].second;
		buffers.push_back(boost::asio::buffer(response_header));
	}

	//����
	buffers.push_back(boost::asio::buffer("\r\n"));

	//��Ӧ����
	buffers.push_back(boost::asio::buffer(response->m_content));
	return buffers;
}


void HttpServer::accept()
{
	sock_ptr sock(new socket_type(m_io_service));

	m_acceptor.async_accept(*sock, std::bind(&HttpServer::handle_accept, this,
		boost::asio::placeholders::error, sock));
}

void HttpServer::handle_accept(const boost::system::error_code& err, sock_ptr sock)
{
	if (err)
	{
		return;
	}
	//��ӡ��־...

	//���տͻ�����������
	sock->async_read_some(*read_buff,std::bind(&HttpServer::handle_read_request,
		this, boost::asio::placeholders::error, sock));
}

void HttpServer::handle_read_request(const boost::system::error_code& err, sock_ptr sock)
{
	if (!err)
	{
		size_t iRequestSize = read_buff->size();
		std::istream read_stream(read_buff.get());

		size_t iRequestContentSize = iRequestSize;

		//��������
		ptrHTTPRequest ptrRequest = PraseRequest(read_stream);

		//������Ӧ����Ӧ
		ptrHTTPResponse ptrResponse = GetResponse(ptrRequest);

		//����Ӧ�����boost::asio::buffer
		to_buffer(ptrResponse);

		//���͸��ͻ���
		sock->async_write_some(write_buff, std::bind(&HttpServer::handle_write_response,
			this, boost::asio::placeholders::error, sock));
	}
}

void HttpServer::handle_write_response(const boost::system::error_code& err, sock_ptr sock)
{
	//���ͳɹ�����ر����ӣ�����keep-Aliving���
	if (!err)
	{
		boost::system::error_code ignored_ec;
		sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}

	start();//���½�������
}
