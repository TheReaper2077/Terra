#pragma once

#include "define.h"
#include "TileRegistry.h"
#include "Chunk.h"

class ChunkManager {
	std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> chunk_ptr_map;

	bool camera_moved = false;
	glm::ivec3 last_camera_chunk_pos;

	std::vector<glm::ivec3> generate_request;
	std::vector<std::thread> generate_threads;

	std::vector<Chunk*> mesh_request;
	std::vector<std::thread> mesh_threads;

public:
	ChunkManager() {}

	Chunk *GetChunk(const glm::ivec3 pos) {
		auto id = (pos & ~15);

		if(chunk_ptr_map.find(id) == chunk_ptr_map.end())
			chunk_ptr_map[id] = std::make_shared<Chunk>(id);

		return chunk_ptr_map[id].get();
	}

	Chunk *GetChunkAvailable(const glm::ivec3 pos) {
		auto id = (pos & ~15);

		if(chunk_ptr_map.find(id) != chunk_ptr_map.end())
			return chunk_ptr_map[id].get();

		return nullptr;
	}

	bool ChunkPresent(const glm::ivec3 &pos) {
		return (chunk_ptr_map.find(pos & ~15) == chunk_ptr_map.end());
	}

	void Generate() {
		for (int i = 0; i != 8; i++) {
			if (!generate_request.size()) break;

			auto id = generate_request[generate_request.size() - i - 1];
			// generate_request.pop_back();

			if (GetChunkAvailable(id) != nullptr) continue;

			auto* chunk = GetChunk(id);
			// GenerateChunk(chunk);
			mesh_request.push_back(chunk);

			generate_threads.push_back(std::thread(&GenerateChunk, chunk));

			auto* front = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z - 16));
			if (front != nullptr) {
				front->meshed = false;
				mesh_request.push_back(front);
			}

			auto* back = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z + 16));
			if (back != nullptr) {
				back->meshed = false;
				mesh_request.push_back(back);
			}

			auto* top = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z));
			if (top != nullptr) {
				top->meshed = false;
				mesh_request.push_back(top);
			}

			auto* bottom = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y - 16, chunk->id.z));
			if (bottom != nullptr) {
				bottom->meshed = false;
				mesh_request.push_back(bottom);
			}

			auto* left = GetChunkAvailable(glm::ivec3(chunk->id.x - 16, chunk->id.y, chunk->id.z));
			if (left != nullptr) {
				left->meshed = false;
				mesh_request.push_back(left);
			}

			auto* right = GetChunkAvailable(glm::ivec3(chunk->id.x + 16, chunk->id.y, chunk->id.z));
			if (right != nullptr) {
				right->meshed = false;
				mesh_request.push_back(right);
			}
		}

		for (auto& thread: generate_threads) {
			if (thread.joinable()) {
				thread.join();
			}
		}

		if (generate_threads.size()) generate_threads.clear();
		if (generate_request.size()) generate_request.clear();

		for (auto* chunk: mesh_request) {
			glm::ivec3 id;

			if (!chunk->top_chunk) {
				id = glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z);
				if (GetChunkAvailable(id) == nullptr) {
					generate_request.push_back(id);
				}

				// id = glm::ivec3(chunk->id.x, chunk->id.y + 16 + 16, chunk->id.z);
				// if (GetChunkAvailable(id) == nullptr) {
				// 	generate_request.push_back(id);
				// }
				
				// id = glm::ivec3(chunk->id.x + 16, chunk->id.y + 16, chunk->id.z);
				// if (GetChunkAvailable(id) == nullptr) {
				// 	generate_request.push_back(id);
				// }

				// id = glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z - 16);
				// if (GetChunkAvailable(id) == nullptr) {
				// 	generate_request.push_back(id);
				// }

				// id = glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z + 16);
				// if (GetChunkAvailable(id) == nullptr) {
				// 	generate_request.push_back(id);
				// }

				// id = glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z - 16);
				// if (GetChunkAvailable(id) == nullptr) {
				// 	generate_request.push_back(id);
				// }
			}
		}
	}

	void Mesh() {
		for (auto& chunk: mesh_request) {
			if (chunk->meshed) continue;

			auto* front = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z - 16));
			auto* back = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z + 16));
			auto* top = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z));
			auto* bottom = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y - 16, chunk->id.z));
			auto* left = GetChunkAvailable(glm::ivec3(chunk->id.x - 16, chunk->id.y, chunk->id.z));
			auto* right = GetChunkAvailable(glm::ivec3(chunk->id.x + 16, chunk->id.y, chunk->id.z));

			// std::cout << chunk->id.x << " " << chunk->id.y << " " << chunk->id.z << " \n";

			mesh_threads.push_back(std::thread(&MeshChunk, chunk, front, back, top, bottom, left, right));
			
			chunk->buffer.bind();
			// MeshChunk(chunk, front, back, top, bottom, left, right);
		}

		for (auto& thread: mesh_threads) {
			if (thread.joinable()) thread.join();
		}

		for (auto& chunk: mesh_request) {
			if (chunk->mesh.size()) {
				// Renderer::SharedInstance()->VAO->bind();
				if (!chunk->buffer.init) chunk->buffer.Init();
				chunk->buffer.Add(chunk->mesh);
				chunk->mesh.clear();
			}
		}

		mesh_threads.clear();
		mesh_request.clear();
	}

	void Render() {
		for (auto& chunk: chunk_ptr_map) {
			RenderChunk(chunk.second.get());
		}
	}

	void Render(const glm::ivec3 &camera) {
		auto pos = (camera & ~15);

		if (pos.x != last_camera_chunk_pos.x && pos.y != last_camera_chunk_pos.y) {
			last_camera_chunk_pos = pos;
			camera_moved = true;
		}

		for (int z = -32;  z != 32; z++) {
			for (int x = -32;  x != 32; x++) {
				bool top = false;

				for (int y = 0; true; y++) {
					glm::ivec3 id = glm::ivec3(pos.x + x*16, y*16, pos.z + z*16);
					auto* chunk = GetChunkAvailable(id);

					if (chunk == nullptr) {
						if (!top) {
							generate_request.push_back(id);
						}
						break;
					}

					if (!top) top = chunk->top_chunk;

					RenderChunk(chunk);
				}
			}
		}

		camera_moved = false;
	}

public:
	void AddBlock(const glm::ivec3 &pos, const BlockID &block) {
		Add(GetChunk(pos), pos, block);
	}

	BlockID &GetBlock(const glm::ivec3 &pos) {
		return Get(GetChunk(pos), pos);
	}
};
