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
	void start(); //��������
	void close(); //�رշ���
	void accept();

	ptrHTTPRequest PraseRequest(std::istream &read_stream);  //��������
	ptrHTTPResponse GetResponse(ptrHTTPRequest& request);  //������Ӧ
	std::vector<boost::asio::const_buffer> to_buffer(ptrHTTPResponse& response);

	resource_type m_resource;
	resource_type m_default_resource;

protected:
	//���Ӻ�--�첽����
	void handle_accept(const boost::system::error_code& err, sock_ptr sock);

	//������Ӧ--�첽����
	void handle_write_response(const boost::system::error_code& err, sock_ptr sock);

	//��ȡ����--�첽����
	void handle_read_request(const boost::system::error_code& err, sock_ptr sock);

private:
	boost::asio::io_service m_io_service;	    //io_serviceΪProactor
	boost::asio::ip::tcp::endpoint m_endpoint;  //�˵�
	boost::asio::ip::tcp::acceptor m_acceptor;  //�����¼����������ڷ�����д���
	std::set<sock_ptr> m_setsock;

	void init_resource();//��ʼ������������Դ

	std::vector<resource_type::iterator> all_resources;

	std::string m_RootPath;
	std::string m_notFoundFile;

	//�漰�����߳�ʱ����������
	std::shared_ptr<boost::asio::streambuf> read_buff;  //���󻺳���
	std::vector<boost::asio::const_buffer> write_buff; //��Ӧ������
};

//post����
std::string Post(std::string url);

//get����
std::string Get(std::string url);

#endif // _HTTP_SERVER_H_