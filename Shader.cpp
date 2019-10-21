#include "Shader.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

Shader::Shader(): ID(0)
{
}

Shader::Shader(const GLchar* vertexPth, const GLchar* fragmentPth)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open((char*)vertexPth);
		fShaderFile.open((char*)fragmentPth);

		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//compile

	GLuint vs, fs;
	int success;
	char infoLog[512];

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vShaderCode, nullptr);
	glCompileShader(vs);
	if (!ShaderCompileLog(vs))
		exit(1);

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fShaderCode, nullptr);
	glCompileShader(fs);
	if (!ShaderCompileLog(fs))
		exit(1);

	ID = glCreateProgram();
	glAttachShader(ID, vs);
	glAttachShader(ID, fs);
	glLinkProgram(ID);

	//delete shaders as they are linked to the program
	glDeleteShader(vs);
	glDeleteShader(fs);
}

Shader::~Shader()
{
	std::cout << "destroying shader with ID: " << this->ID << std::endl;
}

Shader* Shader::Use()
{
	glUseProgram(ID);
	return this;
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::SetMatrix4(const std::string& name, glm::mat4x4& mat) const	
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetVector3f(const std::string& name, glm::fvec3& vec3) const
{
	glUniform3f(glGetUniformLocation(this->ID, name.c_str()), vec3.x, vec3.y, vec3.z);
}

bool Shader::ShaderCompileLog(const unsigned int& shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << std::string(errorLog.begin(), errorLog.end()) << std::endl;

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
		return false;
	}
	else
	{
		return true;
	}
}
