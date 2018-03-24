
PROJECT(web_server)
cmake_minimum_required (VERSION 2.6)
SET(CMAKE_BUILE_TYPE DEBUG)

INCLUDE_DIRECTORIES("D:\\boost\\boost_1_64_0")  #添加头文件目录

LINK_DIRECTORIES("D:\\boost\\boost_1_64_0\\bin\\vc14\\lib") #添加库文件目录

SET(SRC_LIST
	Server/HTTPServer.cpp
	Server/HTTPServer.h
	Server/http_mark.h
	Server/HTTPRequest.h
	Server/HTTPRequest.cpp
	Server/HTTPResponse.h
	Server/HTTPResponse.cpp
	main.cpp
	)
	
	

	
ADD_EXECUTABLE(web_server ${SRC_LIST})

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/lib)



#ADD_SUBDIRECTORY(src)

