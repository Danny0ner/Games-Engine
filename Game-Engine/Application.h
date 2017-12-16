#pragma once
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
#include "GeometryLoader.h"
#include "FileSystem.h"
#include "ResourcesManager.h"
#include "ModuleEditor.h"

#include "MathGeo\Algorithm\Random\LCG.h"


struct Performance
{
	uint	Frame_Count = 0;
	uint	Framerate = 0;
	uint	Frames_Last_Second = 0;
	uint	Miliseconds_Per_Frame = 0;
};

struct GameTime
{
	Timer	GameStart;
	float	TimeScale = 1.0f;
	bool	NextFrame = false;



};

struct RealTime
{
	Timer	ms_timer;
	Timer	startup_timer;
	Timer	last_sec_frame_time;
	float	dt;
	uint	prev_last_sec_frame_count = 0;
	uint	last_sec_frame_count = 0;
	Performance	performance;
};

enum GameStatus
{
	PLAY=0,
	PAUSE,
	STOP,
	NEXTFRAME
};

class GeometryLoader;
class FileSystem;
class ResourcesManager;


class Application
{
public:
	ModuleWindow*		window;
	ModuleInput*		input;
	ModuleAudio*		audio;
	ModuleRenderer3D*	renderer3D;
	ModuleCamera3D*		camera;
	ModuleImGui*		imgui;
	GeometryLoader*		geometryloader;
	JConfig*			config;
	ModuleEditor*		editor;
	FileSystem*			filesystem;
	LCG*			RandomUIDGen = nullptr;
	ResourcesManager*   resources;
	RealTime			realtime;
	GameTime			gametime;
	GameStatus			gamestatus = GameStatus::STOP;
	GameStatus			lastgamestatus = GameStatus::STOP;


	
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
	void Play();
	void Pause();
	void Stop();
	void NextFrame();
	void StatusSwitch();
	void RestartGame();
	void StartGame();
	int GetEditorDt();
	int GetGameStart();
	GameStatus GetGameStatus();
	float GetDeltaTime();
	int GetFramesSinceStart();
	int GetlastFrames();

	std::string GetFilenameOfString(std::string file);
private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};
extern Application* App; 