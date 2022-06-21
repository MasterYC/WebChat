#pragma once
#include<unordered_map>
#include<string>


class ContextType
{
public:
	static ContextType& getInstance();
	std::unordered_map<std::string, std::string> contexttype;
private:
	ContextType();
	~ContextType() = default;
	ContextType(const ContextType&) = default;
	ContextType& operator=(const ContextType&) = default;

	static ContextType instance;

};