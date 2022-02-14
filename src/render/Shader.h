#pragma once

#include "../define.h"

static std::string readFile(const std::string filename ) {
	std::string line, text;
	std::ifstream file(filename);

	while (std::getline(file, line)) text += (line + '\n');

	return text;
}

class Shader {
public:
	unsigned int ID;

private:
	std::map<const char*, unsigned int> uniformCache;

public:
	Shader() {}

	void Init(const std::string &vertex_shader_filename, const std::string &fragment_shader_filename) {
		ID = glCreateProgram();
		
		compileAttach<GL_FRAGMENT_SHADER>(readFile(fragment_shader_filename));
		compileAttach<GL_VERTEX_SHADER>(readFile(vertex_shader_filename));
		
		glLinkProgram(ID);
	}

	template<unsigned int sType>
	void compileAttach(const std::string &source) {
		unsigned int shader = glCreateShader(sType);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		unsigned int result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, (GLint*)&result);

		if (!result) {
			unsigned int logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, (GLint*)&logLength);
			GLchar* message = (GLchar*)alloca(logLength*sizeof(char));
			glGetShaderInfoLog(shader, logLength, (GLsizei*)&logLength, message);
			std::cout << message << std::endl;
		}

		assert(result);

		glAttachShader(ID, shader);
		glDeleteShader(shader);
	}

	void bind() {
		glUseProgram(ID);
	}
	void unBind() {
		glUseProgram(0);
	}

	unsigned int &getUniformLocation(const char* uniform) {
		if (uniformCache.find(uniform) == uniformCache.end()) {
			uniformCache[uniform] = glGetUniformLocation(ID, uniform);
		}
		return uniformCache[uniform];
	}

	void setUniformMat(const char* uniform, const glm::mat4 &matrix) {
		glUniformMatrix4fv(getUniformLocation(uniform), 1, GL_FALSE, &matrix[0][0]);
	}
	void setUniformMat(const char* uniform, const glm::mat3 &matrix) {
		glUniformMatrix3fv(getUniformLocation(uniform), 1, GL_FALSE, &matrix[0][0]);
	}
	void setUniformf(const char* uniform, float v0) {
		glUniform1f(getUniformLocation(uniform), v0);
	}
	void setUniformf(const char* uniform, float v0, float v1) {
		glUniform2f(getUniformLocation(uniform), v0, v1);
	}
	void setUniformf(const char* uniform, float v0, float v1, float v2) {
		glUniform3f(getUniformLocation(uniform), v0, v1, v2);
	}
	void setUniformf(const char* uniform, float v0, float v1, float v2, float v3) {
		glUniform4f(getUniformLocation(uniform), v0, v1, v2, v3);
	}
	void setUniformiv(const char* uniform, int *array) {
		glUniform1iv(getUniformLocation(uniform), sizeof(array)/sizeof(array[0]), array);
	}
	void setUniformVec(const char* uniform, const glm::vec3 &vec) {
		glUniform3fv(getUniformLocation(uniform), 1, &vec[0]);
	}
};
