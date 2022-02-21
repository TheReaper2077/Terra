#pragma once

#include "ECS/Registry.h"
#include "Components/Components.h"
#include "render/Renderer.h"
#include "TileRegistry.h"
#include "Chunk.h"


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

	void GenerateChunk(const glm::ivec3 &pos) {
		// for (int z = -4; z != 4; z++) {
		// 	for (int y = 0; y != 4; y++) {
		// 		for (int x = -4; x != 4; x++) {
		// 			GetChunk(glm::ivec3(x*16, y*16, z*16)).Generate();
		// 		}
		// 	}
		// }
		for (int z = pos.z; z != pos.z + 16; z++) {
			for (int x = pos.x; x != pos.x + 16; x++) {
				float value = glm::simplex(glm::vec2{x / 64.0, z / 64.0});
				value = (value + 1) / 2;
				value *= 32 + 32;
				for (int y = 0; y <= value; y++) {
					if (y > value - 1) {
						AddBlock(GRASS_BLOCK, glm::ivec3(x, y, z));
					} else if (y > value - 4) {
						AddBlock(DIRT_BLOCK, glm::ivec3(x, y, z));
					} else {
						AddBlock(STONE_BLOCK, glm::ivec3(x, y, z));
					}
				}
			}
		}

		generated = true;
	}

	Chunk *GetChunkPtr(const glm::ivec3 &id) {
		if (IsChunkPresent(id)) {
			return &GetChunk(id);
		}
		return nullptr;
	}

	bool generated = false;
	std::vector<glm::ivec3> chunk_render_id;


	double total_time = 0;
	int n = 0, iter = 0, s = 0, r = 0;

	bool Generate(std::vector<glm::ivec3> &positions) {
		for (const auto& pos: positions) {
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
				id = glm::ivec3(pos.x, pos.y - 16, pos.z);
				if (IsChunkPresent(id)) {
					GetChunk(id).meshed = false;
				}
				id = glm::ivec3(pos.x, pos.y + 16, pos.z);
				if (IsChunkPresent(id)) {
					GetChunk(id).meshed = false;
				}

				generated = true;
			}
		}

		bool ret = false;

		for (std::thread& thread: threads) {
			if (thread.joinable()) thread.join();
		}

		// for (auto& pos: positions) {
		// 	if (!GetChunk(pos).top_chunk) {
		// 		ret = true;
		// 		pos.y += 16;

		// 		glm::ivec3 id;
		// 		id = glm::ivec3(pos.x - 16, pos.y, pos.z);
		// 		if (!IsChunkPresent(id)) {
		// 			positions.push_back(id);
		// 		}
		// 		id = glm::ivec3(pos.x + 16, pos.y, pos.z);
		// 		if (!IsChunkPresent(id)) {
		// 			positions.push_back(id);
		// 		}
		// 		id = glm::ivec3(pos.x, pos.y, pos.z - 16);
		// 		if (!IsChunkPresent(id)) {
		// 			positions.push_back(id);
		// 		}
		// 		id = glm::ivec3(pos.x, pos.y, pos.z + 16);
		// 		if (!IsChunkPresent(id)) {
		// 			positions.push_back(id);
		// 		}
		// 	}
		// }

		if (threads.size()) threads.clear();

		return ret;
	}

	void Mesh() {
		if (generated) {
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
			generated = false;
		}
	}

	double last_gen_time = 0, render_time = 0;
	std::vector<glm::ivec3> gen_id;
	glm::ivec3 last_camera_pos;

	void Update(double &dt, const glm::ivec3 &camera_pos) {
		auto pos = glm::ivec3(camera_pos.x & ~15, 0, camera_pos.z & ~15);
		if (pos.x != last_camera_pos.x && pos.z != last_camera_pos.z) {
			n = 0;
			r = 0;
			s = 0;
			iter = 0;
			last_camera_pos = pos;
			last_gen_time = 0;
		}
		if (total_time > last_gen_time) {
		// if (true) {
			while (!generated) {
				if (iter == 2*n + 1) {
					iter = 0;
					r = 0;
					s = n;
					n++;
				}
				iter++;

				if (iter - 1 <= n) r = iter - 1;
				if (iter - 1 > n) s--;
				
				gen_id.push_back(glm::ivec3(r*16 + pos.x, 0, s*16 + pos.z));
				gen_id.push_back(glm::ivec3(-s*16 + pos.x, 0, r*16 + pos.z));
				gen_id.push_back(glm::ivec3(s*16 + pos.x, 0, -r*16 + pos.z));
				gen_id.push_back(glm::ivec3(-r*16 + pos.x, 0, -s*16 + pos.z));

				while(Generate(gen_id)) {
				}
				gen_id.clear();
			}
			Mesh();


			last_gen_time = total_time;
			total_time = 0;
		}
		total_time += dt;
		render_time += dt;
	}

	void Render(const glm::ivec3 &camera_pos) {
		if (generated) Mesh();

		for (int z = -12; z != 12; z++) {
			for (int x = -12; x != 12; x++) {
				int y = 0;
				while (true) {
					auto pos = glm::ivec3((camera_pos.x & ~15) - x*16, y*16, (camera_pos.z & ~15) - z*16);
					if (IsChunkPresent(pos)) {
						GetChunk(pos).RenderMesh();
						if (GetChunk(pos).top_chunk) break;
					}
					else break;
					y++;
				}
			}
		}

		chunk_render_id.clear();
	}
};
