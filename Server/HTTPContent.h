#ifndef _HTTP_CONTENT_H_
#define _HTTP_CONTENT_H_
#include <iostream>


//封装响应报文类
class HTTPContent
{
public:
	HTTPContent();
	~HTTPContent();


private:
	std::string m_strHTMLFileName;
	std::string m_strContentType;
	std::shared_ptr<std::istream> m_ContentStream;
};

#endif  //_HTTP_CONTENT_H_