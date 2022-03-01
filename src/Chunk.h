#pragma once

#include "ECS/Registry.h"
#include "Components/Components.h"
#include "render/Renderer.h"
#include "TileRegistry.h"

struct Chunk {
	glm::ivec3 id;

	BlockID chunk_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {AIR_BLOCK};
	std::vector<Vertex3D> mesh;
	Buffer<GL_ARRAY_BUFFER> buffer;

	bool generated;
	bool meshed;
	bool top_chunk;
	bool visible;

	Chunk(glm::ivec3 id) {
		this->id = id;

		this->generated = false;
		this->meshed = false;
		this->top_chunk = false;
		this->visible = true;
	}
	
	void Add(const glm::ivec3 &pos, const BlockID &block) {
		if (chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15] == block) return;

		chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15] = block;
		meshed = false;
	}

	void Add(const glm::ivec3 &pos, const BlockID &block, bool &front, bool &back, bool &top, bool &bottom, bool &left, bool &right) {
		if (chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15] == block) return;

		chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15] = block;
		meshed = false;

		front = ((pos.z & 15) == 0);
		back = ((pos.z & 15) == 15);
		top = ((pos.y & 15) == 15);
		bottom = ((pos.y & 15) == 0);
		left = ((pos.x & 15) == 0);
		right = ((pos.x & 15) == 15);
	}

	BlockID &Get(const glm::ivec3 &pos) {
		return chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15];
	}

	BlockID &Get(int x, int y, int z) {
		return chunk_blocks[z & 15][y & 15][x & 15];
	}
};


void DrawQuad(Chunk *chunk, float x, float y, float z, float w, float h, float d, const TileID &tile_id, int dir) {
	// auto& tile = tile_registry->GetTile(tile_id);
	// float index = renderer->GetTextureIndex(&tile.texture);
	auto& tile = TileRegistry::SharedInstance()->GetTile(tile_id);
	float index = Renderer::SharedInstance()->GetTextureIndex(&tile.texture);
	// std::cout << chunk->id.x << " " << chunk->id.y << " " << chunk->id.z << " \n";

	chunk->mesh.reserve(chunk->mesh.size() + 4);
	
	if (dir == 0) {
		chunk->mesh.emplace_back(Vertex3D{x, y, z, tile.tx + tile.tw, tile.ty + tile.th, index});
		chunk->mesh.emplace_back(Vertex3D{x, y + h, z, tile.tx + tile.tw, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y + h, z, tile.tx, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y, z, tile.tx, tile.ty + tile.th, index});
	}
	if (dir == 1) {
		chunk->mesh.emplace_back(Vertex3D{x, y, z + d, tile.tx + tile.tw, tile.ty + tile.th, index});
		chunk->mesh.emplace_back(Vertex3D{x, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y + h, z + d, tile.tx, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y, z + d, tile.tx, tile.ty + tile.th, index});
	}
	if (dir == 3) {
		chunk->mesh.emplace_back(Vertex3D{x, y, z, tile.tx + tile.tw, tile.ty + tile.th, index});
		chunk->mesh.emplace_back(Vertex3D{x, y, z + d, tile.tx + tile.tw, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y, z + d, tile.tx, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y, z, tile.tx, tile.ty + tile.th, index});
	}
	if (dir == 2) {
		chunk->mesh.emplace_back(Vertex3D{x, y + h, z, tile.tx + tile.tw, tile.ty + tile.th, index});
		chunk->mesh.emplace_back(Vertex3D{x, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y + h, z + d, tile.tx, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y + h, z, tile.tx, tile.ty + tile.th, index});
	}
	if (dir == 4) {
		chunk->mesh.emplace_back(Vertex3D{x, y, z + d, tile.tx + tile.tw, tile.ty + tile.th, index});
		chunk->mesh.emplace_back(Vertex3D{x, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x, y + h, z, tile.tx, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x, y, z, tile.tx, tile.ty + tile.th, index});
	}
	if (dir == 5) {
		chunk->mesh.emplace_back(Vertex3D{x + w, y, z + d, tile.tx + tile.tw, tile.ty + tile.th, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y + h, z, tile.tx, tile.ty, index});
		chunk->mesh.emplace_back(Vertex3D{x + w, y, z, tile.tx, tile.ty + tile.th, index});
	}
}

void MeshChunk(Chunk *chunk, Chunk *front, Chunk *back, Chunk *top, Chunk *bottom, Chunk *left, Chunk *right) {
	if (chunk->meshed) return;

	auto& chunk_blocks = chunk->chunk_blocks;
	auto& id = chunk->id;
	auto& visible = chunk->visible;
	
	chunk->meshed = true;

	for (int z = 0; z != CHUNK_SIZE; z++) {
		for (int y = 0; y != CHUNK_SIZE; y++) {
			for (int x = 0; x != CHUNK_SIZE; x++) {
					auto& block = chunk_blocks[z][y][x];
					if (block == AIR_BLOCK) continue;

					auto& tiles = TileRegistry::SharedInstance()->GetBlock(block);

					if (z - 1 >= 0) {
						if (chunk_blocks[z - 1][y][x] == AIR_BLOCK) {
							DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.front, 0);
						}
					} else {
						if (front != nullptr) {
							if (front->chunk_blocks[15][y][x] == AIR_BLOCK) {
								DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.front, 0);
								visible = true;
							}
						}
					}
					if (z + 1 < 16) {
						if (chunk_blocks[z + 1][y][x] == AIR_BLOCK) {
							DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.back, 1);
						}
					} else {
						if (back != nullptr) {
							if (back->chunk_blocks[0][y][x] == AIR_BLOCK) {
								DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.back, 1);
								visible = true;
							}
						}
					}

					if (y - 1 >= 0) {
						if (chunk_blocks[z][y - 1][x] == AIR_BLOCK) {
							DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.bottom, 3);
						}
					} else {
						if (bottom != nullptr) {
							if (bottom->chunk_blocks[z][15][x] == AIR_BLOCK) {
								DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.bottom, 3);
								visible = true;
							}
						}
					}
					if (y + 1 < 16) {
						if (chunk_blocks[z][y + 1][x] == AIR_BLOCK) {
							DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.top, 2);
						}
					} else {
						if (top != nullptr) {
							if (top->chunk_blocks[z][0][x] == AIR_BLOCK) {
								DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.top, 2);
								visible = true;
							}
						}
					}

					if (x - 1 >= 0) {
						if (chunk_blocks[z][y][x - 1] == AIR_BLOCK) {
							DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.left, 4);
						}
					} else {
						if (left != nullptr) {
							if (left->chunk_blocks[z][y][15] == AIR_BLOCK) {
								DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.left, 4);
								visible = true;
							}
						}
					}
					if (x + 1 < 16) {
						if (chunk_blocks[z][y][x + 1] == AIR_BLOCK) {
							DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.right, 5);
						}
					} else {
						if (right != nullptr) {
							if (right->chunk_blocks[z][y][0] == AIR_BLOCK) {
								DrawQuad(chunk, id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.left, 5);
								visible = true;
							}
						}
					}
			}
		}
	}
}

void GenerateChunk(Chunk *chunk) {
	auto& id = chunk->id;
	auto& visible = chunk->visible;
	auto& top_chunk = chunk->top_chunk;
	auto& generated = chunk->generated;

	int i = 0;
	float top_value = 0;

	// for (int z = 0; z != 16; z++) {
	// 	for (int x = 0; x != 16; x++) {
	// 		chunk->chunk_blocks[z][0][x] = GRASS_BLOCK;
	// 	}
	// }
	// top_chunk = true;
	// generated = true;
	// visible = true;
	
	for (int z = 0; z != 16; z++) {
		for (int x = 0; x != 16; x++) {
			float value = glm::perlin(glm::vec2{(id.x + x) / 64.0, (id.z + z) / 64.0});
			value = (value + 1) / 2;
			value *= 32 + 32;
			if (value > top_value) {
				top_value = value;
			}

			int idx = 0;	
			for (int y = id.y; y <= value; y++) {
				if (idx > 15) {
					i++;
					break;
				}
				if (y > value - 1) {
					chunk->chunk_blocks[z][idx][x] = GRASS_BLOCK;
				} else if (y > value - 4) {
					chunk->chunk_blocks[z][idx][x] = DIRT_BLOCK;
				} else {
					chunk->chunk_blocks[z][idx][x] = STONE_BLOCK;
				}
				idx++;
			}
		}
	}

	top_chunk = (top_value - id.y < 16);
	generated = true;
	if (visible) visible = (i < 256);
}

void RenderChunk(Chunk *chunk) {
	auto& chunk_blocks = chunk->chunk_blocks;
	auto& id = chunk->id;
	auto& visible = chunk->visible;
	auto& top_chunk = chunk->top_chunk;
	auto& generated = chunk->generated;

	if (visible && chunk->meshed && chunk->buffer.init) {
		// Renderer::SharedInstance()->SetModel(::ivec3()
		Renderer::SharedInstance()->RenderMesh(&chunk->buffer);
		// Renderer::SharedInstance()->RenderMesh(chunk->mesh);
	}
	// Renderer::SharedInstance()->DrawCube(id, 16, 16, 16);

	if (top_chunk) {
		// Renderer::SharedInstance()->DrawCube(id, 16, 16, 16);
		
		Renderer::SharedInstance()->DrawCube(glm::ivec3(id.x + 1, id.y + 1, id.z + 1), 14, 14, 14);
	}
}
