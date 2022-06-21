#pragma once

#include<memory>
#include"Request.h"
#include"Response.h"
#include<asio.hpp>


void listener();
void begin();
void runner();


class HttpServer:public std::enable_shared_from_this<HttpServer>
{
private:
	asio::streambuf bufs;
	Request request;
	Response response;
	unsigned endlength;
	char predata;
	asio::steady_timer timer;
public:
	asio::ip::tcp::socket sock;
	
	HttpServer();
	~HttpServer();
	void run();
	void on_read(const std::error_code& ,size_t );
	size_t read_complete(const std::error_code& , size_t );
	void reading();
	void writing();
	size_t writing_complete(const std::error_code& , size_t );
	void on_write(const std::error_code& , size_t );
	std::shared_ptr<HttpServer> getthis();
	void on_wait(const std::error_code& );
	void on_time(const std::error_code&);
};

void on_listen(std::shared_ptr<HttpServer>, const std::error_code&);