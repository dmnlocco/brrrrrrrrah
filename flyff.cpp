#include <Windows.h>
#include <cmath>
#include "flyff.h"
#include "summoner.h"
#include <iostream>
#include <conio.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

// dont put nothing after definition as its macro not regular c code
#define OFFSET_NAME 0x13DC
#define OFFSET_ADMIN 0x77C

using namespace std;

unsigned long __stdcall autHealThread(void *args) 
{
    flyff *f = (flyff *)args;	

    for (;; Sleep(600)) 
    {
        if (f->getMeHp() < f->getHealingHp() && f->getMeHp()!=0)
        {

            printf("\nur hp is low, goin to heal\n");
            //cout <<" HP is low, Auto-Heal!"<<"\n";
            Sleep(20);
            PostMessage((HWND)f->getHwnd(), WM_KEYDOWN, 0x75, MapVirtualKey(0x75, MAPVK_VK_TO_VSC));
            Sleep(20);
            PostMessage((HWND)f->getHwnd(), WM_KEYUP, 0x75, MapVirtualKey(0x75, MAPVK_VK_TO_VSC));
        }

        else if (f->getMeHp()==0)
        {
            cout<<" HP is zero!\n"<<endl;
            Sleep(60000);
        }

    }
    return 0;
}



flyff::flyff(void *hwnd, void *handle, unsigned long base_addr) 
{
    _hwnd = hwnd;
    _handle = handle;
    _select_addr = base_addr +0x6C0438; 			
    _maxInView_addr = base_addr + 0x844F80;			
    _targetBaseaddr = base_addr +0x840160;	
    _me_addr = base_addr + 0x6BC7D8;				

    // null some vars to defult
    _threadHandle_autoHealThread = nullptr;
}
void *flyff::getHwnd()
{
    return _hwnd;
}
void flyff::setAutoHealThreadState(bool active)
{
    if (active)
    {
        if (_threadHandle_autoHealThread == nullptr) 
        {
            _threadHandle_autoHealThread = CreateThread(0, 0, &autHealThread, this, 0, 0);
        }
    }
    else
    {
        if (_threadHandle_autoHealThread != nullptr)
        {
            TerminateThread(_threadHandle_autoHealThread, 0);
            _threadHandle_autoHealThread = nullptr;
        }
    }
}
void flyff::setHealingHp(unsigned long hp)
{ 
    _healingHp = hp;
}
unsigned long flyff::getHealingHp() {
    return _healingHp;
}
void flyff::select(unsigned long target)
{ 
    unsigned long pointed = 0;

    ReadProcessMemory(_handle, (LPCVOID)(_select_addr),&pointed, 4, 0);
    WriteProcessMemory(_handle, (LPVOID)(pointed + 0x20),&target, 4, 0);
}
unsigned long flyff::getSelect()
{
    unsigned long pointed = 0;
    ReadProcessMemory(_handle, (LPCVOID)(_select_addr),&pointed, 4, 0);
    ReadProcessMemory(_handle, (LPCVOID)(pointed + 0x20),&pointed, 4, 0);

    return pointed;
}
unsigned long flyff::getMe()
{ 
    unsigned long value = 0;
    ReadProcessMemory(_handle, (LPCVOID)(_me_addr),&value, 4, 0);
    return value; 
}
char *flyff::get_name(unsigned long target) 
{
    char *player_name = (char *)malloc(256);
ReadProcessMemory(_handle, (void *)(target + OFFSET_NAME), player_name, 256, 0);


return player_name;

}

void flyff::adminAut()
{ 
    unsigned long value = 90;
    WriteProcessMemory(_handle, (LPVOID)(getMe()+0x77C),&value, 4, 0);
    return; 
}
unsigned long flyff::getMeHp()
{ 
    unsigned long value = 0;
    ReadProcessMemory(_handle, (LPCVOID)(getMe() + 0x730), &value, 4, 0);
    return value;

}
float flyff::getHyp(targetInfo ti)
{
    float x,z;
    ReadProcessMemory(_handle, (LPCVOID)(getMe()+0x160),&x, 4, 0);
    ReadProcessMemory(_handle, (LPCVOID)(getMe()+0x168),&z, 4, 0);

    return math :: sqrt((x -ti.x )*(x -ti.x )+(z -ti.z )*(z -ti.z ));
}

flyff::targetInfo flyff::getClosestTargetInView(char *name)
{
    unsigned long maxInView =0;
    targetInfo closest_ti = targetInfo();

    ReadProcessMemory(_handle, (LPCVOID)(_maxInView_addr) ,&maxInView, 4, 0);
    for(unsigned long i = 0; i < maxInView; i++)
    {
        unsigned long target=0;	
        unsigned long type=0;
        unsigned long lvl=0;
        unsigned long hp=0;
        unsigned long pet_type=0;
        unsigned long pet_type2=0;		


        ReadProcessMemory(_handle, (LPCVOID)(i * 4	+ _targetBaseaddr),&target, 4, 0);
        ReadProcessMemory(_handle, (LPCVOID)(target+ 4),&type, 4, 0);
        ReadProcessMemory(_handle, (LPCVOID)(target + 0x70c),&lvl, 4, 0);	
        ReadProcessMemory(_handle, (LPCVOID)(target + 0x730),&hp, 4, 0);		
        ReadProcessMemory(_handle, (LPCVOID)(target + 0x744),&pet_type, 4, 0);
        ReadProcessMemory(_handle, (LPCVOID)(target + 0x748),&pet_type2, 4, 0);


        if (target != getMe() 		
            && type == 18 			
            && lvl <= 181
            && lvl >=1
            && hp != lvl
            && hp >= 120 && hp <= 2200000000
            && pet_type != 19 
            )				
        {
            // check if name isnt null or empty, cause that means we dont want to select by name
            if (name != nullptr && strcmp(name, "") != 0) {
                // and check if passed name contains target name ... \/
                if (CheckSubstring(std::string(get_name(target)), std::string(name))) {
                    // passed we found with that name
                } else {
                    // further code gets passed and takes next for loop
                    continue;
                }
            }

            targetInfo ti;
            ReadProcessMemory(_handle, (LPCVOID)(target + 0x160),&ti.x, 4, 0);
            ReadProcessMemory(_handle, (LPCVOID)(target + 0x164),&ti.y, 4, 0);
            ReadProcessMemory(_handle, (LPCVOID)(target + 0x168),&ti.z, 4, 0);
            ti.hyp = getHyp(ti);

            if(ti.hyp > closest_ti.hyp)
            {
                ti.base = target;
                ti.lvl = lvl;
                ti.hp = hp;
                ReadProcessMemory(_handle, (LPCVOID)(target + 0x13DC),&ti.name, 255, 0);

                closest_ti = ti;
            }
        }
    }	
    return closest_ti;
}

bool flyff::adminExistsOnView()
{
    unsigned long maxInView = 0;

    ReadProcessMemory(_handle, (LPCVOID)(_maxInView_addr) ,&maxInView, 4, 0);
    for(unsigned long i = 0; i < maxInView; i++)
    {
        unsigned long target = 0;	
        unsigned long admin = 0;
        unsigned long lvl = 0;
        unsigned long hp = 0;

        ReadProcessMemory(_handle, (LPCVOID)(i * 4	+ _targetBaseaddr),&target, 4, 0);
        ReadProcessMemory(_handle, (LPCVOID)(target + OFFSET_ADMIN),&admin, 4, 0);
        ReadProcessMemory(_handle, (LPCVOID)(target + 0x70c), &lvl, 4, 0);	
        ReadProcessMemory(_handle, (LPCVOID)(target + 0x730), &hp, 4, 0);

        if (//target != getMe() && 
            admin >= 80 && 
            lvl <= 181 &&
            lvl >=1 && 
            hp != lvl && 
            hp >= 120 && 
            hp <= 2200000000)				
        {
            return true;
        }
    }	
    return false;
}

void flyff::teleportToTarget(unsigned long target) {
    unsigned char target_xyz[3 * 4];
    //unsigned char me_xyz[3 * 4];

    ReadProcessMemory(_handle, (void *)(target + 0x160), &target_xyz, 3 * 4, 0);

    // to force real teleport
    //*(float *)(target_xyz + 4) += 3.f;

    WriteProcessMemory(_handle, (void *)(getMe() + 0x160), &target_xyz, 3 * 4, 0);
}