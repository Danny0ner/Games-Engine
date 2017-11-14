#include "Application.h"
#include "mmgr\mmgr.h"
Application::Application()
{
	RandomUIDGen = new math::LCG();

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	imgui = new ModuleImGui(this);
	geometryloader = new GeometryLoader(this);
	editor = new ModuleEditor(this);
	filesystem = new FileSystem(this);
	resources = new ResourcesManager(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(geometryloader);
	AddModule(resources);
	// Renderer last!
	AddModule(renderer3D);
	AddModule(editor);
	AddModule(filesystem);
	AddModule(imgui);
}

Application::~Application()
{
	RELEASE(RandomUIDGen);
	RELEASE(config);
	std::list<Module*>::iterator item = list_modules.end();
	item--;
	while (item != list_modules.begin())	{
		RELEASE(item._Ptr->_Myval->module_timer);
		RELEASE(item._Ptr->_Myval);
		item--;
		}
	if (item == list_modules.begin())
	{
		RELEASE(item._Ptr->_Myval->module_timer);
		RELEASE(item._Ptr->_Myval);
	}
	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	config = new JConfig();
	config->Init();
	config->Load();
	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == true)
	{

		ret = (*item)->Init();
		(*item)->module_timer = new Timer();
		item++;
	}


	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.begin();

	while (item != list_modules.end() && ret == true)
	{

		ret = (*item)->Start();
		item++;
	}

	realtime.ms_timer.Start();
	realtime.last_sec_frame_time.Start();
	realtime.startup_timer.Start();
	gametime.GameStart.Start();
	gametime.GameStart.Stop();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	realtime.performance.Frame_Count++;
	realtime.last_sec_frame_count++;
	realtime.dt = (float)realtime.ms_timer.Read() / 1000.0f;
	realtime.ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (realtime.last_sec_frame_time.Read() > 1000)
	{
		realtime.last_sec_frame_time.Start();
		realtime.prev_last_sec_frame_count = realtime.last_sec_frame_count;
		realtime.last_sec_frame_count = 0;
	}
	realtime.performance.Frames_Last_Second = realtime.prev_last_sec_frame_count;
	realtime.performance.Framerate = (float)realtime.performance.Frame_Count / ((float)realtime.startup_timer.Read() / 1000.0f);
	realtime.performance.Miliseconds_Per_Frame = realtime.ms_timer.Read();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = list_modules.begin();
	
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*item)->StartTimer();
		ret = (*item)->PreUpdate(realtime.dt);
		(*item)->PauseTimer();
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*item)->ResumeTimer();
		ret = (*item)->Update(realtime.dt);
		(*item)->PauseTimer();
		item++;

	}
	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*item)->ResumeTimer();
		ret = (*item)->PostUpdate(realtime.dt);
		(*item)->StopTimer();

		item++;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == true)
	{

		ret = (*item)->CleanUp();
		item++;

	}
	config->Save();
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

Performance* Application::GetPerformanceStruct()
{
	return &realtime.performance;
}

bool Application::Options()
{
	if (ImGui::CollapsingHeader("ModulesPerformance"))
	{
		std::list<Module*>::iterator item = list_modules.begin();

		while (item != list_modules.end())
		{

			const char* name = (*item)->name.c_str();

			(*item)->performance[(*item)->performance_offset] = (*item)->module_timer->Read_ms();
			(*item)->performance_offset = ((*item)->performance_offset + 1) % IM_ARRAYSIZE((*item)->performance);

			ImGui::PlotHistogram((char*)name, (*item)->performance, IM_ARRAYSIZE((*item)->performance), 0, name, 0.0f, 30.f, ImVec2(0, 40));
			item++;
		}
	}
	return true;
}

void Application::Play()
{
	lastgamestatus = gamestatus;
	gamestatus = PLAY;
	StatusSwitch();
}
void Application::Pause()
{
	lastgamestatus = gamestatus;
	gamestatus = PAUSE;
	StatusSwitch();
}
void Application::Stop()
{
	lastgamestatus = gamestatus;
	gamestatus = STOP;
	StatusSwitch();
}
void Application::NextFrame()
{
	lastgamestatus = gamestatus;
	gamestatus = NEXTFRAME;
	StatusSwitch();
}
void Application::StatusSwitch()
{
	if (gamestatus == PLAY)
	{
		if (lastgamestatus == PAUSE){
			gametime.GameStart.Resume();
		}
		else {
			gametime.TimeScale = 1.0f;
			gametime.GameStart.Start();
			StartGame();
		}

	}
	if (gamestatus == PAUSE)
	{
		
		gametime.TimeScale = 0.0f;
		gametime.GameStart.Pause();
	}
	if (gamestatus == STOP)
	{
		
		gametime.TimeScale = 0.0f;
		RestartGame();
		gametime.GameStart.Stop();
		gametime.GameStart.actual_ms = 0;

	}
	if (gamestatus == NEXTFRAME)
	{
		
		gametime.TimeScale = 0.0f;

	}


}

void Application::RestartGame()
{
	App->editor->LoadScene("Library/Scenes/Scene test.Pochinki");


}
void Application::StartGame()
{
	App->editor->SerializeScene("Scene test");

}

int Application::GetEditorDt()
{
	return realtime.dt;
}
int Application::GetGameStart()
{
	return gametime.GameStart.Read();
}
float Application::GetDeltaTime()
{
	return realtime.dt;
}
int Application::GetFramesSinceStart()
{
	return realtime.performance.Frame_Count;
}
int Application::GetlastFrames()
{
	return realtime.performance.Framerate;
}

