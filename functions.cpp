#include <windows.h>
#include <TlHelp32.h>
#include "summoner.h"
#include <string>


unsigned long get_module(unsigned long pid, char *module_name, unsigned long *size)
{
	void *snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 me32;
	me32.dwSize = sizeof (MODULEENTRY32);

	while (Module32Next(snapshot, &me32))
	{
		if(strcmp( me32.szModule, module_name)==0 )
			{
			if(size != 0) *size = me32.modBaseSize;
			return (unsigned long)me32.modBaseAddr;
			}
	}return NULL;
}
float math::sqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5F;
	y = number;
	i = * (long*) &y;
	i = 0x5F3759DF  -	(	i	>>	1);
	y = * ( float*) &i;
	y = y * ( threehalfs *(x2*y*y));

	return y;
}

bool CheckSubstring(std::string firstString, std::string secondString) {
    if (secondString.size() > firstString.size())
        return false;

    for (unsigned int i = 0; i < firstString.size(); i++){
        unsigned int j = 0;
        // If the first characters match
        if (firstString[i] == secondString[j]){
            int k = i;
            while (firstString[i] == secondString[j] && j < secondString.size()){
                j++;
                i++;
            }
            if (j == secondString.size())
                return true;
            else // Re-initialize i to its original value
                i = k;
        }
    }
    return false;
}