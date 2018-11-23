#ifndef FLYFF_H
#define FLYFF_H

class flyff
{
private:
	void *_hwnd;
	void *_handle;
	unsigned long _base_addr;
	unsigned long _select_addr;
	unsigned long _maxInView_addr;
	unsigned long _targetBaseaddr;
	unsigned long _me_addr;		

	void *_threadHandle_autoHealThread;
	unsigned long _healingHp;

public:
	flyff(void *hwnd, void *handle, unsigned long base_addr);

	void *getHwnd();
	void setAutoHealThreadState(bool active);

	struct targetInfo
	{
		unsigned long base;		
		float x,y,z;			
		float hyp;				
		unsigned long lvl;
		unsigned long hp;
		char name[255];

	};

	void select(unsigned long target); 
	unsigned long getSelect();
	unsigned long getMe();
	unsigned long getMeSpeed();
	unsigned long getMeHp();
	void adminAut();
	float getHyp(targetInfo ti);
	void setHealingHp(unsigned long hp);
	unsigned long getHealingHp();

	/// <summary>
	/// get char name by its base id
	/// </summary>
	char *get_name(unsigned long target);

	/// <summary>
	/// if name is set then get closest by its name
	/// </summary>
	targetInfo getClosestTargetInView(char *name = nullptr);

    /// <summary>
	/// checks if admin players exists on view,
    /// local player isnt counted
	/// </summary>
    bool flyff::adminExistsOnView();

	void teleportToTarget(unsigned long target);

};
#endif