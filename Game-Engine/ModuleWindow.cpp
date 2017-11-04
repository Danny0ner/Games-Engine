#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "mmgr\mmgr.h"
ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Window";
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	App->Console.AddLog("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		App->Console.AddLog("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(EngineName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Window_Width, Window_Height, SDL_WINDOW_OPENGL);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			App->Console.AddLog("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	Brightness = App->window->GetBrightness();
	Fullscreen = IsFullscreen();
	Resizable = IsResizable();
	Borderless =IsBorderless();
	Fullscreen_Desktop =IsFullscreenDesktop();
	App->renderer3D->OnResize(Window_Width, Window_Height);
	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");
	App->Console.AddLog("Destroying SDL window and quitting all SDL systems");
	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}


void ModuleWindow::SetFullscreen(bool set)
{
	
	Fullscreen = set;
	if (Fullscreen == true)
	{
		if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
		{
				LOG("Could not switch to fullscreen: %s\n", SDL_GetError());
				App->Console.AddLog("Could not switch to fullscreen: %s\n", SDL_GetError());
		}
			Fullscreen_Desktop = false;
	}
	else
	{
		if (SDL_SetWindowFullscreen(window, 0) != 0)
		{
				LOG("Could not switch to windowed: %s\n", SDL_GetError());
				App->Console.AddLog("Could not switch to windowed: %s\n", SDL_GetError());
		}
	}

}

void ModuleWindow::SetResizable(bool set)
{
	// cannot be changed while the program is running, but we can save the change
	Resizable = set;
}

void ModuleWindow::SetBorderless(bool set)
{
	if (Fullscreen == false && Fullscreen_Desktop == false)
	{
		Borderless = set;
		SDL_SetWindowBordered(window, (SDL_bool)!Borderless);
	}
}

void ModuleWindow::SetFullScreenDesktop(bool set)
{

	Fullscreen_Desktop = set;
	if (Fullscreen_Desktop == true)
	{
		if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
		{
				LOG("Could not switch to fullscreen desktop: %s\n", SDL_GetError());
				App->Console.AddLog("Could not switch to fullscreen desktop: %s\n", SDL_GetError());
		}
		Fullscreen = false;
	}
	else
	{
		if (SDL_SetWindowFullscreen(window, 0) != 0)
		{
				LOG("Could not switch to windowed: %s\n", SDL_GetError());
				App->Console.AddLog("Could not switch to windowed: %s\n", SDL_GetError());
		}
	}
}

void ModuleWindow::SetBrightness(float set)
{
	CAP(set);
	if (SDL_SetWindowBrightness(window, set) != 0)
	{
		LOG("Could not change window brightness: %s\n", SDL_GetError());
		App->Console.AddLog("Could not change window brightness: %s\n", SDL_GetError());
	}
}

bool ModuleWindow::IsFullscreen()
{
	return Fullscreen;
}

bool ModuleWindow::IsResizable() 
{

	return Resizable;
}

bool ModuleWindow::IsBorderless() 
{
	return Borderless;
}

bool ModuleWindow::IsFullscreenDesktop() 
{
	return Fullscreen_Desktop;
}

float ModuleWindow::GetBrightness()
{
	return SDL_GetWindowBrightness(window);
}


void ModuleWindow::SaveConfig(JSON_Object * root)
{
	json_object_set_number(root, "Width", Window_Width / SCREEN_SIZE);
	json_object_set_number(root, "Height", Window_Height / SCREEN_SIZE);
	json_object_set_string(root, "EngineName", EngineName);
}

void ModuleWindow::LoadConfig(JSON_Object * root)
{

	if (json_object_get_value(root, "Width") == NULL) {
		json_object_set_value(root, "Width", json_value_init_object());
		Window_Width = SCREEN_WIDTH * SCREEN_SIZE;
		json_object_set_number(root, "Width", Window_Width);

	}
	else 
	{
		Window_Width = json_object_get_number(root, "Width")  * SCREEN_SIZE;

	}

	if (json_object_get_value(root, "Height") == NULL) {
		json_object_set_value(root, "Height", json_value_init_object());
		Window_Height = SCREEN_HEIGHT * SCREEN_SIZE;
		json_object_set_number(root, "Height", Window_Height);
	}
	else 
	{
		Window_Height = json_object_get_number(root, "Height") * SCREEN_SIZE;

	}
	if (json_object_get_string(root, "EngineName") == NULL) {
		json_object_set_string(root, "EngineName", "Pochinki's Engine");
		EngineName = "Pockinki's Engine";
	}
	else 
	{
		EngineName = json_object_get_string(root, "EngineName");
	}
}


void ModuleWindow::ImGuiDrawer() 
{
	if (ImGui::SliderFloat("Brightness", &Brightness, 0.0f, 1.0f))
	{
		SetBrightness(Brightness);
	}

	ImGui::SliderInt("Screen Width", &Window_Width, 100, 1920);
	ImGui::SliderInt("Screen Height", &Window_Height, 100, 1080);

	if (ImGui::Button("Save Window Size"))
	{
		SDL_SetWindowSize(window, Window_Width, Window_Height);
		App->renderer3D->OnResize(Window_Width, Window_Height);
	}


	ImGui::SameLine();

	if (ImGui::Checkbox("Resizable", &Resizable))
	{
		SetResizable(Resizable);
	}
	if (ImGui::Checkbox("borderless", &Borderless)) SetBorderless(Borderless);

	if (ImGui::Checkbox("fullscreen", &Fullscreen)) SetFullscreen(Fullscreen);


}
