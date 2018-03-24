
程序完成流程：
1.HTTPServer对象启动并监控网络模块；
2.当新的客户端连接建立以后,HTTPServer对象开始读取这个新连接，读取完整请求；
3.收到完整请求后，以此为参数，建立HTTPRequest对象；
4.分析HTTPRequest对象,生成正确的HTTPResponse对象；
5.将HTTPResponse对象的内容，发送到客户端；
6.数据发送完之后，关闭连接


