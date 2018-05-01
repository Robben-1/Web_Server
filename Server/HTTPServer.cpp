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
	m_io_service.run(); //事件循环
}

//解析请求
ptrHTTPRequest HttpServer::PraseRequest(std::istream &read_stream)
{
	ptrHTTPRequest request = std::make_shared<HTTPRequest>();

	/*使用正则对请求进行解析 (GET)
	*请求行：
	*请求头部：
	*请求报文：
	*/
	static boost::regex request_line = boost::regex("^([^ ]* ([^ ]*) HTTP/([^ ]*)$)"); 	//请求行匹配规则
	static boost::regex request_header = boost::regex("^([^:]*): ?(.*)$");    //请求头匹配规则

	boost::smatch request_match;  //匹配结果

	std::string line;
	bool MatchFlag = true;
	int lineNo = 0;
	while (MatchFlag)
	{
		std::getline(read_stream, line);  //在gcc / g++中读入换行符
		line.pop_back(); // 移除换行符
		lineNo++;

		//解析请求行
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
				//url最后一位字符为/时，自动加上index.html
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
		//解析请求头
		else
		{
			//空行
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

//生成响应
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

	//扩展码
	std::size_t last_slash_pos = request->m_path.find_last_of("/"); //最后一个/
	std::size_t last_dot_pos = request->m_path.find_last_of(".");  //最后以个.
	std::string extension;
	if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
	{
		extension = request->m_path.substr(last_dot_pos + 1);
	}

	//响应报文
	int length;
	std::string strFullPath = m_RootPath + request->m_path;
	std::ifstream is(strFullPath.c_str(), std::ios::in | std::ios::binary);
	std::stringstream buffer;
	buffer << is.rdbuf();
	response->m_content = buffer.str();

	//响应码
	response->m_rescode = status_ok;
	response->status_string = "HTTP/" + response->m_version + ok;

	//响应头
	response->m_header[0].first = "Content-Length";
	response->m_header[0].second = response->m_content.size();
	response->m_header[1].first = "Content-Type";
	response->m_header[1].second = extension;

	return response;

} 

//将响应转换成boost::asio::buffer
std::vector<boost::asio::const_buffer> HttpServer::to_buffer(ptrHTTPResponse& response)
{
	std::vector<boost::asio::const_buffer> buffers;

	//响应行
	buffers.push_back(boost::asio::buffer(response->status_string));

	//响应头
	for (std::size_t i = 0; i < response->m_header.size(); i++)
	{
		std::string response_header;
		response_header = response->m_header[i].first + ":" + response->m_header[i].second;
		buffers.push_back(boost::asio::buffer(response_header));
	}

	//空行
	buffers.push_back(boost::asio::buffer("\r\n"));

	//响应报文
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
	//打印日志...

	//接收客户端请求数据
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

		//解析请求
		ptrHTTPRequest ptrRequest = PraseRequest(read_stream);

		//生成相应的响应
		ptrHTTPResponse ptrResponse = GetResponse(ptrRequest);

		//将响应构造成boost::asio::buffer
		to_buffer(ptrResponse);

		//发送给客户端
		sock->async_write_some(write_buff, std::bind(&HttpServer::handle_write_response,
			this, boost::asio::placeholders::error, sock));
	}
}

void HttpServer::handle_write_response(const boost::system::error_code& err, sock_ptr sock)
{
	//发送成功，则关闭连接，忽略keep-Aliving情况
	if (!err)
	{
		boost::system::error_code ignored_ec;
		sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}

	start();//重新接收连接
}
