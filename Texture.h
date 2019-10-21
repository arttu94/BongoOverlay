#pragma once

#include <GL/glew.h>

class Texture
{
public:
	GLuint ID;

	GLuint Width, Height;

	GLuint InternalFormat;
	GLuint ImageFormat;

	GLuint Wrap_S;
	GLuint Wrap_T;

	GLuint Filter_Min; 
	GLuint Filter_Max; 

	char* source;

	Texture();
	~Texture();

	static Texture* loadTextureFromFile(const GLchar* file, GLboolean alpha);

	void Generate(GLuint width, GLuint height, unsigned char* data);

	void Bind() const;

};

