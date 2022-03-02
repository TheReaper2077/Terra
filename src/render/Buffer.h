#pragma once

#include "../define.h"

template <unsigned int Target>
class Buffer {
public:
	unsigned int ID;
	std::size_t curr_size = 0, used_size = 0, element_size = 0;
	bool init = false;
	bool vbo_bound = false;
	
public:
	Buffer() {}

	void Init() {
		glGenBuffers(1, &ID);
		init = true;
	}

	void bind() {
		glBindBuffer(Target, ID);
	}

	void unBind() {
		glBindBuffer(Target, 0);
	}

	template <typename T>
	void Add(const T &data, std::size_t offset = 0) {
		bind();
		curr_size = data.size()*sizeof(data[0]);
		if (data.size() > element_size) element_size = data.size();

		if (curr_size > used_size) {
			glBufferData(Target, curr_size, data.data(), GL_DYNAMIC_DRAW);
			used_size = curr_size;
		} else {
			glBufferSubData(Target, offset, curr_size, data.data());
		}
	}

	template <typename T>
	void Add(unsigned int offset, const std::size_t &size, const T &data) {
		bind();
		glBufferSubData(Target, offset, size, data);
	}

	void Allocate(std::size_t allocate_size, std::size_t single_element_size) {
		if (allocate_size > element_size) {
			used_size = allocate_size*single_element_size;
			element_size = allocate_size;
			bind();
			glBufferData(Target, used_size, NULL, GL_DYNAMIC_DRAW);
		}
	}

	void BindRangeToIndex(const unsigned int &index, const unsigned int &offset, const std::size_t &size) {
		glBindBufferRange(Target, index, ID, offset, size);
	}
};