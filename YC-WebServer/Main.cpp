#include"HttpServer.h"
#include"Controller.h"
#include <iostream>


using namespace std;

void fff(int a,Request& request, Response& response)
{
	response.setBody(to_string(a));
}





int main()
{
	


	setController("/hello", bind(fff,50,placeholders::_1,placeholders::_2));
	begin();
}