#pragma once
#include <Windows.h>
#include <memory>
#include <glm/glm.hpp>

struct GLFWwindow;
class Shader;
class SpriteRenderer;
class Texture;

enum TapObject
{
	none = 0,
	ducks,
	keyboard,
	bongoes,
	synth,
	ak,
	kbNms
};

enum Limb
{
	na = 0,
	right,
	left
};

class BongoApp
{
public:
	BongoApp();

	~BongoApp();

	static void KeyCallback(GLFWwindow*, int, int, int, int);

	void SetMousePos(float x, float y);

	void SetInput();

	bool Init();

	void LoadResources();

	bool MakeGLWindowTransparent(COLORREF colorKey);

	void ChangeTapObject(const char* resource, TapObject objectType);

	void Update(float dt);

	void Render();

	void Close();

	GLFWwindow* GetGLFWWindow() { return this->window; }

	Texture* r_arm_d;
	Texture* r_arm_u;
	Texture* r_arm;

	Texture* l_arm_d;
	Texture* l_arm_u;
	Texture* l_arm;

	Texture* r_arm_mouse;

	Limb lastUsedLimb;

	static bool isMuted;
	static TapObject tapObject;
	std::unique_ptr<SpriteRenderer> dynamicSpriteRenderer;

	bool (*HookCallback)();
	void (*UnhookCallback)();

private:
	GLFWwindow* window;

	glm::vec2 mousePos;

	std::shared_ptr<Shader> shader;

	std::unique_ptr<Texture> bodyTex;
	std::unique_ptr<Texture> tapObjTex;
	std::unique_ptr<Texture> mouseTex;

	std::unique_ptr<SpriteRenderer> spriteRenderer;
};

