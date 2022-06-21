#pragma once
#include<string>
#include<unordered_map>


class Request
{
private:
	std::string request_method;
	std::string request_url;
	std::unordered_map<std::string, std::string> request_header;
	std::string request_body;

public:
	unsigned setRequest(std::streambuf&);
	void setBody(char *,unsigned);
	std::string& getMethod();
	std::string& getUrl();
	std::unordered_map<std::string, std::string>& getHeader();
	std::string& getBody();
};