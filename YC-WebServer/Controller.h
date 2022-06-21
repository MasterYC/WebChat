#pragma once
#include<unordered_map>
#include<functional>
#include"Response.h"
#include"Request.h"
class Controller
{
private:
	std::unordered_map < std::string, std::function<void(Request&, Response&)> > controller;
	static Controller instance;

	Controller()=default;
	~Controller() = default;
	Controller(const Controller&) = default;
	Controller& operator=(const Controller&) = default;

public:
	static Controller& getInstance();
	bool setController(const std::string&, const std::function<void(Request&, Response&)>&);
	bool getController(const std::string&,  std::function<void(Request&, Response&)>&);
};

bool setController(const std::string&, const std::function<void(Request&, Response&)>&);