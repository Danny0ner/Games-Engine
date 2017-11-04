#pragma once
#include "p2List.h"
#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleImGui.h"
#include "imguiconsole.h"
#include "JConfig.h"
#include "Assimp\include\ai_assert.h"
#include"Assimp\include\anim.h"
#include "ModuleEditor.h"
#include "GeometryLoader.h"

struct Performance
{
	uint	Frame_Count = 0;
	uint	Framerate = 0;
	uint	Frames_Last_Second = 0;
	uint	Miliseconds_Per_Frame = 0;
};

class GeometryLoader;
class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleImGui* imgui;
	GeometryLoader* geometryloader;
	JConfig* config;
	ModuleEditor* editor;
private:

	Timer	ms_timer;
	Timer	startup_timer;
	Timer	last_sec_frame_time;
	float	dt;
	uint	prev_last_sec_frame_count = 0;
	uint	last_sec_frame_count = 0;
	Performance	performance;
public: 
	std::list<Module*> list_modules;
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	ExampleAppConsole2 Console;
	Performance* GetPerformanceStruct();
	bool Options();
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};
extern Application* App; 