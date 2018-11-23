#include	<Windows.h>
#include	"summoner.h"
#include	"flyff.h"
#include	<cmath>
#include	<stdlib.h>
#include	<conio.h>
#include	<iostream>
#include	<stdio.h>


using namespace std;


float zoom = 30;
//BYTE range [7]= {0xC7, 0x45 ,0xF8 ,0x00 ,0x00 ,0x40 ,0x40};
//BYTE nop2[2] ={0x90,0x90};


int main()
{
	HWND hwnd = FindWindowA(0, "GALAXY FLYFF R4");
	if (!hwnd)
	{
		std::cout << "Can't find GameWindow! "<<endl;
		Sleep (2000);
		return -1;
	}
	else
	{
		unsigned long pid;
		GetWindowThreadProcessId(hwnd, &pid);
		HANDLE handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, pid);
		if (!handle)
		{
			std::cout <<"Can't open process!"<<endl;
			Sleep (2000);
			return -1;
		}
		else
		{
			unsigned long base_addr = 0;
			int askedHp = 0;
            char askedName[256];

			base_addr = get_module(pid, "rto.exe");

			WriteProcessMemory(handle, (LPVOID)(base_addr+0x6A191C),"\x00", 1, 0); // no collision rto.exe+6A191C
			WriteProcessMemory(handle, (LPVOID)(base_addr+0x6BDA98),&zoom, 4, 0); // Zoom

			printf("Do u want Auto-Heal? If not type [-1 or 0]\n");
            fgets(askedName, 256, stdin); // using askedName as buf right now
            askedHp = atoi(askedName);
            printf("askedHp: %d", askedHp);


            printf("\nEnter name u wish to target, if not leave it empty\n");
            fgets(askedName, 256, stdin);
            printf("askedName: %s", askedName);
            // fix new line at the end
            askedName[strlen(askedName) -1] = 0;

			//printf("Searching for target ...");	
			flyff f(hwnd, handle, base_addr);
			f.setHealingHp(askedHp);
			f.setAutoHealThreadState(true);
			f.adminAut();		

			if (f.getSelect()!=0) {
				f.select(0);	
			}

			for(;; Sleep(500))	
			{		
                if (f.getSelect() == 0 && !f.adminExistsOnView())
				{
					flyff::targetInfo ti = f.getClosestTargetInView(askedName); // do I need to change that?
                    // thats right
					// does this code work ?
					//Yes

					if (ti.base != 0)
					{	
						//system("COLOR A");				
						printf( "\tTarget found! : \n\n\tname: %s\n\tlvl: %d\n\thp: %d\n\thyp : %f\n\n",ti.name,ti.lvl,ti.hp,ti.hyp);

						f.select(ti.base);

						Sleep(20);
						PostMessage(hwnd, WM_KEYDOWN, 0x73, MapVirtualKey(0x73, MAPVK_VK_TO_VSC));
						Sleep(20);
						PostMessage(hwnd, WM_KEYUP, 0x73, MapVirtualKey(0x73, MAPVK_VK_TO_VSC));
						Sleep(20);
						PostMessage(hwnd, WM_KEYDOWN, 0x73, MapVirtualKey(0x73, MAPVK_VK_TO_VSC));
						Sleep(20);
						PostMessage(hwnd, WM_KEYUP, 0x73, MapVirtualKey(0x73, MAPVK_VK_TO_VSC));

						//f.teleportToTarget(ti.base);   // To teleport to Monster
					} //end of if (ti.base != 0)

					Sleep(500);
					//rotate cam for searching..
					PostMessage(hwnd, WM_KEYDOWN, VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC));
					Sleep(50);
					PostMessage(hwnd, WM_KEYUP, VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC));
					Sleep(50);
				}//end of if (f.getSelect()==0)
			}//end of for(;;Sleep(500))
		}
	}

	return 0;
}