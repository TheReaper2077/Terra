#pragma once

#include "../define.h"
#include "VertexArray.h"
#include "Types.h"
#include "Texture.h"
#include "Shader.h"
#include "../TileRegistry.h"

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

	VertexArray *VAO = VertexArray::sharedInstance();
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

private:
	Renderer() {}

public:
	static Renderer *SharedInstance() {
		static Renderer instance;
		return &instance;
	}

	void Init() {
		VAO->Init(LAYOUT);

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
		return texture_handler->loadTexture(filename);
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
		
		VAO->getEBO()->Add(indices);
	}

public:
	void SetColor(int r, int g, int b, int a) {
		this->color[0] = r/255.0;
		this->color[1] = g/255.0;
		this->color[2] = b/255.0;
		this->color[3] = a/255.0;
	}

	void SetTextureColor(int r, int g, int b, int a) {
		this->tex_color[0] = r/255.0;
		this->tex_color[1] = g/255.0;
		this->tex_color[2] = b/255.0;
		this->tex_color[3] = a/255.0;
	}

	// Point - V, {}

	void DrawPoint(float x0, float y0) {
		if (render_type != POINT) {
			Render();
		}

		vertices.push_back(Vertex2D{x0, y0, color});

		render_type = POINT;
	}

	void DrawPoint(const std::vector<float> &points) {
		assert(points.size() % 2 == 0);

		if (render_type != POINT) {
			Render();
		}

		vertices.reserve(vertices.size() + points.size()/2);

		for (int i = 0; i < points.size(); i += 2) {
			vertices.emplace_back(Vertex2D{points[i], points[i+1], color});
		}

		render_type = POINT;
	}

	// Line - V, {}

	void DrawLine(float x0, float y0, float x1, float y1) {
		if (render_type != LINE) {
			Render();
		}

		vertices.reserve(vertices.size() + 2);

		vertices.emplace_back(Vertex2D{x0, y0, color});
		vertices.emplace_back(Vertex2D{x1, y1, color});

		render_type = LINE;
	}

	void DrawLine(const std::vector<float> &points) {
		assert(points.size() % 4 == 0);

		if (render_type != LINE) {
			Render();
		}

		vertices.reserve(vertices.size() + points.size()/2);

		for (int i = 0; i < points.size(); i += 4) {
			vertices.emplace_back(Vertex2D{points[i], points[i+1], color});
			vertices.emplace_back(Vertex2D{points[i+2], points[i+3], color});
		}

		render_type = LINE;
	}

	// Triangle - V, {}, FILL, DRAW, TEXTURE
private:
	void DrawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, RenderType render_type) {
		if (this->render_type != render_type) {
			Render();
		}

		vertices.reserve(vertices.size() + 3);

		vertices.emplace_back(Vertex2D{x0, y0, color});
		vertices.emplace_back(Vertex2D{x1, y1, color});
		vertices.emplace_back(Vertex2D{x2, y2, color});

		this->render_type = render_type;
	}

	void DrawTriangle(const std::vector<float> &points, RenderType render_type) {
		assert(points.size() % 6 == 0);

		if (this->render_type != render_type) {
			Render();
		}

		vertices.reserve(vertices.size() + points.size()/2);

		for (int i = 0; i < points.size(); i += 6) {
			vertices.emplace_back(Vertex2D{points[i], points[i+1], color});
			vertices.emplace_back(Vertex2D{points[i+2], points[i+3], color});
			vertices.emplace_back(Vertex2D{points[i+4], points[i+5], color});
		}

		this->render_type = render_type;
	}

public:
	void DrawTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
		DrawTriangle(x0, y0, x1, y1, x2, y2, DRAW_TRIANGLE);
	}

	void DrawTriangle(const std::vector<float> &points) {
		DrawTriangle(points, DRAW_TRIANGLE);
	}

	void FillTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
		DrawTriangle(x0, y0, x1, y1, x2, y2, FILL_TRIANGLE);
	}

	void FillTriangle(const std::vector<float> &points) {
		DrawTriangle(points, FILL_TRIANGLE);
	}

	// Quad - Fill, Texture

private:
	void RenderRect(float x, float y, float w, float h, RenderType render_type) {
		if (this->render_type != render_type) {
			Render();
		}
		
		curr_quads++;

		vertices.reserve(vertices.size() + 4);

		vertices.emplace_back(Vertex2D{x, y, color});
		vertices.emplace_back(Vertex2D{x, y + h, color});
		vertices.emplace_back(Vertex2D{x + w, y + h, color});
		vertices.emplace_back(Vertex2D{x + w, y, color});

		this->render_type = render_type;
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

public:
	void FillRect(float x, float y, float w, float h) {
		RenderRect(x, y, w, h, FILL_RECT);
	}

	void FillRect(glm::vec3 &pos, float w, float h) {
		RenderRect(pos.x, pos.y, w, h, FILL_RECT);
	}

	void DrawRect(float x, float y, float w, float h) {
		RenderRect(x, y, w, h, DRAW_RECT);
	}

	void DrawRect(glm::vec3 &pos, float w, float h) {
		RenderRect(pos.x, pos.y, w, h, DRAW_RECT);
	}

	void DrawCube(const glm::ivec3 &pos, float w, float h, float d) {
		RenderCube(pos.x, pos.y, pos.z, w, h, d, DRAW_CUBE);
	}

	void TextureRect(const glm::vec2 &pos, const glm::vec2 &dimension, const glm::vec2 &uv_pos, const glm::vec2 &uv_size, Texture *texture) {
		TextureRect(pos.x, pos.y, dimension.x, dimension.y, uv_pos.x, uv_pos.y, uv_size.x, uv_size.y, texture);
	}

	void TextureRect(const glm::vec2 &pos, const glm::vec2 &dimension, Tile &tile) {
		TextureRect(pos.x, pos.y, dimension.x, dimension.y, tile.x, tile.y, tile.w, tile.h, &tile.texture);
	}

	void TextureRect(float x, float y, float w, float h, Tile &tile) {
		TextureRect(x, y, w, h, tile.x, tile.y, tile.w, tile.h, &tile.texture);
	}

	void TextureRect(float x, float y, float w, float h, float tx, float ty, float tw, float th, Texture *texture) {
		if (next_tex_index > 5 || render_type != TEXTURE_RECT) {
			Render();
		}

		curr_quads++;

		tx /= texture->width;
		ty /= texture->height;
		tw /= texture->width;
		th /= texture->height;

		vertices.reserve(vertices.size() + 4);
		
		if (index_tex_map.find(texture->id) == index_tex_map.end()) {
			index_tex_map[texture->id] = next_tex_index;
			texture_handler->bindTextureUnit(next_tex_index, texture->id);
			next_tex_index++;
		}

		float index = index_tex_map[texture->id];

		vertices.emplace_back(Vertex2D{x, y, tx + tw, ty + th, index});
		vertices.emplace_back(Vertex2D{x, y + h, tx + tw, ty, index});
		vertices.emplace_back(Vertex2D{x + w, y + h, tx, ty, index});
		vertices.emplace_back(Vertex2D{x + w, y, tx, ty + th, index});

		render_type = TEXTURE_RECT;
	}

	void TextureCube(float x, float y, float z, float w, float h, float d, Tile &tile, const Faces &faces) {
		TextureCube(x, y, z, w, h, d, tile.x, tile.y, tile.w, tile.h, &tile.texture, faces, vertices);
	}

	void TextureCube(float x, float y, float z, float w, float h, float d, Tile &tile, const Faces &faces, std::vector<Vertex2D> &mesh) {
		TextureCube(x, y, z, w, h, d, tile.x, tile.y, tile.w, tile.h, &tile.texture, faces, mesh);
	}

	void TextureCube(float x, float y, float z, float w, float h, float d, float tx, float ty, float tw, float th, Texture *texture, const Faces &faces) {
		TextureCube(x, y, z, w, h, d, tx, ty, tw, th, texture, faces, vertices);
	}

	void TextureCube(float x, float y, float z, float w, float h, float d, float tx, float ty, float tw, float th, Texture *texture, const Faces &faces, std::vector<Vertex2D> &mesh) {
		if (next_tex_index > 5 || render_type != TEXTURE_CUBE) {
			Render();
		}

		curr_quads += faces.count();

		tx /= texture->width;
		ty /= texture->height;
		tw /= texture->width;
		th /= texture->height;

		mesh.reserve(mesh.size() + faces.count());
		
		if (index_tex_map.find(texture->id) == index_tex_map.end()) {
			index_tex_map[texture->id] = next_tex_index;
			next_tex_index++;
		}

		float index = index_tex_map[texture->id];

		// front
		// back
		// top
		// bottom
		// left
		// right

		if (faces[0]) {
			// mesh.emplace_back(Vertex2D{x, y, z, tx, ty, index});
			// mesh.emplace_back(Vertex2D{x, y + h, z, tx, ty + th, index});
			// mesh.emplace_back(Vertex2D{x + w, y + h, z, tx + tw, ty + th, index});
			// mesh.emplace_back(Vertex2D{x + w, y, z, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x, y, z, tx + tw, ty + th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, tx, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, tx, ty + th, index});
		}
		if (faces[1]) {
			// mesh.emplace_back(Vertex2D{x, y, z + d, tx, ty, index});
			// mesh.emplace_back(Vertex2D{x, y + h, z + d, tx, ty + th, index});
			// mesh.emplace_back(Vertex2D{x + w, y + h, z + d, tx + tw, ty + th, index});
			// mesh.emplace_back(Vertex2D{x + w, y, z + d, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x, y, z + d, tx + tw, ty + th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, tx, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z + d, tx, ty + th, index});
		}
		if (faces[2]) {
			mesh.emplace_back(Vertex2D{x, y + h, z, tx + tw, ty + th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, tx, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, tx, ty + th, index});
		}
		if (faces[3]) {
			mesh.emplace_back(Vertex2D{x, y, z, tx + tw, ty + th, index});
			mesh.emplace_back(Vertex2D{x, y, z + d, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z + d, tx, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, tx, ty + th, index});
		}
		if (faces[4]) {
			mesh.emplace_back(Vertex2D{x, y, z + d, tx + tw, ty + th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x, y + h, z, tx, ty, index});
			mesh.emplace_back(Vertex2D{x, y, z, tx, ty + th, index});
		}
		if (faces[5]) {
			mesh.emplace_back(Vertex2D{x + w, y, z + d, tx + tw, ty + th, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, tx + tw, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, tx, ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, tx, ty + th, index});
		}

		render_type = TEXTURE_CUBE;
	}

	float GetTextureIndex(Texture *texture) {
		if (index_tex_map.find(texture->id) == index_tex_map.end()) {
			index_tex_map[texture->id] = next_tex_index;
			next_tex_index++;
		}

		return index_tex_map[texture->id];
	}

	TileRegistry *tile_registry = TileRegistry::SharedInstance();

	void TextureCube(float x, float y, float z, float w, float h, float d, const BlockID &id, const Faces &faces, std::vector<Vertex2D> &mesh) {
		if (next_tex_index > 5 || render_type != TEXTURE_CUBE) {
			Render();
		}

		auto& block = tile_registry->GetBlock(id);

		auto& front = tile_registry->GetTile(block.front);
		auto& back = tile_registry->GetTile(block.back);
		auto& top = tile_registry->GetTile(block.top);
		auto& bottom = tile_registry->GetTile(block.bottom);
		auto& left = tile_registry->GetTile(block.left);
		auto& right = tile_registry->GetTile(block.right);

		curr_quads += faces.count();

		mesh.reserve(mesh.size() + faces.count());

		// front
		// back
		// top
		// bottom
		// left
		// right

		float index;

		if (faces[0]) {
			index = GetTextureIndex(&front.texture);
			
			mesh.emplace_back(Vertex2D{x, y, z, front.tx + front.tw, front.ty + front.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z, front.tx + front.tw, front.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, front.tx, front.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, front.tx, front.ty + front.th, index});
		}
		if (faces[1]) {
			index = GetTextureIndex(&back.texture);

			mesh.emplace_back(Vertex2D{x, y, z + d, back.tx + back.tw, back.ty + back.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, back.tx + back.tw, back.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, back.tx, back.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z + d, back.tx, back.ty + back.th, index});
		}
		if (faces[3]) {
			index = GetTextureIndex(&bottom.texture);

			mesh.emplace_back(Vertex2D{x, y, z, bottom.tx + bottom.tw, bottom.ty + bottom.th, index});
			mesh.emplace_back(Vertex2D{x, y, z + d, bottom.tx + bottom.tw, bottom.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z + d, bottom.tx, bottom.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, bottom.tx, bottom.ty + bottom.th, index});
		}
		if (faces[2]) {
			index = GetTextureIndex(&top.texture);

			mesh.emplace_back(Vertex2D{x, y + h, z, top.tx + top.tw, top.ty + top.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, top.tx + top.tw, top.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, top.tx, top.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, top.tx, top.ty + top.th, index});
		}
		if (faces[4]) {
			index = GetTextureIndex(&left.texture);

			mesh.emplace_back(Vertex2D{x, y, z + d, left.tx + left.tw, left.ty + left.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, left.tx + left.tw, left.ty, index});
			mesh.emplace_back(Vertex2D{x, y + h, z, left.tx, left.ty, index});
			mesh.emplace_back(Vertex2D{x, y, z, left.tx, left.ty + left.th, index});
		}
		if (faces[5]) {
			index = GetTextureIndex(&right.texture);

			mesh.emplace_back(Vertex2D{x + w, y, z + d, right.tx + right.tw, right.ty + right.th, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, right.tx + right.tw, right.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, right.tx, right.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, right.tx, right.ty + right.th, index});
		}

		render_type = TEXTURE_CUBE;
	}
	
	bool polygon_mode = true;
	void WireMode(bool value) {
		if (polygon_mode != value) {
			glPolygonMode(GL_FRONT_AND_BACK, (value) ? GL_FILL : GL_LINE);
			polygon_mode = value;
		}
	}

	void Render() {
		if (vertices.size() == 0) return;

		if (render_type == LINE) {
			basic_shader->bind();

			VAO->getVBO()->Add(vertices);
			glDrawArrays(GL_LINES, 0, vertices.size());
		}
		
		if (render_type == POINT) {
			basic_shader->bind();

			VAO->getVBO()->Add(vertices);
			glDrawArrays(GL_POINTS, 0, vertices.size());
		}
		
		if (render_type == FILL_TRIANGLE) {
			basic_shader->bind();

			VAO->getVBO()->Add(vertices);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		
		if (render_type == DRAW_TRIANGLE) {
			basic_shader->bind();

			VAO->getVBO()->Add(vertices);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		if (render_type == FILL_RECT) {
			if (curr_quads > max_quads) {
				IncreaseRectIndices();
			}

			basic_shader->bind();

			VAO->getVBO()->Add(vertices);
			glDrawElements(GL_TRIANGLES, curr_quads*6, GL_UNSIGNED_INT, NULL);
			curr_quads = 0;
		}
		
		if (render_type == DRAW_RECT) {
			basic_shader->bind();

			VAO->getVBO()->Add(vertices);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_QUADS, 0, vertices.size());
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (render_type == DRAW_CUBE && !polygon_mode) {
			basic_shader->bind();

			VAO->getVBO()->Add(vertices);
			// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			// WireMode(false);
			glDrawArrays(GL_QUADS, 0, vertices.size());
			// if (polygon_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		if (render_type == TEXTURE_RECT) {
			if (curr_quads > max_quads) {
				IncreaseRectIndices();
			}

			texture_shader->bind();

			for (const auto& pair: index_tex_map) {
				texture_handler->bindTextureUnit(next_tex_index, pair.second);
			}

			VAO->getVBO()->Add(vertices);
			glDrawElements(GL_TRIANGLES, curr_quads*6, GL_UNSIGNED_INT, NULL);
			curr_quads = 0;
			next_tex_index = 0;
			index_tex_map.clear();
		}

		if (render_type == TEXTURE_CUBE) {
			if (curr_quads > max_quads) {
				IncreaseRectIndices();
			}

			texture_shader->bind();

			for (const auto& pair: index_tex_map) {
				texture_handler->bindTextureUnit(next_tex_index, pair.second);
			}

			VAO->getVBO()->Add(vertices);
			glDrawElements(GL_TRIANGLES, curr_quads*6, GL_UNSIGNED_INT, NULL);
			curr_quads = 0;
			next_tex_index = 0;
			index_tex_map.clear();
		}

		vertices.clear();
	}

	void RenderMesh(std::vector<Vertex2D> &mesh) {
		curr_quads = mesh.size()/4;

		if (curr_quads > max_quads) {
			IncreaseRectIndices();
		}

		texture_shader->bind();

		for (const auto& pair: index_tex_map) {
			texture_handler->bindTextureUnit(next_tex_index, pair.second);
		}

		VAO->getVBO()->Add(mesh);
		glDrawElements(GL_TRIANGLES, mesh.size()*1.5, GL_UNSIGNED_INT, NULL);
		curr_quads = 0;
		next_tex_index = 0;
		index_tex_map.clear();
	}
};
