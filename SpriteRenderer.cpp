#include "SpriteRenderer.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include "Texture.h"
#include <iostream>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

SpriteRenderer::SpriteRenderer(std::shared_ptr<Shader> shader)
{
	this->shader = shader;
	this->InitRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
	std::cout << "destroying spriteRenderer" << std::endl;
	glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
	this->shader->Use();
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(position, 0.f));

	//make the pivot point the center of the sprite
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.f));
	//rotate
	model = glm::rotate(model, rotate, glm::vec3(0.f, 0.f, 1.f));
	//undo the pivot
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.f));

	model = glm::scale(model, glm::vec3(size, 1.f));

	this->shader->SetMatrix4("model", model);
	this->shader->SetVector3f("tint", color);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::InitRenderData()
{
	//VBO;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	this->quadVAO = 0;
	glGenVertexArrays(1, &this->quadVAO);
	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
