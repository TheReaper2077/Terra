#pragma once

#include "../define.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

enum RenderType {
	LINE, POINT,
	FILL_TRIANGLE, DRAW_TRIANGLE,
	TEXTURE_RECT, FILL_RECT, DRAW_RECT,
	TEXTURE_CUBE, DRAW_CUBE, FILL_CUBE,
	UI_LINE,
};

class Renderer {
private:
	Buffer<GL_UNIFORM_BUFFER> *UBO;
	
	std::vector<Vertex3D> vertices3D;
	std::vector<Vertex2D> vertices2D;

	std::vector<unsigned int> indices;

	TextureHandler *texture_handler = nullptr;

	Shader *texture_shader, *basic_shader, *ui_basic_shader;
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

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

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

		ui_basic_shader = new Shader();
		ui_basic_shader->Init(UI_BASIC_VS, UI_BASIC_FS);
		ui_basic_shader->bind();

		glUniformBlockBinding(ui_basic_shader->ID, glGetUniformBlockIndex(ui_basic_shader->ID, "Matrices_2D"), 1);

		UBO = new Buffer<GL_UNIFORM_BUFFER>();
		UBO->Init();
		UBO->Allocate(2*sizeof(glm::mat4));
		UBO->BindRangeToIndex(0, NULL, sizeof(glm::mat4));
		UBO->BindRangeToIndex(1, sizeof(glm::mat4), sizeof(glm::mat4));

		texture_handler = new TextureHandler();
		texture_handler->Init(texture_shader);

		glm::mat4 ortho_projection = glm::ortho<float>(0, WIDTH, HEIGHT, 0, -1, 1);
		glm::mat4 ortho_view = glm::mat4(1.0f);
		glm::mat4 ortho_model = glm::mat4(1.0f);

		UBO->Add(sizeof(glm::mat4), sizeof(glm::mat4), &(ortho_projection * ortho_view * ortho_model)[0][0]);
	}

	void SetProjection(const glm::mat4 &projection) {
		if (this->projection == projection) return;
		this->projection = projection;
	
		UpdateMVP();
	}

	void SetView(const glm::mat4 &view) {
		if (this->view == view) return;
		this->view = view;
	
		UpdateMVP();
	}

	void SetModel(const glm::mat4 &model) {
		if (this->model == model) return;
		this->model = model;

		UpdateMVP();
	}

	void UpdateMVP() {
		UBO->Add(0, sizeof(glm::mat4), &(projection * view * model)[0][0]);
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

	void RenderLine(float x0, float y0, float x1, float y1, RenderType render_type) {
		if (this->render_type != render_type) {
			Render();
		}

		vertices2D.reserve(2 + vertices2D.size());

		vertices2D.emplace_back(Vertex2D{x0, y0, color});
		vertices2D.emplace_back(Vertex2D{x1, y1, color});

		this->render_type = render_type;
	}

	void RenderLine(float x0, float y0, float z0, float x1, float y1, float z1, RenderType render_type) {
		if (this->render_type != render_type) {
			Render();
		}

		vertices3D.reserve(2 + vertices3D.size());

		vertices3D.emplace_back(Vertex3D{x0, y0, z0, color});
		vertices3D.emplace_back(Vertex3D{x1, y1, z1, color});

		this->render_type = render_type;
	}

	void DrawLine(float x0, float y0, float x1, float y1) {
		RenderLine(x0, y0, x1, y1, UI_LINE);
	}

	void DrawLine(float x0, float y0, float z0, float x1, float y1, float z1) {
		RenderLine(x0, y0, z0, x1, y1, z1, LINE);
	}

	void RenderCube(float x, float y, float z, float w, float h, float d, RenderType render_type, uint8_t faces) {
		if (this->render_type != render_type) {
			Render();
		}

		curr_quads += (faces & 0b000001 + faces & 0b000010 + faces & 0b000100 + faces & 0b001000 + faces & 0b010000 + faces & 0b100000);
		vertices3D.reserve(vertices3D.size() + 4*curr_quads);

		if (faces & 0b000001) {
			vertices3D.emplace_back(Vertex3D{x, y, z, color});
			vertices3D.emplace_back(Vertex3D{x, y + h, z, color});
			vertices3D.emplace_back(Vertex3D{x + w, y + h, z, color});
			vertices3D.emplace_back(Vertex3D{x + w, y, z, color});
		}

		if (faces & 0b000010) {
			vertices3D.emplace_back(Vertex3D{x, y, z + d, color});
			vertices3D.emplace_back(Vertex3D{x, y + h, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y + h, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y, z + d, color});
		}
		
		if (faces & 0b000100) {
			vertices3D.emplace_back(Vertex3D{x, y + h, z, color});
			vertices3D.emplace_back(Vertex3D{x, y + h, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y + h, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y + h, z, color});
		}
		
		if (faces & 0b001000) {
			vertices3D.emplace_back(Vertex3D{x, y, z, color});
			vertices3D.emplace_back(Vertex3D{x, y, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y, z, color});
		}
		
		if (faces & 0b010000) {
			vertices3D.emplace_back(Vertex3D{x, y, z + d, color});
			vertices3D.emplace_back(Vertex3D{x, y + h, z + d, color});
			vertices3D.emplace_back(Vertex3D{x, y + h, z, color});
			vertices3D.emplace_back(Vertex3D{x, y, z, color});
		}
		
		if (faces & 0b100000) {
			vertices3D.emplace_back(Vertex3D{x + w, y, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y + h, z + d, color});
			vertices3D.emplace_back(Vertex3D{x + w, y + h, z, color});
			vertices3D.emplace_back(Vertex3D{x + w, y, z, color});
		}

		this->render_type = render_type;
	}

	void DrawCube(const glm::ivec3 &pos, float w, float h, float d, uint8_t faces = 0b11111) {
		RenderCube(pos.x, pos.y, pos.z, w, h, d, DRAW_CUBE, faces);
	}

	void FillCube(const glm::ivec3 &pos, float w, float h, float d, uint8_t faces = 0b11111) {
		RenderCube(pos.x, pos.y, pos.z, w, h, d, FILL_CUBE, faces);
	}

	void FillCube(const glm::vec3 &pos, float w, float h, float d, uint8_t faces = 0b11111) {
		RenderCube(pos.x, pos.y, pos.z, w, h, d, FILL_CUBE, faces);
	}

	float GetTextureIndex(Texture *texture) {
		if (index_tex_map.find(texture->id) == index_tex_map.end()) {
			index_tex_map[texture->id] = next_tex_index;
			next_tex_index++;
		}

		return index_tex_map[texture->id];
	}

	void Render() {
		if (vertices3D.size() != 0) {
			if (render_type == DRAW_CUBE && !polygon_mode) {
				basic_shader->bind();

				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices3D.size() * sizeof(vertices3D[0]), vertices3D.data(), GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
				glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));

				glDrawArrays(GL_QUADS, 0, vertices3D.size());
				if (polygon_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			if (render_type == FILL_CUBE) {
				basic_shader->bind();

				if (curr_quads > max_quads) {
					IncreaseRectIndices();
				}

				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices3D.size() * sizeof(vertices3D[0]), vertices3D.data(), GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
				glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));

				glDrawElements(GL_TRIANGLES, vertices3D.size()*1.5, GL_UNSIGNED_INT, NULL);

				curr_quads = 0;
			}

			if (render_type == LINE) {
				basic_shader->bind();

				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices3D.size() * sizeof(vertices3D[0]), vertices3D.data(), GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
				glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));

				glDrawArrays(GL_TRIANGLES, 0, vertices3D.size());
			}
		}

		if (vertices2D.size() != 0) {
			if (render_type == UI_LINE) {
				ui_basic_shader->bind();

				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices2D.size() * sizeof(vertices2D[0]), vertices2D.data(), GL_STATIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex2D), 0);
				glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex2D), (void*)offsetof(Vertex2D, color));

				glDrawArrays(GL_LINES, 0, vertices2D.size());
			}
		}

		vertices3D.clear();
		vertices2D.clear();
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

		glBindVertexArray(vao);
		buffer->bind();

		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));

		glDrawElements(GL_TRIANGLES, buffer->element_size*1.5, GL_UNSIGNED_INT, NULL);

		curr_quads = 0;
		next_tex_index = 0;
		index_tex_map.clear();
	}
};
