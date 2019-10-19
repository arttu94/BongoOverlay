#pragma once
#include <Windows.h>
#include <memory>

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
	synth
};

class BongoApp
{
public:
	BongoApp();

	~BongoApp();

	static void KeyCallback(GLFWwindow*, int, int, int, int);

	void SetInput();

	bool Init();

	void LoadResources();

	bool MakeGLWindowTransparent(COLORREF colorKey);

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

	static bool isMuted;
	static TapObject tapObject;

private:
	GLFWwindow* window;

	std::shared_ptr<Shader> shader;

	std::unique_ptr<Texture> bodyTex;
	std::unique_ptr<Texture> tapObjTex;

	std::unique_ptr<SpriteRenderer> spriteRenderer;
};

