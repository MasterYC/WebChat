#include"Response.h"
#include"ConntextType.h"
#include"Controller.h"
#include<iostream>
#include<fstream>
#include<filesystem>

using namespace std;
void Response::getResponse(std::streambuf& bufs)
{
	ostream osm(&bufs);
	osm << response_line << "\r\n";
	for (auto i = response_header.begin(); i != response_header.end(); i++)
	{
		osm << i->first << ':' << i->second << "\r\n";
	}
	osm << "\r\n" << response_body;
}

void Response::setResponse(Request& request)
{
	response_line.clear();
	response_header.clear();
	response_body.clear();
	string& url = request.getUrl();
	unordered_map<string,string>& header = request.getHeader();

	response_line = "HTTP/1.1 200 OK";
	response_header.insert({ "Accept-Ranges","bytes" });
	response_header.insert({ {"Connection","keep-alive"},{"Keep-Alive","timeout=60"} ,{"Server","YC-WebServer"} });

	if (Controller::getInstance().getController(url, Func))
	{
		Func(request, *this);
		response_header.insert({ "Content-Length",to_string(response_body.size()) });
		return;
	}

	url = "httpdocs" + url;
	if (url == "httpdocs/")url = url+"index.html";
	
	ifstream ifs(url, ios_base::binary);
	if (!ifs.is_open())
	{
		url = "httpdocs/404.html";
		ifs.open(url, ios_base::binary);
		response_line = "HTTP/1.1 404 Not Found";
	}
	string temp;
	for (int i = 0; i < url.length(); i++)
	{
		if (url[i] == '.')
		{
			temp.assign(url, i + 1, url.length() - i);
			break;
		}
	}
	response_header.insert({ "Content-Type",ContextType::getInstance().contexttype[temp]});

	if (header.contains("Range"))
	{
		temp = header["Range"].erase(0, 6);
		temp.pop_back();
		unsigned long started =stoul(temp);
		ifs.seekg(started);
		char* tt = new char[1024*1024 * 30];
		ifs.read(tt, 1024*1024* 30);
		long long int sizess = ifs.gcount();
		response_body = string(tt, sizess);
		delete[] tt;
		temp = "bytes "+temp+'-'+ to_string(started+sizess-1)+'/';
		filesystem::path path(url);
		temp.append(to_string(filesystem::file_size(path)));
		response_header.insert({ "Content-Range",temp });
		response_line = "HTTP/1.1 206 Partial Content";
	}
	else
	{
		filesystem::path path(url);
		char* tt = new char[filesystem::file_size(path)];
		ifs.read(tt,filesystem::file_size(path));	
		response_body = string(tt, ifs.gcount());
		delete[] tt;
	}



	response_header.insert({ "Content-Length",to_string(response_body.size())});

}

void Response::setHeader(string& header,string& message)
{
	//response_header[header]
}

void Response::setBody(const std::string& body)
{
	response_body.append(body);
}


