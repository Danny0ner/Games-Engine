#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "Application.h"
#include "SDL/include/SDL.h"
#include "Glew\include\glew.h"
#include "ImGui\imgui.h"
#include "Imgui/imgui_impl_sdl_gl3.h"
#include "SDL\include\SDL_opengl.h"

class Application;

class ModuleWindow : public Module
{
public:

	int		Window_Width = 1280;
	int		Window_Height = 720;
	bool	Fullscreen = false;
	bool	Fullscreen_Desktop = false;
	bool	Resizable;
	bool	Borderless;
	float	Brightness;
	const char* EngineName;



	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);
	void SetFullscreen(bool fullscreen);
	void SetResizable(bool resizable);
	void SetBorderless(bool borderless);
	void SetFullScreenDesktop(bool fullscreendesktop);
	void SetBrightness(float brightness);
	void SetWindowWidhtHeight(int width, int height);

	bool IsFullscreen()const;
	bool IsResizable()const;
	bool IsBorderless()const;
	bool IsFullscreenDesktop() const ;
	float GetBrightness() const ;
	void ImGuiDrawer();
	float GetAspectRatio()const ;

	int GetWidth()const ;

	int GetHeight()const ;
	

	void SaveConfig(JSON_Object* root);
	void LoadConfig(JSON_Object* root);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

};

#endif // __ModuleWindow_H__