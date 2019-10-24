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
TapObject BongoApp::tapObject = (TapObject)0;

BongoApp::BongoApp() : 
	window(nullptr), shader(nullptr), lastUsedLimb((Limb)0), mousePos(glm::vec2(0.f)),
	bodyTex(nullptr), tapObjTex(nullptr),
	r_arm_u(nullptr), r_arm_d(nullptr), r_arm(nullptr), r_arm_mouse(nullptr),
	l_arm_u(nullptr), l_arm_d(nullptr), l_arm(nullptr)
{

}

BongoApp::~BongoApp()
{
	std::cout << "destroying BongoApp" << std::endl;

	this->r_arm = nullptr;
	this->l_arm = nullptr;

	if (r_arm_mouse)
		delete r_arm_mouse;

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

void BongoApp::SetMousePos(float x, float y)
{
	this->mousePos.x = x;
	this->mousePos.y = y;
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

	this->mouseTex = std::make_unique<Texture>(*Texture::loadTextureFromFile("Resources/mouse.png", GL_TRUE));

	this->r_arm_d = Texture::loadTextureFromFile("Resources/r_arm_d.png", GL_TRUE);
	this->r_arm_u = Texture::loadTextureFromFile("Resources/r_arm_u.png", GL_TRUE);
	this->r_arm = r_arm_u;

	this->l_arm_d = Texture::loadTextureFromFile("Resources/l_arm_d.png", GL_TRUE);
	this->l_arm_u = Texture::loadTextureFromFile("Resources/l_arm_u.png", GL_TRUE);
	this->l_arm = l_arm_u;

	this->r_arm_mouse = Texture::loadTextureFromFile("Resources/r_arm_mouse.png", GL_TRUE);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(300),
		static_cast<GLfloat>(300), 0.0f, -10.0f, 10.0f);

	this->shader->Use()->SetInt("image", 0);
	this->shader->Use()->SetMatrix4("projection", projection);

	this->spriteRenderer = std::make_unique<SpriteRenderer>(this->shader);
	this->dynamicSpriteRenderer = std::make_unique<SpriteRenderer>(this->shader);
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
	// TODO: find the sweet spot with the offset
	// TODO: do the arm thing in a less expensive way
	if (this->tapObject == TapObject::kbNms)
	{
		GLfloat vertices[] = {
			// Pos      // Tex
			0.0f + (this->mousePos.x * .00015), 1.1f + (this->mousePos.y * .00015), 0.0f, 1.0f,
			1.0f                              , 0.0f                              , 1.0f, 0.0f,
			0.0f                              , 0.0f                              , 0.0f, 0.0f,
							              			                
			0.0f + (this->mousePos.x * .00012), 1.0f + (this->mousePos.y * .00012), 0.0f, 1.0f,
			1.0f + (this->mousePos.x * .00012), 1.0f + (this->mousePos.y * .00012), 1.0f, 1.0f,
			1.0f                              , 0.0f                              , 1.0f, 0.0f
		};

		if (this->dynamicSpriteRenderer != nullptr)
		{
			glBindBuffer(GL_ARRAY_BUFFER, this->dynamicSpriteRenderer->VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		}

		// (https://stackoverflow.com/questions/34125298/modify-single-vertex-position-in-opengl) for reference
	}


	//if there are any opaque sprites draw them above this comment
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->spriteRenderer->DrawSprite(*this->bodyTex, glm::vec2(0, 0), glm::vec2(300, 300)); 

	if(BongoApp::tapObject != TapObject::none)
		this->spriteRenderer->DrawSprite(*this->tapObjTex, glm::vec2(0, 0), glm::vec2(300, 300));

	if(this->tapObject == TapObject::kbNms)
		this->spriteRenderer->DrawSprite(*this->mouseTex, glm::vec2(0 + (this->mousePos.x * 0.014f), 170 + (this->mousePos.y * 0.014f)), glm::vec2(80, 80));
	
	if(this->tapObject == TapObject::kbNms)
		this->dynamicSpriteRenderer->DrawSprite(*this->r_arm_mouse, glm::vec2(3, 100), glm::vec2(125, 125), (42.25 * (3.14f / 180)));
	else
		this->spriteRenderer->DrawSprite(*this->r_arm, glm::vec2(0, 0), glm::vec2(300, 300));

	this->spriteRenderer->DrawSprite(*this->l_arm, glm::vec2(0, 0), glm::vec2(300, 300));
	
	//draw transparent objects above this comment
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void BongoApp::ChangeTapObject(const char* resource, TapObject objectType)
{
	if (BongoApp::tapObject != objectType)
	{
		this->tapObjTex.reset();
		this->tapObjTex = std::make_unique<Texture>(*Texture::loadTextureFromFile(resource, GL_TRUE));
		BongoApp::tapObject = objectType;
	}
}

void BongoApp::Update(float dt)
{
	int state = glfwGetKey(window, GLFW_KEY_1);
	if (state == GLFW_PRESS)
	{
		this->tapObjTex.reset();
		BongoApp::tapObject = TapObject::none;
		//this->UnhookCallback();
	}
	state = glfwGetKey(window, GLFW_KEY_2);
	if (state == GLFW_PRESS)
	{
		ChangeTapObject("Resources/ducks.png", TapObject::ducks);
		this->UnhookCallback();
	}
	state = glfwGetKey(window, GLFW_KEY_3);
	if (state == GLFW_PRESS)
	{
		ChangeTapObject("Resources/kb.png", TapObject::keyboard);
		this->UnhookCallback();
	}
	state = glfwGetKey(window, GLFW_KEY_4);
	if (state == GLFW_PRESS)
	{
		ChangeTapObject("Resources/synth.png", TapObject::synth);
		this->UnhookCallback();
	}
	state = glfwGetKey(window, GLFW_KEY_5);
	if (state == GLFW_PRESS)
	{
		ChangeTapObject("Resources/ak.png", TapObject::ak);
		this->UnhookCallback();
	}
	state = glfwGetKey(window, GLFW_KEY_6);
	if (state == GLFW_PRESS)
	{
		ChangeTapObject("Resources/kbnpad.png", TapObject::kbNms);
		this->HookCallback();
	}
	// TODO: change to mouse + keyboard 
}

void BongoApp::Close()
{
	glfwTerminate();
}