#include "Texture.h"
#include <SOIL/SOIL.h>
#include <iostream>

Texture::Texture()
	: Width(0), Height(0), InternalFormat(GL_RGB), ImageFormat(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR), source(nullptr)
{
	glGenTextures(1, &this->ID);
}

Texture::~Texture()
{
	std::cout << "destroying texture with source: " << this->source << std::endl;
}

Texture* Texture::loadTextureFromFile(const GLchar* file, GLboolean alpha)
{
	// Create Texture object
	Texture* texture = new Texture();
	if (alpha)
	{
		texture->InternalFormat = GL_RGBA;
		texture->ImageFormat = GL_RGBA;
	}
	// Load image
	int width, height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, texture->ImageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
	// Now generate texture
	texture->Generate(width, height, image);
	texture->source = (char*)file;
	// And finally free image data
	SOIL_free_image_data(image);
	return texture;
}

void Texture::Generate(GLuint width, GLuint height, unsigned char* data)
{
	this->Width = width;
	this->Height = height;

	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}
