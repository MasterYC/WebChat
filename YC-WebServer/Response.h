#pragma once
#include<string>
#include<unordered_map>
#include"Request.h"
#include<functional>

class Response
{
private:
	std::string response_line;
	std::unordered_map<std::string, std::string> response_header;
	std::string response_body;
	std::function<void(Request&, Response&)> Func;
public:
	void getResponse(std::streambuf&);
	void setResponse(Request&);
	void setHeader(std::string&,std::string&);
	void setBody(const std::string&);
};

