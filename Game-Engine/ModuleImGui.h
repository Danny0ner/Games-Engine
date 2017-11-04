#pragma once

#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#define FPS_AND_MS_PLOT_DATA_LENGTH 100

class ModuleImGui : public Module
{
public:
	ModuleImGui(Application* app, bool start_enabled = true);
	~ModuleImGui();
	bool Init();

	bool Start();

	//PreUpdate
	update_status PreUpdate(float dt);

	//PostUpdate
	update_status PostUpdate(float dt);

	void PushFPSandMSPlot(uint fps, uint ms);
	void ChangeName(char*);
	void Hardware();
	void ShowAbout();
	bool CleanUp();
	void ShowModules();

	void ShowEditor(bool* p_open = NULL);
public:
	char*	New_Name;
	int		Pc_Ram;
	int		Pc_Cache;
	int		Pc_Cpu;
	int		First = 0;
	int		Last = 100;
	LCG		Random_Num_1;
	LCG		Random_Num_2;
	float	Random_Num_Int = 0;
	float	Random_Num_Float = 0;
	bool    Showconsole = false;
	bool	Modules = false;
	bool	About = false;
	bool    showEditor = false;
	//OpenGL//





	vec Temp = { 0,0,0 };
	vec Temp1 = { 1,0,0 };
	vec Temp2 = { 3,0,0 };
	bool SphereTest = false;
	std::vector<float> Fps_Plot_Data;
	std::vector<float> Ms_Plot_Data;

private:


};

