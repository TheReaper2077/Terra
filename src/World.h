#pragma once

#include "ECS/Registry.h"
#include "Components/Components.h"
#include "render/Renderer.h"
#include "TileRegistry.h"


enum BlockType {
	TRANSPARENT,
	SOLID,
	LIQUID
};

struct Block {
	Faces faces;
	BlockType type;
	BlockID block_id;

	Block() {
		type = TRANSPARENT;
		faces.reset();
	}

	Block(BlockType type, BlockID block_id) {
		faces.set();
		this->block_id = block_id;
		this->type = type;
	}

	Block(BlockType type) {
		faces.reset();
		this->type = type;
	}
};

class Chunk {
	Renderer* renderer = Renderer::SharedInstance();
	TileRegistry *tile_registry = TileRegistry::SharedInstance();

	std::vector<Vertex2D> mesh;

public:
	Block chunk_blocks[16][16][16];
	glm::ivec3 id;

	bool init;
	bool meshed = false;
	bool inner_mesh = false;
	bool outer_mesh = false;
	bool top_chunk = false;

	Chunk(glm::ivec3 &chunk_id) {
		id = chunk_id;
		init = false;

		// Generate();
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
						chunk_blocks[z][idx][x] = Block{SOLID, tile_registry->GetBlockID("GRASS_BLOCK")};
					} else if (y > value - 4) {
						chunk_blocks[z][idx][x] = Block{SOLID, tile_registry->GetBlockID("DIRT_BLOCK")};
					} else {
						chunk_blocks[z][idx][x] = Block{SOLID, tile_registry->GetBlockID("STONE_BLOCK")};
					}
					idx++;
				}
			}
		}

		init = true;
	}

	void Add(const glm::ivec3 &pos, Block block) {
		chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15] = block;
		meshed = outer_mesh = inner_mesh = false;
	}

	Block &Get(const glm::ivec3 &pos) {
		return chunk_blocks[pos.z & 15][pos.y & 15][pos.x & 15];
	}

	Block &Get(const int &x, const int &y, const int &z) {
		return chunk_blocks[z & 15][y & 15][x & 15];
	}
	int quads = 0;

	void Render() {
		if (!outer_mesh) return;
		if (meshed) return;
		if (!inner_mesh) MeshChunk();

		mesh.clear();

		for (int z = 0; z != 16; z++) {
			for (int y = 0; y != 16; y++) {
				for (int x = 0; x != 16; x++) {
					const auto& block = chunk_blocks[z][y][x];
					if (block.type == TRANSPARENT) continue;
					quads += block.faces.count();

					// renderer->TextureCube(id.x + x, id.y + y, id.z + z, 1, 1, 1, tile_registry->GetTile(block.tile_id), block.faces, mesh);
					renderer->TextureCube(id.x + x, id.y + y, id.z + z, 1, 1, 1, block.block_id, block.faces, mesh);
				}
			}
		}

		std::printf("%i, %i, %i\n", quads, mesh.size(), std::this_thread::get_id());
		meshed = true;
	}

	void RenderMesh() {
		renderer->RenderMesh(mesh);
	}

	void MeshChunk() {
		for (int z = 0; z != 16; z++) {
			for (int y = 0; y != 16; y++) {
				for (int x = 0; x != 16; x++) {
					auto& block = chunk_blocks[z][y][x];
					if (block.type == TRANSPARENT) continue;

					if (z - 1 >= 0) block.faces[0] = (chunk_blocks[z - 1][y][x].type == TRANSPARENT);
					if (z + 1 < 16) block.faces[1] = (chunk_blocks[z + 1][y][x].type == TRANSPARENT);

					if (y - 1 >= 0) block.faces[3] = (chunk_blocks[z][y - 1][x].type == TRANSPARENT);
					if (y + 1 < 16) block.faces[2] = (chunk_blocks[z][y + 1][x].type == TRANSPARENT);

					if (x - 1 >= 0) block.faces[4] = (chunk_blocks[z][y][x - 1].type == TRANSPARENT);
					if (x + 1 < 16) block.faces[5] = (chunk_blocks[z][y][x + 1].type == TRANSPARENT);
				}
			}
		}

		inner_mesh = true;
	}
};

class World {
	unsigned int next_chunk_index = 0;
	std::vector<std::thread> threads;
	std::vector<Chunk> world_chunks;
	std::unordered_map<glm::ivec3, unsigned int> chunk_id_map;
	Renderer *renderer = Renderer::SharedInstance();

public:
	Chunk *GetChunk(const glm::ivec3 &pos) {
		glm::ivec3 chunk_id = (pos & ~15);
		if (chunk_id_map.find(chunk_id) == chunk_id_map.end()) {
			std::printf("[CHUNK] (%i, %i, %i)\n", chunk_id.x, chunk_id.y, chunk_id.z);
			chunk_id_map[chunk_id] = next_chunk_index++;
			world_chunks.push_back(Chunk(chunk_id));
			return &world_chunks.back();
		}
		return &world_chunks[chunk_id_map[chunk_id]];
	}

	void AddBlock(Block block, glm::ivec3 pos) {
		GetChunk(pos)->Add(pos, block);
	}

	bool IsChunkPresent(const int &x, const int &y, const int &z) {
		return (chunk_id_map.find(glm::ivec3(x & ~15, y & ~15, z & ~15)) != chunk_id_map.end());
	}

	bool IsChunkPresent(const glm::ivec3 &pos) {
		return (chunk_id_map.find(pos) != chunk_id_map.end());
	}

	void Generate() {
		for (int z = -6; z != 6; z++) {
			for (int y = 0; y != 4; y++) {
				for (int x = -6; x != 6; x++) {
					GetChunk(glm::ivec3(x*16, y*16, z*16))->Generate();
				}
			}
		}
	}

	void Meshing(Chunk *chunk) {
		if (chunk->outer_mesh) return;

		for (int z = 0; z != 16; z++) {
			for (int y = 0; y != 16; y++) {
				for (int x = 0; x != 16; x++) {
					if ((z - 1 < 0 || z + 1 >= 16 || y - 1 < 0 || y + 1 >= 16 || x - 1 < 0 || x + 1 >= 16)) {
						auto& block = chunk->chunk_blocks[z][y][x];

						if (block.type != TRANSPARENT) {
							if (z - 1 < 0) {
								auto id = glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z - 16);
								if (IsChunkPresent(id)) {
									block.faces[0] = (world_chunks[chunk_id_map[id]].chunk_blocks[15][y][x].type == TRANSPARENT);
								} else {
									block.faces[0] = true;
								}
							}
							if (z + 1 >= 16) {
								auto id = glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z + 16);
								if (IsChunkPresent(id)) {
									block.faces[1] = (world_chunks[chunk_id_map[id]].chunk_blocks[0][y][x].type == TRANSPARENT);
								} else {
									block.faces[1] = true;
								}
							}

							if (y - 1 < 0) {
								auto id = glm::ivec3(chunk->id.x, chunk->id.y - 16, chunk->id.z);
								if (IsChunkPresent(id)) {
									block.faces[3] = (world_chunks[chunk_id_map[id]].chunk_blocks[z][15][x].type == TRANSPARENT);
								} else {
									block.faces[3] = true;
								}
							}
							if (y + 1 >= 16) {
								auto id = glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z);
								if (IsChunkPresent(id)) {
									block.faces[2] = (world_chunks[chunk_id_map[id]].chunk_blocks[z][0][x].type == TRANSPARENT);
								} else {
									block.faces[2] = true;
								}
							}

							if (x - 1 < 0) {
								auto id = glm::ivec3(chunk->id.x - 16, chunk->id.y, chunk->id.z);
								if (IsChunkPresent(id)) {
									block.faces[4] = (world_chunks[chunk_id_map[id]].chunk_blocks[z][y][15].type == TRANSPARENT);
								} else {
									block.faces[4] = true;
								}
							}
							if (x + 1 >= 16) {
								auto id = glm::ivec3(chunk->id.x + 16, chunk->id.y, chunk->id.z);
								if (IsChunkPresent(id)) {
									block.faces[5] = (world_chunks[chunk_id_map[id]].chunk_blocks[z][y][0].type == TRANSPARENT);
								} else {
									block.faces[5] = true;
								}
							}
						}
					}
				}
			}
		}
		chunk->outer_mesh = true;
	}

	void Render() {
		for (auto& chunk: world_chunks) {
			if (chunk.outer_mesh) continue;
			threads.push_back(std::thread(&World::Meshing, this, &chunk));
		}

		for (std::thread& thread: threads) {
			if (thread.joinable()) thread.join();
		}

		if (threads.size()) threads.clear();

		for (auto& chunk: world_chunks) {
			if (chunk.meshed) continue;
			
			threads.push_back(std::thread(&Chunk::Render, std::ref(chunk)));
		}

		for (std::thread& thread: threads) {
			if (thread.joinable()) thread.join();
		}

		for (auto& chunk: world_chunks) {
			chunk.RenderMesh();
		}

		if (threads.size()) threads.clear();
	}
};
