
#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"

#include "mmgr\mmgr.h"
#include "mmgr\nommgr.h"

#include "Glew\include\glew.h"
#include "ImGui\imgui.h"
#include "Imgui/imgui_impl_sdl_gl3.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "ModuleEditor.h"
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */



ModuleImGui::ModuleImGui(Application * app, bool start_enabled) : Module(app, start_enabled),
Fps_Plot_Data(FPS_AND_MS_PLOT_DATA_LENGTH), Ms_Plot_Data(FPS_AND_MS_PLOT_DATA_LENGTH)
{
	name = "ImGui";
}

ModuleImGui::~ModuleImGui()
{
}

bool ModuleImGui::Init()
{
	glewInit();
	ImGui_ImplSdlGL3_Init(App->window->window);
	ImGuiIO& io = ImGui::GetIO();
	io.WantTextInput = true;
	io.IniFilename = "imgui.ini";
	return true;
}

bool ModuleImGui::Start()
{
	return true;
}

update_status ModuleImGui::PreUpdate(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	return UPDATE_CONTINUE;
}

update_status ModuleImGui::PostUpdate(float dt)
{



	//		MAIN MENU		//

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/JaviOrtiz/Game-Engines/wiki", NULL, NULL, SW_SHOWNORMAL);
			}
			if (ImGui::MenuItem("Download Latest"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/JaviOrtiz/Game-Engines/releases", NULL, NULL, SW_SHOWNORMAL);

			}
			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/JaviOrtiz/Game-Engines/issues", NULL, NULL, SW_SHOWNORMAL);

			}
			if (ImGui::MenuItem("Console"))
			{
				Showconsole = !Showconsole;
			}
			if (ImGui::MenuItem("About"))
			{
				About = !About;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Modules"))
		{
			if (ImGui::MenuItem("Show Modules"))
				Modules = !Modules;
			if (ImGui::MenuItem("Show Editor Window"))
			{
				showEditor = !showEditor;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Exit"))
		{
			return UPDATE_STOP;
		}

	}
	ImGui::EndMainMenuBar();


	//			BEGIN MENU INFORMATION		//


	// Start About? //
	if (About)
	{
		ShowAbout();

	}
	if (showEditor)
	{
		ShowEditor();
	}
	//End About//

	if (Modules)
	{
		ShowModules();
	}
		if (Showconsole) App->Console.Draw("Super Console", (bool*)false);
		ImGui::Render();
		SDL_GL_SwapWindow(App->window->window);
		return UPDATE_CONTINUE;
	};


	bool ModuleImGui::CleanUp()
	{
		return true;
	}

	void ModuleImGui::PushFPSandMSPlot(uint fps, uint ms)
	{
		static uint count = 0;

		if (count >= FPS_AND_MS_PLOT_DATA_LENGTH)
			for (int i = 0; i < FPS_AND_MS_PLOT_DATA_LENGTH - 1; i++)
			{
				Fps_Plot_Data[i] = Fps_Plot_Data[i + 1];
				Ms_Plot_Data[i] = Ms_Plot_Data[i + 1];
			}
		else
			count++;

		Fps_Plot_Data[count - 1] = fps;
		Ms_Plot_Data[count - 1] = ms;
	}


	void ModuleImGui::ChangeName(char* new_name)
	{

		App->window->EngineName = new_name;


	}

	void ModuleImGui::Hardware()
	{
		uint title_size = 50;
		char title[50];
		Pc_Cache = SDL_GetCPUCacheLineSize();
		ImGui::Text("Cache: %i", Pc_Cache);

		Pc_Cpu = SDL_GetCPUCount();
		ImGui::Text("CPU: %i", Pc_Cpu);

		//Memory Statistics//
		sMStats MemoryStats = m_getMemoryStatistics();
		sprintf_s(title, title_size, "totalReportedMemory: %i", MemoryStats.totalReportedMemory);
		ImGui::Text(title);
		sprintf_s(title, title_size, "totalActualMemory: %i", MemoryStats.totalActualMemory);
		ImGui::Text(title);
		sprintf_s(title, title_size, "peakReportedMemory: %i", MemoryStats.peakReportedMemory);
		ImGui::Text(title);
		sprintf_s(title, title_size, "peakActualMemory: %i", MemoryStats.peakActualMemory);
		ImGui::Text(title);
		sprintf_s(title, title_size, "accumulatedReportedMemory: %i", MemoryStats.accumulatedReportedMemory);
		ImGui::Text(title);
		sprintf_s(title, title_size, "accumulatedActualMemory: %i", MemoryStats.accumulatedActualMemory);
		ImGui::Text(title);
		sprintf_s(title, title_size, "accumulatedAllocUnitCount: %i", MemoryStats.accumulatedAllocUnitCount);
		ImGui::Text(title);
		sprintf_s(title, title_size, "totalAllocUnitCount: %i", MemoryStats.totalAllocUnitCount);
		ImGui::Text(title);
		sprintf_s(title, title_size, "peakAllocUnitCount: %i", MemoryStats.peakAllocUnitCount);
		ImGui::Text(title);


		sprintf_s(title, title_size, "RAM: %i", SDL_GetSystemRAM());
		ImGui::Text(title);

		ImGui::Separator();

		const GLubyte* GPUData = nullptr;
		GPUData = glGetString(GL_VENDOR);
		sprintf_s(title, title_size, "GL_Vendor: %s", GPUData);
		ImGui::Text(title);
		GPUData = glGetString(GL_RENDERER);
		sprintf_s(title, title_size, "GL_Renderer: %s", GPUData);
		ImGui::Text(title);
		GPUData = glGetString(GL_VERSION);
		sprintf_s(title, title_size, "GL_Versions: %s", GPUData);
		ImGui::Text(title);





	}
	void ModuleImGui::ShowAbout()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("About", (bool*)false, window_flags);
		ImGui::Text("Pochinki's Engine 0.1");
		ImGui::Text("We are a pair of students who are working on a 3d Game Engine c++ based.");
		ImGui::Text("We are using ImGui library, DevIL,OpenGl,mmgr, Assimp, Glew and MathGeoLib.");		
		ImGui::Text("Authors:		Daniel Lopez and Javier Ortiz");
		ImGui::End();
	}

	void ModuleImGui::ShowModules() {

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("Modules", (bool*)false, window_flags);
		ImGui::Text("Here You can see and edit the Modules Preferences");
		

		//  Start Window Options//
		if (ImGui::CollapsingHeader("Window Options"))
		{
			App->window->ImGuiDrawer();
		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			Hardware();
		}
		const Performance* PerformanceData = App->GetPerformanceStruct();
		PushFPSandMSPlot(PerformanceData->Frames_Last_Second, PerformanceData->Miliseconds_Per_Frame);

		// End Window Options//

		// Start Application//
		if (ImGui::CollapsingHeader("Application"))
		{
			uint title_size = 50;
			char title[50];
			//Framerate PlotHistogram
			sprintf_s(title, title_size, "Framerate: %i", PerformanceData->Frames_Last_Second);
			ImGui::PlotHistogram("##Framerate", &Fps_Plot_Data[0], Fps_Plot_Data.size(), 0, title, 0.0f, 150.0f, ImVec2(310, 100));

			//Miliseconds PlotHistogram
			sprintf_s(title, title_size, "Frame Miliseconds: %i", PerformanceData->Miliseconds_Per_Frame);
			ImGui::PlotHistogram("##Frame Miliseconds", &Ms_Plot_Data[0], Ms_Plot_Data.size(), 0, title, 0.0f, 50.0f, ImVec2(310, 100));
		}


		if (ImGui::CollapsingHeader("Renderer"))
		{
			App->renderer3D->ImGuiDrawer();
		}

		App->Options();

		ImGui::End();
	}

	void ModuleImGui::ShowEditor(bool* p_open)
	{
		ImGuiWindowFlags window_flags = 0;

		if (!ImGui::Begin("Show Editor", p_open, window_flags))
		{
			// Early out if the window is collapsed, as an optimization.
			ImGui::End();
			return;
		}

		//ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
		ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

		if (ImGui::CollapsingHeader("GameObjects Hieriarchy"))
		{
			App->editor->ShowEditor();
		}

		ImGui::End();
	}

