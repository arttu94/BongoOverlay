#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include "BongoApp.h"

#include <SoLoud/soloud.h>
#include <SoLoud/soloud_wav.h>

#include <Gl/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

HHOOK k_Hook;

std::unique_ptr<BongoApp> app;

SoLoud::Soloud gSoLoud;
SoLoud::Wav gWave;
SoLoud::Wav wNotes[7];

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

			switch (BongoApp::tapObject)
			{
			case TapObject::none:
				break;
			case TapObject::keyboard:
				break;
			case TapObject::ducks:
				//only play the sound of the paw goes from a u-d action and not a d-d
				if (app->r_arm == app->r_arm_u && !BongoApp::isMuted)
					gSoLoud.play(gWave);
				break;
			case TapObject::synth:
				if (app->r_arm == app->r_arm_u && !BongoApp::isMuted && rnd == 0)
				{
					gSoLoud.play(wNotes[rand() % 6]);
				}
				if (app->l_arm == app->l_arm_u && !BongoApp::isMuted && rnd != 0)
				{
					gSoLoud.play(wNotes[rand() % 6]);
				}
				break;
			default:
				break;
			}

			if (rnd == 0)
				app->r_arm = app->r_arm_d;
			else
				app->l_arm = app->l_arm_d;

			break;
		case WM_KEYUP:
			if (0 == (rand() % 2))
				app->r_arm = app->r_arm_u;
			else
				app->l_arm = app->l_arm_u;
			break;
		default:
			break;
		}//switch
	}
	return CallNextHookEx(k_Hook, code, wParam, lParam);
}

int main(int argc, char** argv) 
{
	std::cout << "\nWith the program in the foreground use the L key to lock/unlock the window" << std::endl;
	std::cout << "locking will hide title bar and shadows and make it topmost (local overlay)" << std::endl;
	std::cout << "Exit the program with ESC or using the X button in the title bar" << std::endl;

	std::cout << "\nUse keys 1 ... 4 to change the object that will be tapped" << std::endl;
	std::cout << "if the object has a sound you can mute/unmute it with the M key\n" << std::endl;

	//FreeConsole();

	app = std::make_unique<BongoApp>();

	gSoLoud.init();
	gWave.load("Resources/Sound/quack.wav");

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

	app->Init();
	app->LoadResources();
	app->SetInput();

	BongoApp::isMuted = false;

	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

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
	}

	UnhookWindowsHookEx(k_Hook);
	app->Close();
	gSoLoud.deinit();

	return 0;
}
