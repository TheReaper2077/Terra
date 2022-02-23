#pragma once

#include "../define.h"
#include "Buffer.h"
#include "Types.h"
#include "Texture.h"
#include "Shader.h"

using Faces = std::bitset<6>;

enum RenderType {
	LINE, POINT,
	FILL_TRIANGLE, DRAW_TRIANGLE,
	TEXTURE_RECT, FILL_RECT, DRAW_RECT,
	TEXTURE_CUBE, DRAW_CUBE
};


class Renderer {
private:
	Buffer<GL_UNIFORM_BUFFER> *UBO;
	std::vector<Vertex2D> vertices;
	std::vector<unsigned int> indices;

	TextureHandler *texture_handler = TextureHandler::sharedInstance();

	Shader *texture_shader, *basic_shader;
	RenderType render_type;

	float color[4] = {1, 1, 1, 1};
	float tex_color[4] = {1, 1, 1, 1};

	int curr_quads = 0, max_quads = 0;
	int curr_draw_quads = 0, max_draw_quads = 0;
	int next_tex_index = 0;

	std::map<unsigned int, unsigned int> index_tex_map;
	std::map<unsigned int, RenderType> layer_id_map;

public:
	bool polygon_mode = true;

	unsigned int vao;
	unsigned int vbo, ebo;

private:
	Renderer() {}

public:
	static Renderer *SharedInstance() {
		static Renderer instance;
		return &instance;
	}

	void Init() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		texture_shader = new Shader();
		texture_shader->Init(TEXTURE_VS, TEXTURE_FS);
		texture_shader->bind();

		glUniformBlockBinding(texture_shader->ID, glGetUniformBlockIndex(texture_shader->ID, "Matrices"), 0);
		
		basic_shader = new Shader();
		basic_shader->Init(BASIC_VS, BASIC_FS);
		basic_shader->bind();

		glUniformBlockBinding(basic_shader->ID, glGetUniformBlockIndex(basic_shader->ID, "Matrices"), 0);

		UBO = new Buffer<GL_UNIFORM_BUFFER>();
		UBO->Init();
		UBO->Allocate(3*sizeof(glm::mat4));
		UBO->BindRangeToIndex(0, NULL, 3*sizeof(glm::mat4));

		texture_handler->setup(texture_shader);
	}

	void CreateLayer(unsigned int layer_id, RenderType render_type) {

	}

	void SetProjection(const glm::mat4 &projection) {
		UBO->Add(0*sizeof(glm::mat4), sizeof(glm::mat4), &projection[0][0]);
	}

	void SetView(const glm::mat4 &view) {
		UBO->Add(1*sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);
	}

	void SetModel(const glm::mat4 &model) {
		UBO->Add(2*sizeof(glm::mat4), sizeof(glm::mat4), &model[0][0]);
	}

	Texture LoadTexture(const char *filename) {
		return texture_handler->LoadTexture(filename);
	}

private:
	void IncreaseRectIndices() {
		indices.reserve(curr_quads*6);

		for (int i = max_quads; i < curr_quads; i ++) {
			indices.emplace_back(0 + i * 4);
			indices.emplace_back(1 + i * 4);
			indices.emplace_back(3 + i * 4);
			indices.emplace_back(1 + i * 4);
			indices.emplace_back(3 + i * 4);
			indices.emplace_back(2 + i * 4);
		}
		
		max_quads = curr_quads;
		
		// VAO->getEBO()->Add(indices);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
	}

public:
	void SetColor(int r, int g, int b, int a) {
		this->color[0] = r/255.0;
		this->color[1] = g/255.0;
		this->color[2] = b/255.0;
		this->color[3] = a/255.0;
	}

	void RenderCube(float x, float y, float z, float w, float h, float d, RenderType render_type) {
		if (this->render_type != render_type) {
			Render();
		}

		curr_quads += 6;
		vertices.reserve(vertices.size() + 4*6);
		
		vertices.emplace_back(Vertex2D{x, y, z, color});
		vertices.emplace_back(Vertex2D{x, y + h, z, color});
		vertices.emplace_back(Vertex2D{x + w, y + h, z, color});
		vertices.emplace_back(Vertex2D{x + w, y, z, color});
		
		vertices.emplace_back(Vertex2D{x, y, z + d, color});
		vertices.emplace_back(Vertex2D{x, y + h, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y + h, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y, z + d, color});
		
		vertices.emplace_back(Vertex2D{x, y + h, z, color});
		vertices.emplace_back(Vertex2D{x, y + h, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y + h, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y + h, z, color});
		
		vertices.emplace_back(Vertex2D{x, y, z, color});
		vertices.emplace_back(Vertex2D{x, y, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y, z, color});
		
		vertices.emplace_back(Vertex2D{x, y, z + d, color});
		vertices.emplace_back(Vertex2D{x, y + h, z + d, color});
		vertices.emplace_back(Vertex2D{x, y + h, z, color});
		vertices.emplace_back(Vertex2D{x, y, z, color});
		
		vertices.emplace_back(Vertex2D{x + w, y, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y + h, z + d, color});
		vertices.emplace_back(Vertex2D{x + w, y + h, z, color});
		vertices.emplace_back(Vertex2D{x + w, y, z, color});

		this->render_type = render_type;
	}

	void DrawCube(const glm::ivec3 &pos, float w, float h, float d) {
		RenderCube(pos.x, pos.y, pos.z, w, h, d, DRAW_CUBE);
	}

	float GetTextureIndex(Texture *texture) {
		if (index_tex_map.find(texture->id) == index_tex_map.end()) {
			index_tex_map[texture->id] = next_tex_index;
			next_tex_index++;
		}

		return index_tex_map[texture->id];
	}

	void Render() {
		if (vertices.size() == 0) return;
		
		if (render_type == DRAW_CUBE && !polygon_mode) {
			basic_shader->bind();

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex2D), 0);
			glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex2D), (void*)offsetof(Vertex2D, color));

			glDrawArrays(GL_QUADS, 0, vertices.size());
			if (polygon_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		vertices.clear();
	}

	void RenderMesh(Buffer<GL_ARRAY_BUFFER> *buffer) {
		curr_quads = buffer->element_size/4;

		if (curr_quads > max_quads) {
			IncreaseRectIndices();
		}

		texture_shader->bind();

		for (const auto& pair: index_tex_map) {
			texture_handler->bindTextureUnit(next_tex_index, pair.second);
		}

		// VAO->bind();
		glBindVertexArray(vao);
		buffer->bind();

		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex2D), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex2D), (void*)offsetof(Vertex2D, color));

		glDrawElements(GL_TRIANGLES, buffer->element_size*1.5, GL_UNSIGNED_INT, NULL);

		curr_quads = 0;
		next_tex_index = 0;
		index_tex_map.clear();
	}
};
