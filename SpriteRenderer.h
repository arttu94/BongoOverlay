#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

class Shader;
class Texture;

class SpriteRenderer
{
public: 
	SpriteRenderer(std::shared_ptr<Shader> shader);
	//SpriteRenderer(Shader& shader);
	~SpriteRenderer();

	void DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.f, 10.f), GLfloat rotate = 0.f, glm::vec3 color = glm::vec3(1.f));

	GLuint VBO;
private:
	std::shared_ptr<Shader> shader;
	//Shader shader;
	GLuint quadVAO;

	void InitRenderData();
};

