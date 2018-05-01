#ifndef SERVER_BASE_HPP
#define SERVER_BASE_HPP

#include <boost/asio.hpp>
#include <thread>


namespace WebServer
{
	//对客户端的请求信息进行解析
	struct Request 
	{
		std::string methd; //请求方法(POST GET)
		std::string path;  //请求路径
		std::string version;//协议版本
		
		std::share_prt<std::istream> content;//请求信息
		std::unordermap<std::string, std::string> header; // 利用无序容器对header头部进行处理
		std::smatch path_match;  //利用正则表达式，对请求路径进行解析
	}
	
	
	
	template<typename socket_type>
	class ServerBase
	{
	public:
		ServerBase();
		~ServerBase();
		//启动服务器
		void start();
		
	protected:  //对子类可见，外部不可见
		virtual void accept(){};
		void process_request_and_respond(share_prt<socket_type> socket) const;
	
		//asio库中的io_service是调度器，所有异步的IO事件都要通过它来分发处理
		//需要IO的对象的构造函数，都需要传入一个io_service对象
		boost::asio::io_service m_io_service;
		
		//IP地址、端口号、协议版本构成一个endpoint,并通过这个endpoint在服务器生成
		//tcp::acceptor 对象，并在指定端口上等待连接
		boost::asio::ip::tcp::endpoint m_endpoint;
		
		//一个accpetor对象的构造需要io_service和endpoint两个参数
		boost::asio::ip::tcp::acceptor m_accpetor;
		
		//服务器线程  监听到就分配一个线程进行处理
		//优化：可使用线程池
		size_t m_NumThread; 
		std::vector<thread> m_vecThread; //线程队列
		
		//所有监听的资源, 都在Verctor 尾部添加，并在start()中创建
		std::vector<socket_type::iterator> m_AllResources;
	}
	
	typedef std::share_prt<ServerBase> ServerBasePtr;
	
	//通过模板偏特化来实现不同的子类
	template <typename socket_type>
	class Server:public ServerBase<socket_type> {};
	
}