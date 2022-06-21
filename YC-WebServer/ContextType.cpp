#include"ConntextType.h"
#include<fstream>
using namespace std;
ContextType ContextType::instance;

ContextType& ContextType::getInstance()
{
	return instance;
}

ContextType::ContextType()
{
	ifstream ifs("file/Content-Type.txt");
	string type;
	string value;
	while (!ifs.eof())
	{
		ifs >> type;
		ifs >> value;
		contexttype.insert({ type,value });
	}
	ifs.close();
	contexttype["html"] = "text/html; charset=UTF-8";
	contexttype[""] = "application/x-";

}