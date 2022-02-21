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
	bool visible = false;

	Chunk(glm::ivec3 &chunk_id) {
		id = chunk_id;
		init = false;
	}

	void Generate() {
		int t = 0;
		float top_value = 0;

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
						t++;
						break;
					}

					if (y > value - 1) {
						chunk_blocks[z][idx][x] = GRASS_BLOCK;
						visible = true;
					} else if (y > value - 4) {
						chunk_blocks[z][idx][x] = DIRT_BLOCK;
						visible = true;
					} else {
						chunk_blocks[z][idx][x] = STONE_BLOCK;
						visible = true;
					}
					idx++;
				}
			}
		}

		top_chunk = (top_value - id.y < 16);

		init = true;
		if (visible) visible = (t < 256);
		// if (visible == false) {
		// 	std::cout << "jsakhfsda\n";
		// }
		// for (int z = 0; z != 10; z++) {
		// 	for (int x = 0; x != 10; x++) {
		// 		for (int y = 0; y != 10; y++) {
		// 			chunk_blocks[z][y][x] = GRASS_BLOCK;
		// 		}
		// 	}
		// }
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
		if (visible && mesh.size()) {
			renderer->RenderMesh(mesh);
		}
		// else
		// renderer->DrawCube(glm::ivec3(id.x + 1, id.y + 1, id.z + 1), 14, 14, 14);
		// renderer->DrawCube(id, 16, 16, 16);
		if (top_chunk) {
			renderer->DrawCube(id, 16, 16, 16);
		}
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
								visible = true;
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
								visible = true;
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
								visible = true;
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
								visible = true;
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
								visible = true;
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
								visible = true;
							}
						}
					}
				}
			}
		}
	}
};