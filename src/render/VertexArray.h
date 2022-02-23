#pragma once

#include "../define.h"
#include "Buffer.h"

class VertexArray {
private:
	unsigned int id;

	Buffer<GL_ARRAY_BUFFER> VBO;
	Buffer<GL_ELEMENT_ARRAY_BUFFER> EBO;

private:
	VertexArray() {}

public:
	static VertexArray *sharedInstance() {
		static VertexArray instance;
		return &instance;
	}

	void Init(const std::vector<unsigned int> &layout) {
		glGenVertexArrays(1, &id);
		
		VBO.Init();
		EBO.Init();

		bind();
		
		// setAttrib(layout);
		
	}

	Buffer<GL_ARRAY_BUFFER> *getVBO() {
		bind();
		return &VBO;
	}

	Buffer<GL_ELEMENT_ARRAY_BUFFER> *getEBO() {
		bind();
		return &EBO;
	}

	void drawElements() {

	}

	void setAttrib(const std::vector<unsigned int> &layout) {
		std::size_t stride = 0;
		for (int i = 0; i != layout.size(); i++) {
			stride += sizeof(float)*layout[i];
		}

		unsigned int offset = 0;
		for (int i = 0; i != layout.size(); i++) {
			glVertexAttribPointer(i, layout[i], GL_FLOAT, false, stride, (void*)offset);
			glEnableVertexAttribArray(i);
			offset += sizeof(float)*layout[i];
		}
	}

	void bind() {
		glBindVertexArray(id);
	}

	void unBind() {
		glBindVertexArray(0);
	}
};