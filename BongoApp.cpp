#include "BongoApp.h"
#include <iostream>
#include <Windows.h>
#include <vector>

#include "SpriteRenderer.h"
#include "Texture.h"
#include "Shader.h"

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <stdio.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

bool BongoApp::isMuted = false;	
TapObject BongoApp::tapObject = TapObject::none;

BongoApp::BongoApp() : 
	window(nullptr), shader(nullptr), 
	bodyTex(nullptr), tapObjTex(nullptr),
	r_arm_u(nullptr), r_arm_d(nullptr), r_arm(nullptr), 
	l_arm_u(nullptr), l_arm_d(nullptr), l_arm(nullptr)
{

}

BongoApp::~BongoApp()
{
	std::cout << "destroying BongoApp" << std::endl;

	this->r_arm = nullptr;
	this->l_arm = nullptr;

	if (r_arm_u)
		delete r_arm_u;
	if (r_arm_d)
		delete r_arm_d;

	if (l_arm_u)
		delete l_arm_u;
	if (l_arm_d)
		delete l_arm_d;
}

void BongoApp::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(window, GLFW_DECORATED) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
			glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_TRUE);
		}
		else
		{
			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
			glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_FALSE);
		}
	}
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		// mute
		BongoApp::isMuted = !BongoApp::isMuted;
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		//none
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		//keyboard
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		//ducks
	}
}

void BongoApp::SetInput()
{
	glfwSetKeyCallback(this->window, BongoApp::KeyCallback);
}

bool BongoApp::Init()
{
	if (!glfwInit())
	{
		fprintf(stderr, "ERROR:couldn't start GLFW3\n");
		return false;
	}

	//get active screen dimentions
	int desktopWidth = GetSystemMetrics(SM_CXSCREEN);
	int desktopHeight = GetSystemMetrics(SM_CYSCREEN);

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); //transparent background, but transparent areas are still "clickable"
	glfwWindowHint(GLFW_FLOATING, GLFW_FALSE); //topmost, always visible
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	//create window
	this->window = glfwCreateWindow(300.f, 300.f, "Bongo App-OpenGL4", nullptr, nullptr);

	//something went wrong 
	if (!window)
	{
		fprintf(stderr, "ERROR:couldn't open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(this->window);

	//start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	//get version info
	const GLubyte* renderer = glGetString(GL_RENDER);
	const GLubyte* version = glGetString(GL_VERSION);

	printf("Renderer: %s\n", renderer);
	printf("OpenGl version supported: %s\n", version);

	//tell GL to only draw onto a pixel if it's closer to the viewer
	glEnable(GL_DEPTH_TEST); //enable depth testing
	glDepthFunc(GL_LESS); //depth test interprets a smaller value as closer

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//MakeGLWindowTransparent(RGB(0.0,1.0,0.0)); //makes black background transparent, makes the transparent area ignore input.

	if (glfwGetWindowAttrib(this->window, GLFW_TRANSPARENT_FRAMEBUFFER))
	{
		//this changes the whole widnow transparency including the content not just the decorations.
		glfwSetWindowOpacity(window, 1.0f);
	}

	return true;
}

void BongoApp::LoadResources()
{
	this->shader = std::make_shared<Shader>("Resources/Shader/sprite.vert", "Resources/Shader/sprite.frag");

	this->bodyTex = std::make_unique<Texture>(*Texture::loadTextureFromFile("Resources/body.png", GL_TRUE));

	this->r_arm_d = Texture::loadTextureFromFile("Resources/r_arm_d.png", GL_TRUE);
	this->r_arm_u = Texture::loadTextureFromFile("Resources/r_arm_u.png", GL_TRUE);
	this->r_arm = r_arm_u;

	this->l_arm_d = Texture::loadTextureFromFile("Resources/l_arm_d.png", GL_TRUE);
	this->l_arm_u = Texture::loadTextureFromFile("Resources/l_arm_u.png", GL_TRUE);
	this->l_arm = l_arm_u;

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(300),
		static_cast<GLfloat>(300), 0.0f, -10.0f, 10.0f);

	this->shader->Use()->SetInt("image", 0);
	this->shader->Use()->SetMatrix4("projection", projection);

	this->spriteRenderer = std::make_unique<SpriteRenderer>(this->shader);
}

bool BongoApp::MakeGLWindowTransparent(COLORREF colorKey)
{
	HWND hWnd = glfwGetWin32Window(this->window);

	// Change window type to layered (https://stackoverflow.com/a/3970218/3357935)
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	// Set transparency color
	return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

void BongoApp::Render()
{
	//if there are any opaque sprites draw them above this comment
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->spriteRenderer->DrawSprite(*this->bodyTex, glm::vec2(0, 0), glm::vec2(300, 300)); 
	if(BongoApp::tapObject != TapObject::none)
		this->spriteRenderer->DrawSprite(*this->tapObjTex, glm::vec2(0, 0), glm::vec2(300, 300));
	this->spriteRenderer->DrawSprite(*this->l_arm, glm::vec2(0, 0), glm::vec2(300, 300));
	this->spriteRenderer->DrawSprite(*this->r_arm, glm::vec2(0, 0), glm::vec2(300, 300));
	//draw transparent objects above this comment
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void BongoApp::Update(float dt)
{
	int state = glfwGetKey(window, GLFW_KEY_1);
	if (state == GLFW_PRESS)
	{
		BongoApp::tapObject = TapObject::none;
	}
	state = glfwGetKey(window, GLFW_KEY_2);
	if (state == GLFW_PRESS)
	{
		if (BongoApp::tapObject != TapObject::ducks)
		{
			this->tapObjTex.reset();
			this->tapObjTex = std::make_unique<Texture>(*Texture::loadTextureFromFile("Resources/ducks.png", GL_TRUE));
			BongoApp::tapObject = TapObject::ducks;
		}
	}
	state = glfwGetKey(window, GLFW_KEY_3);
	if (state == GLFW_PRESS)
	{
		if (BongoApp::tapObject != TapObject::keyboard)
		{
			this->tapObjTex.reset();
			this->tapObjTex = std::make_unique<Texture>(*Texture::loadTextureFromFile("Resources/kb.png", GL_TRUE));
			BongoApp::tapObject = TapObject::keyboard;
		}
	}
}

void BongoApp::Close()
{
	glfwTerminate();
}