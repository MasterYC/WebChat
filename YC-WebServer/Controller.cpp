#include"Controller.h"
using namespace std;
Controller Controller::instance;
Controller& Controller::getInstance()
{
	return instance;
}

bool Controller::setController(const std::string& path,  const std::function<void(Request&, Response&)>& func)
{
	if (controller.contains(path))return false;
	controller[path] = func;
	return true;
}

bool Controller::getController(const std::string& path, std::function<void(Request&, Response&)> &func)
{
	if (!controller.contains(path))return false;
	func = controller[path];
	return true;
}
bool setController(const std::string& path, const std::function<void(Request&, Response&)>& func)
{
	return Controller::getInstance().setController(path, func);
}