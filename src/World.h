#pragma once

#include "ECS/Registry.h"
#include "Components/Components.h"
#include "render/Renderer.h"
#include "TileRegistry.h"

// struct Block {
// 	Faces faces;
// 	BlockType type;
// 	BlockID block_id;

// 	Block() {
// 		type = TRANSPARENT;
// 		faces.reset();
// 	}

// 	Block(BlockType type, BlockID block_id) {
// 		faces.set();
// 		this->block_id = block_id;
// 		this->type = type;
// 	}

// 	Block(BlockType type) {
// 		faces.reset();
// 		this->type = type;
// 	}
// };

class Chunk {
	Renderer* renderer = Renderer::SharedInstance();
	TileRegistry *tile_registry = TileRegistry::SharedInstance();

	std::vector<Vertex2D> mesh;

public:
	BlockID chunk_blocks[16][16][16] = {AIR_BLOCK};
	glm::ivec3 id;

	bool init;
	bool meshed = false;
	bool inner_mesh = false;
	bool outer_mesh = false;
	bool top_chunk = false;

	Chunk(glm::ivec3 &chunk_id) {
		id = chunk_id;
		init = false;
	}

	void Generate() {
		for (int z = 0; z != 16; z++) {
			for (int x = 0; x != 16; x++) {
				float value = glm::simplex(glm::vec2{(id.x + x) / 64.0, (id.z + z) / 64.0});
				value = (value + 1) / 2;
				value *= 32 + 32;

				int idx = 0;
				
				for (int y = id.y; y <= value; y++) {
					if (idx > 15) break;
					if (y > value - 1) {
						chunk_blocks[z][idx][x] = GRASS_BLOCK;
					} else if (y > value - 4) {
						chunk_blocks[z][idx][x] = DIRT_BLOCK;
					} else {
						chunk_blocks[z][idx][x] = STONE_BLOCK;
					}
					idx++;
				}
			}
		}

		init = true;
	}

	void Add(const glm::ivec3 &pos, BlockID block) {
		chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15] = block;
		meshed = outer_mesh = inner_mesh = false;
	}

	BlockID &Get(const glm::ivec3 &pos) {
		return chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15];
	}

	BlockID &Get(const int &x, const int &y, const int &z) {
		return chunk_blocks[z & 15][y & 15][x & 15];
	}

	void RenderMesh() {
		renderer->RenderMesh(mesh);
	}

	void DrawQuad(float x, float y, float z, float w, float h, float d, const TileID &tile_id, int dir) {
		auto& tile = tile_registry->GetTile(tile_id);
		float index = renderer->GetTextureIndex(&tile.texture);

		mesh.reserve(mesh.size() + 4);
		
		if (dir == 0) {
			mesh.emplace_back(Vertex2D{x, y, z, tile.tx + tile.tw, tile.ty + tile.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z, tile.tx + tile.tw, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, tile.tx, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, tile.tx, tile.ty + tile.th, index});
		}
		if (dir == 1) {
			mesh.emplace_back(Vertex2D{x, y, z + d, tile.tx + tile.tw, tile.ty + tile.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, tile.tx, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z + d, tile.tx, tile.ty + tile.th, index});
		}
		if (dir == 3) {
			mesh.emplace_back(Vertex2D{x, y, z, tile.tx + tile.tw, tile.ty + tile.th, index});
			mesh.emplace_back(Vertex2D{x, y, z + d, tile.tx + tile.tw, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z + d, tile.tx, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, tile.tx, tile.ty + tile.th, index});
		}
		if (dir == 2) {
			mesh.emplace_back(Vertex2D{x, y + h, z, tile.tx + tile.tw, tile.ty + tile.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, tile.tx, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, tile.tx, tile.ty + tile.th, index});
		}
		if (dir == 4) {
			mesh.emplace_back(Vertex2D{x, y, z + d, tile.tx + tile.tw, tile.ty + tile.th, index});
			mesh.emplace_back(Vertex2D{x, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
			mesh.emplace_back(Vertex2D{x, y + h, z, tile.tx, tile.ty, index});
			mesh.emplace_back(Vertex2D{x, y, z, tile.tx, tile.ty + tile.th, index});
		}
		if (dir == 5) {
			mesh.emplace_back(Vertex2D{x + w, y, z + d, tile.tx + tile.tw, tile.ty + tile.th, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z + d, tile.tx + tile.tw, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y + h, z, tile.tx, tile.ty, index});
			mesh.emplace_back(Vertex2D{x + w, y, z, tile.tx, tile.ty + tile.th, index});
		}
	}

	void MeshChunk(Chunk *front, Chunk *back, Chunk *top, Chunk *bottom, Chunk *left, Chunk *right) {
		mesh.clear();
		meshed = true;
		
		for (int z = 0; z != 16; z++) {
			for (int y = 0; y != 16; y++) {
				for (int x = 0; x != 16; x++) {
					auto& block = chunk_blocks[z][y][x];
					if (block == AIR_BLOCK) continue;

					auto& tiles = tile_registry->GetBlock(block);

					if (z - 1 >= 0) {
						if (chunk_blocks[z - 1][y][x] == AIR_BLOCK) {
							DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.front, 0);
						}
					} else {
						if (front != nullptr) {
							if (front->chunk_blocks[15][y][x] == AIR_BLOCK) {
								DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.front, 0);
							}
						}
					}
					if (z + 1 < 16) {
						if (chunk_blocks[z + 1][y][x] == AIR_BLOCK) {
							DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.back, 1);
						}
					} else {
						if (back != nullptr) {
							if (back->chunk_blocks[0][y][x] == AIR_BLOCK) {
								DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.back, 1);
							}
						}
					}

					if (y - 1 >= 0) {
						if (chunk_blocks[z][y - 1][x] == AIR_BLOCK) {
							DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.bottom, 3);
						}
					} else {
						if (bottom != nullptr) {
							if (bottom->chunk_blocks[z][15][x] == AIR_BLOCK) {
								DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.bottom, 3);
							}
						}
					}
					if (y + 1 < 16) {
						if (chunk_blocks[z][y + 1][x] == AIR_BLOCK) {
							DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.top, 2);
						}
					} else {
						if (top != nullptr) {
							if (top->chunk_blocks[z][0][x] == AIR_BLOCK) {
								DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.top, 2);
							}
						}
					}

					if (x - 1 >= 0) {
						if (chunk_blocks[z][y][x - 1] == AIR_BLOCK) {
							DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.left, 4);
						}
					} else {
						if (left != nullptr) {
							if (left->chunk_blocks[z][y][15] == AIR_BLOCK) {
								DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.left, 4);
							}
						}
					}
					if (x + 1 < 16) {
						if (chunk_blocks[z][y][x + 1] == AIR_BLOCK) {
							DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.right, 5);
						}
					} else {
						if (right != nullptr) {
							if (right->chunk_blocks[z][y][0] == AIR_BLOCK) {
								DrawQuad(id.x + x, id.y + y, id.z + z, 1, 1, 1, tiles.left, 5);
							}
						}
					}
				}
			}
		}
	}
};

class World {
	unsigned int next_chunk_index = 0;
	std::vector<std::thread> threads;
	std::vector<Chunk> world_chunks;
	std::vector<Chunk*> render_chunks;
	std::unordered_map<glm::ivec3, unsigned int> chunk_id_map;
	Renderer *renderer = Renderer::SharedInstance();

public:
	Chunk &GetChunk(const glm::ivec3 &pos) {
		glm::ivec3 chunk_id = (pos & ~15);
		if (chunk_id_map.find(chunk_id) == chunk_id_map.end()) {
			std::printf("[CHUNK] (%i, %i, %i)\n", chunk_id.x, chunk_id.y, chunk_id.z);
			chunk_id_map[chunk_id] = next_chunk_index++;
			world_chunks.push_back(Chunk(chunk_id));
			return world_chunks.back();
		}
		return world_chunks[chunk_id_map[chunk_id]];
	}

	void AddBlock(BlockID block, glm::ivec3 pos) {
		GetChunk(pos).Add(pos, block);
	}

	bool IsChunkPresent(const int &x, const int &y, const int &z) {
		return (chunk_id_map.find(glm::ivec3(x & ~15, y & ~15, z & ~15)) != chunk_id_map.end());
	}

	bool IsChunkPresent(const glm::ivec3 &pos) {
		return (chunk_id_map.find(pos & ~15) != chunk_id_map.end());
	}

	void Generate() {
		for (int z = -4; z != 4; z++) {
			for (int y = 0; y != 4; y++) {
				for (int x = -4; x != 4; x++) {
					GetChunk(glm::ivec3(x*16, y*16, z*16)).Generate();
				}
			}
		}
	}

	Chunk *GetChunkPtr(const glm::ivec3 &id) {
		if (IsChunkPresent(id)) {
			return &GetChunk(id);
		}
		return nullptr;
	}

	void Render(const glm::ivec3 &camera_pos) {
		for (int z = 0; z != 4; z++) {
			for (int x = 0; x != 4; x++) {
				for (int y = 0; y != 2; y++) {
					glm::ivec3 pos = glm::ivec3(camera_pos.x + x*16, y*16, camera_pos.z + z*16);
					if (!IsChunkPresent(pos)) {
						threads.push_back(std::thread(&Chunk::Generate, std::ref(GetChunk(pos))));

						glm::ivec3 id;

						id = glm::ivec3(pos.x - 16, pos.y, pos.z);
						if (IsChunkPresent(id)) {
							GetChunk(id).meshed = false;
						}
						id = glm::ivec3(pos.x + 16, pos.y, pos.z);
						if (IsChunkPresent(id)) {
							GetChunk(id).meshed = false;
						}
						id = glm::ivec3(pos.x, pos.y, pos.z - 16);
						if (IsChunkPresent(id)) {
							GetChunk(id).meshed = false;
						}
						id = glm::ivec3(pos.x, pos.y, pos.z + 16);
						if (IsChunkPresent(id)) {
							GetChunk(id).meshed = false;
						}
					}
				}
			}
		}

		for (std::thread& thread: threads) {
			if (thread.joinable()) thread.join();
		}

		if (threads.size()) threads.clear();

		for (auto& chunk: world_chunks) {
			if (chunk.meshed) continue;

			auto* front = GetChunkPtr(glm::ivec3(chunk.id.x, chunk.id.y, chunk.id.z - 16));
			auto* back = GetChunkPtr(glm::ivec3(chunk.id.x, chunk.id.y, chunk.id.z + 16));
			auto* top = GetChunkPtr(glm::ivec3(chunk.id.x, chunk.id.y + 16, chunk.id.z));
			auto* bottom = GetChunkPtr(glm::ivec3(chunk.id.x, chunk.id.y - 16, chunk.id.z));
			auto* left = GetChunkPtr(glm::ivec3(chunk.id.x - 16, chunk.id.y, chunk.id.z));
			auto* right = GetChunkPtr(glm::ivec3(chunk.id.x + 16, chunk.id.y, chunk.id.z));
			
			threads.push_back(std::thread(&Chunk::MeshChunk, std::ref(chunk), front, back, top, bottom, left, right));
		}

		for (std::thread& thread: threads) {
			if (thread.joinable()) thread.join();
		}

		if (threads.size()) threads.clear();

		for (auto& chunk: world_chunks) {
			chunk.RenderMesh();
		}
	}
};
