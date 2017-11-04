#include "Application.h"
#include "mmgr\mmgr.h"
Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	imgui = new ModuleImGui(this);
	geometryloader = new GeometryLoader(this);
	editor = new ModuleEditor(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(geometryloader);

	// Renderer last!
	AddModule(renderer3D);
	AddModule(editor);
	AddModule(imgui);






}

Application::~Application()
{
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

	ms_timer.Start();
	last_sec_frame_time.Start();
	startup_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	performance.Frame_Count++;
	last_sec_frame_count++;
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}
	performance.Frames_Last_Second = prev_last_sec_frame_count;
	performance.Framerate = (float)performance.Frame_Count / ((float)startup_timer.Read() / 1000.0f);
	performance.Miliseconds_Per_Frame = ms_timer.Read();
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
		ret = (*item)->PreUpdate(dt);
		(*item)->PauseTimer();
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*item)->ResumeTimer();
		ret = (*item)->Update(dt);
		(*item)->PauseTimer();
		item++;

	}
	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		(*item)->ResumeTimer();
		ret = (*item)->PostUpdate(dt);
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
	return &performance;
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