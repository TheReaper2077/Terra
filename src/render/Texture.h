#pragma once

#include "../define.h"
#include "Shader.h"

struct Texture {
	unsigned int id;
	int width;
	int height;
};


class TextureHandler {
public:
	unsigned int MAX_TEXTURE_SLOTS = 5;

private:
	TextureHandler() {}

public:
	static TextureHandler *sharedInstance() {
		static TextureHandler instance;
		return &instance;
	}

	void setup(Shader *shader) {
		int texture_indices[MAX_TEXTURE_SLOTS];

		for (int i = 0; i != MAX_TEXTURE_SLOTS; i++) {
			texture_indices[i] = i;
		}
		
		shader->setUniformiv("u_Textures", texture_indices);
	}

	Texture loadTexture(const char *filename) {
		unsigned int id;
		int width, height, nr_channels;
		unsigned char* data;

		glGenTextures(1, &id);
		bindTexture(id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(id);

		data = stbi_load(filename, &width, &height, &nr_channels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);

		return Texture{id, width, height};
	}

	void bindTexture(const unsigned int &texture) {
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	void bindTextureUnit(const unsigned int &unit, const unsigned int &texture) {
		glBindTextureUnit(unit, texture);
	}

	void unBind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
