#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include "BongoApp.h"
#include "SpriteRenderer.h"

#include <SoLoud/soloud.h>
#include <SoLoud/soloud_wav.h>

#include <glm/glm.hpp>

#include <Gl/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "main.h"

HHOOK k_Hook;
HHOOK m_Hook;

std::unique_ptr<BongoApp> app;

SoLoud::Soloud gSoLoud;
SoLoud::Wav wQuack;
SoLoud::Wav wAK;
SoLoud::Wav wNotes[7];

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(code < 0)
		return CallNextHookEx(m_Hook, code, wParam, lParam);

	MSLLHOOKSTRUCT* pMouseStruct;

	switch (wParam)
	{
	case WM_MOUSEMOVE:
		pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

		app->SetMousePos(pMouseStruct->pt.x, pMouseStruct->pt.y);
		break;
	default:
		break;
	}

	return CallNextHookEx(m_Hook, code, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	//this (https://www.experts-exchange.com/questions/24116361/Converting-member-function-to-HOOKPROC.html) // no longer used but keeping the reference

	if (code == HC_ACTION)
	{
		//print the key value A B C D E... instead of the code 65, 70, 40...
		/*
		KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
		cout << char(p->vkCode) << endl;
		*/

		int rnd = rand() % 2;

		switch (wParam)
		{
		case WM_KEYDOWN:

			if (!BongoApp::isMuted)
			{
				switch (BongoApp::tapObject)
				{
				case TapObject::none:
					break;
				case TapObject::keyboard:
					break;
				case TapObject::ducks:
					//only play the sound of the paw goes from a u-d action and not a d-d
					if (app->r_arm == app->r_arm_u && rnd == 0)
						gSoLoud.play(wQuack);
					if (app->l_arm == app->l_arm_u && rnd != 0)
						gSoLoud.play(wQuack);
					break;
				case TapObject::synth:
					if (app->r_arm == app->r_arm_u && rnd == 0)
						gSoLoud.play(wNotes[rand() % 6]);
					if (app->l_arm == app->l_arm_u && rnd != 0)
						gSoLoud.play(wNotes[rand() % 6]);
					break;
				case TapObject::ak:
					if (app->r_arm == app->r_arm_u && rnd == 0)
						gSoLoud.play(wAK);
					if (app->l_arm == app->l_arm_u && rnd != 0)
						gSoLoud.play(wAK);
					break;
				default:
					break;
				}
			}

			if (app->tapObject == TapObject::kbNms)
			{
				app->l_arm = app->l_arm_d;
				app->lastUsedLimb = Limb::left;
				break;
			}

			if (rnd == 0)
			{
				app->r_arm = app->r_arm_d;
				app->lastUsedLimb = Limb::right;
			}
			else
			{
				app->l_arm = app->l_arm_d;
				app->lastUsedLimb = Limb::left;
			}


			break;
		case WM_KEYUP:
			if (app->lastUsedLimb == Limb::right)
			{
				app->r_arm = app->r_arm_u;
			}
			else if (app->lastUsedLimb == Limb::left || app->tapObject == TapObject::kbNms)
			{
				app->l_arm = app->l_arm_u;
			}
	
			break;
		default:
			break;
		}//switch
	}
	return CallNextHookEx(k_Hook, code, wParam, lParam);
}

bool HookMouse()
{
	if (m_Hook)
		return false;
	m_Hook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(0), 0);
	if (!m_Hook)
	{
		SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
		std::cout << "ERROR::mouse not hooked" << std::endl;
		return false;
	}
	std::cout << "mouse hooked" << std::endl;
	return true;
}

void UnhookMouse()
{
	if (!m_Hook)
		return;
	std::cout << "mouse unhooked" << std::endl;
	UnhookWindowsHookEx(m_Hook);
	m_Hook = NULL;
}

int main(int argc, char** argv) 
{
	std::cout << "\nWith the program in the foreground use the L key to lock/unlock the window" << std::endl;
	std::cout << "locking will hide title bar and shadows and make it topmost (local overlay)" << std::endl;
	std::cout << "Exit the program with ESC or using the X button in the title bar" << std::endl;

	std::cout << "\nUse keys 1 ... 6 to change the object that will be tapped" << std::endl;
	std::cout << "if the object has a sound you can mute/unmute it with the M key\n" << std::endl;

	//FreeConsole();

	app = std::make_unique<BongoApp>();

	gSoLoud.init();
	wQuack.load("Resources/Sound/quack.wav");
	wAK.load("Resources/Sound/akshot.wav");

	for (int i = 65; i < 71; i++)
	{
		char note = i;
		std::string path = "Resources/Sound/";
		std::string format = ".wav";
		std::string src = path + note + format;
		wNotes[i - 65].load(src.c_str());
	}

	srand(time(NULL));

	k_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(0), 0);
	if (!k_Hook)
	{
		SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
		return 1;
	}

	app->HookCallback = &HookMouse;
	app->UnhookCallback = &UnhookMouse;

	/*if (!HookMouse())
		return 1;*/

	app->Init();
	app->LoadResources();
	app->SetInput();

	BongoApp::isMuted = false;

	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	double savedTime = 0;
	double updateTimer = 0;
	const double updateInterval = 1.0 / 30.0;

	while (!glfwWindowShouldClose(app->GetGLFWWindow()))
	{
		//glfwPollEvents();
		glfwWaitEvents();

		// Calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Update Game state
		app->Update(deltaTime);

		double currentTime = glfwGetTime();
		double elapseTime = currentTime - savedTime;
		savedTime = currentTime;

		updateTimer += elapseTime;

		if (updateTimer >= updateInterval) 
		{
			// Render stuff

			// if it's topmost clear for a transparent background (has to be black with 0.0f alpha value) 
			// else use a green background (eg OBS chroma key) 
			if (glfwGetWindowAttrib(app->GetGLFWWindow(), GLFW_FLOATING))
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			else
				glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			app->Render();

			glfwSwapBuffers(app->GetGLFWWindow());
			
			updateTimer = 0;
		}

	}

	UnhookWindowsHookEx(k_Hook);
	UnhookMouse();
	app->Close();
	gSoLoud.deinit();

	return 0;
}
