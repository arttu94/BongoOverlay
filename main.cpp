#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include "BongoApp.h"

#include <Gl/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

HHOOK k_Hook;

std::unique_ptr<BongoApp> app;

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	//this (https://www.experts-exchange.com/questions/24116361/Converting-member-function-to-HOOKPROC.html)

	if (code == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_KEYDOWN:
			if (0 == (rand() % 2))
			{
				//only play the sound of the paw goes from a u-d action and not a d-d
				if(app->r_arm == app->r_arm_u && !BongoApp::isMuted && BongoApp::tapObject == TapObject::ducks)
					PlaySound(TEXT("Resources/quack.wav"), NULL, SND_ASYNC);
				app->r_arm = app->r_arm_d;
			}
			else
			{
				if (app->l_arm == app->l_arm_u && !BongoApp::isMuted && BongoApp::tapObject == TapObject::ducks)
					PlaySound(TEXT("Resources/quack.wav"), NULL, SND_ASYNC);
				app->l_arm = app->l_arm_d;
			}
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

		//print the key value A B C D E... instead of the code 65, 70, 40...
		/*
		KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
		cout << char(p->vkCode) << endl;
		*/
	}
	return CallNextHookEx(k_Hook, code, wParam, lParam);
}

int main(int argc, char** argv) 
{
	std::cout << "\nWith the program in the foreground use the L key to lock/unlock the window" << std::endl;
	std::cout << "locking will hide title bar and shadows and make it topmost (local overlay)" << std::endl;
	std::cout << "Exit the program with ESC or using the X button in the title bar" << std::endl;

	std::cout << "\nUse keys 1 ... 3 to change the object that will be tapped" << std::endl;
	std::cout << "if the object has a sound you can mute/unmute it with the M key\n" << std::endl;

	//FreeConsole();

	app = std::make_unique<BongoApp>();

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

	return 0;
}
