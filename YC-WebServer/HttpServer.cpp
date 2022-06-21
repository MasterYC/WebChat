
#include "HttpServer.h"
#include<functional>
#include<iostream>
#include<string>
#include<thread>
#include<chrono>
using namespace asio;
using namespace std;

io_context service;//初始化可以指定service运行在多少个线程下，只是建议，不一定执行，本机最多为三个线程
ip::tcp::endpoint ep(ip::tcp::v4(),80);
ip::tcp::acceptor acp(service,ep);
void begin()
{
	listener();
	std::thread t1(runner);
	std::thread t2(runner);
	runner();
}

void runner()
{
	service.run();
}

void listener()
{
	auto httpserver = make_shared<HttpServer>();
	acp.async_accept(httpserver->sock, bind(on_listen, httpserver, std::placeholders::_1));
}

void on_listen(shared_ptr<HttpServer> httpserver,const error_code &ec)
{
	listener();
	if (ec)
	{
		//cout << "accept error:" << ec<<endl;
		return;
	}
	cout << "connect" << endl;
	httpserver->run();
}



HttpServer::HttpServer() :sock(service),endlength(0),predata(0),timer(service)
{
}

HttpServer::~HttpServer()
{
	if(sock.is_open())sock.close();
	cout << "close" << endl;
}

shared_ptr<HttpServer>  HttpServer::getthis()
{
	return shared_from_this();
}

void HttpServer::run()
{
	timer.expires_from_now(std::chrono::seconds(60));
	timer.async_wait(bind(&HttpServer::on_time, shared_from_this(), std::placeholders::_1));
	sock.async_wait(ip::tcp::socket::wait_read, bind(&HttpServer::on_wait, shared_from_this(), std::placeholders::_1));
}



void HttpServer::reading()
{
	endlength = 0;
	predata = 0;
	async_read(sock, bufs,bind(&HttpServer::read_complete,shared_from_this(), std::placeholders::_1, std::placeholders::_2), bind(&HttpServer::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}
size_t HttpServer::read_complete(const error_code& err, size_t bytes)
{
	if (err)
	{
		//cout << "reading error:" << err << endl;
		return 0;
	}
	if (bytes == 0)return 1;
	char temp = *(buffers_begin(bufs.data())+bytes-1);
	if (predata == '\r' && temp == '\n')endlength++;
	else if (predata == '\n' && temp == '\r')endlength++;
	else endlength = 0;
	if (endlength == 3)return 0;
	predata = temp;	
	return 1;
}
void HttpServer::on_read(const error_code& ec, size_t bytes)
{
	if (ec)
	{
		//cout << "read error:" << ec << endl;
		return;
	};
	endlength = request.setRequest(bufs);
	if (endlength)
	{
		char* temp = new char[endlength];
		sock.read_some(buffer(temp,endlength));
		request.setBody(temp,endlength);
		delete[] temp;
	}
	writing();
}

void HttpServer::writing()
{
	response.setResponse(request);
	response.getResponse(bufs);
	async_write(sock, bufs,  bind(&HttpServer::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

size_t HttpServer::writing_complete(const error_code& err, size_t bytes)
{	
	return bufs.size();
}

void HttpServer::on_write(const error_code& er, size_t bytes)
{
	if (er)
	{
		//cout << "write error:" << er << endl;
		return;
	}
	timer.expires_from_now(std::chrono::seconds(60));
	timer.async_wait(bind(&HttpServer::on_time, shared_from_this(), std::placeholders::_1));
	sock.async_wait(ip::tcp::socket::wait_read, bind(&HttpServer::on_wait,shared_from_this(), std::placeholders::_1));
}
void HttpServer::on_time(const error_code& ec)
{
	if (ec)
	{
		//cout << "time error:" << ec << endl;
		return;
	}
	sock.close();
}

void HttpServer::on_wait(const error_code& err)
{
	if (err)
	{
		//cout << "wait error:" << err << endl;
		return;
	}
	timer.cancel();
	reading();
}
