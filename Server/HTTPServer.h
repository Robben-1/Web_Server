#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <iostream>
#include <boost\asio.hpp>
#include <string>
#include <set>
#include <vector>
#include "http_mark.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"


using namespace Response;
using namespace Request;

class HttpServer
{
	typedef boost::asio::ip::tcp::endpoint endpoint_type;
	typedef boost::asio::ip::tcp::socket socket_type;
	typedef boost::asio::ip::address address_type;
	typedef std::shared_ptr<socket_type> sock_ptr;
	typedef std::map<std::string, std::unordered_map<std::string,
		std::function<void(std::ostream&, ptrHTTPRequest&)>>> resource_type;

public:
	HttpServer(unsigned int port);
	~HttpServer();
	void start(); //启动服务
	void close(); //关闭服务
	void accept();

	ptrHTTPRequest PraseRequest(std::istream &read_stream);  //解析请求
	ptrHTTPResponse GetResponse(ptrHTTPRequest& request);  //生成响应
	std::vector<boost::asio::const_buffer> to_buffer(ptrHTTPResponse& response);

	resource_type m_resource;
	resource_type m_default_resource;

protected:
	//连接后--异步函数
	void handle_accept(const boost::system::error_code& err, sock_ptr sock);

	//发送响应--异步函数
	void handle_write_response(const boost::system::error_code& err, sock_ptr sock);

	//读取请求--异步函数
	void handle_read_request(const boost::system::error_code& err, sock_ptr sock);

private:
	boost::asio::io_service m_io_service;	    //io_service为Proactor
	boost::asio::ip::tcp::endpoint m_endpoint;  //端点
	boost::asio::ip::tcp::acceptor m_acceptor;  //连接事件处理器，在服务端中存在
	std::set<sock_ptr> m_setsock;

	void init_resource();//初始化服务器上资源

	std::vector<resource_type::iterator> all_resources;

	std::string m_RootPath;
	std::string m_notFoundFile;

	//涉及到多线程时，会有问题
	std::shared_ptr<boost::asio::streambuf> read_buff;  //请求缓冲区
	std::vector<boost::asio::const_buffer> write_buff; //响应缓冲区
};

//post请求
std::string Post(std::string url);

//get请求
std::string Get(std::string url);

#endif // _HTTP_SERVER_H_