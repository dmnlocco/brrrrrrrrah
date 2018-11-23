#ifndef SUMMONER_H
#define SUMMONER_H

#include <string>

unsigned long get_module(unsigned long pid, char *module_name, unsigned	long * size = 0);
bool CheckSubstring(std::string firstString, std::string secondString);

class math
{
public:
	static float sqrt(float number);

};
#endif