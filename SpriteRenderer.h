#pragma once
#include <memory>
#include <glm.hpp>
#include "Shader.h"

class Shader;
class Texture;

class SpriteRenderer
{
public: 
	SpriteRenderer(std::shared_ptr<Shader> shader);
	//SpriteRenderer(Shader& shader);
	~SpriteRenderer();

	void DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.f, 10.f), float rotate = 0.f, glm::vec3 color = glm::vec3(1.f));

private:
	std::shared_ptr<Shader> shader;
	//Shader shader;
	unsigned int quadVAO;

	void InitRenderData();
};

