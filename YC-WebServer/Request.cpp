#include"Request.h"
#include<iostream>
using namespace std;
unsigned Request::setRequest(std::streambuf &bufs)
{
	istream ism(&bufs);
	request_header.clear();
	request_body.clear();
	request_method.clear();
	request_url.clear();
	string temp;
	getline(ism, temp);
	int temps = 0;
	int i = 0;
	//读取请求方法和请求的url
	for ( i = 0; i < temp.size();i++)
	{
		if (temp[i] == ' ')
		{
			request_method.assign(temp, temps, i-temps);
			break;
		}
	}
	temps = i + 1;
	for (i=temps; i < temp.size(); i++)
	{
		if (temp[i] == ' ')
		{
			request_url.assign(temp, temps, i - temps);
		}
	}

	//读取请求头，存入map
	while (getline(ism, temp))
	{
		if (temp[0] =='\r')break;
		temps = 0;
		string header;
		string message;
		for (i = 0; i < temp.size(); i++)
		{
			if (temp[i] == ':')
			{
				header.assign(temp, temps, i - temps);
				break;
			}
		}
		temps = i + 2;
		for (i = temps; i < temp.size(); i++)
		{
			if (temp[i] == '\r')
			{
				message.assign(temp, temps, i - temps);
				break;
			}
		}
		request_header.insert({ header, message });
	}



	//将url中的中文改为utf-8格式
	temp = request_url;
	request_url.clear();
	for (int i = 0; i < temp.length();i++)
	{
		if (temp[i] == '%')
		{
			if (temp[i + 1] >= 65)temp[i + 1] -= 55;
			else temp[i+1]-=48;
			if (temp[i + 2] >= 65)temp[i + 2] -= 55;
			else temp[i + 2] -= 48;
			char aa = temp[i + 1] * 16 + temp[i + 2];
			request_url += aa;
			i=i+2;
			continue;
		}
		request_url+=temp[i];
	}
	for ( i = 0; i < request_url.length(); i++)
	{
		if (request_url[i] == '?')
		{
			request_body.assign(request_url, i + 1, request_url.length() - i);
			request_url.assign(request_url, 0, i);
			break;
		}
	}
	//cout << request_url << endl<<request_body<<endl;
	if (request_header.contains("Content-Length"))
	{
		temp = request_header["Content-Length"];
		return stoul(temp);
	}
	return 0;


}

void Request::setBody(char *temp,unsigned length)
{
	request_body.append(temp,length);
	//cout << request_body << endl;
}

string& Request::getMethod()
{
	return request_method;
}

string& Request::getUrl()
{
	return request_url;
}

unordered_map<string, string>& Request::getHeader()
{
	return request_header;
}

string& Request::getBody()
{
	return request_body;
}
