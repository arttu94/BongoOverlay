#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int ID;

	Shader();
	Shader(const GLchar* vertexPth, const GLchar* fragmentPth);

	~Shader();

	Shader* Use();

	void SetInt(const std::string& name, int value) const;
	void SetBool(const std::string& name, bool value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetMatrix4(const std::string& name, glm::mat4x4&) const;
	void SetVector3f(const std::string& name, glm::fvec3&) const;

private:
	bool ShaderCompileLog(const unsigned int& shader);
};

