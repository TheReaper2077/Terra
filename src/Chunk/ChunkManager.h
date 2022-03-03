#pragma once

#include "../define.h"
#include "../TileRegistry.h"
#include "ChunkColumn.h"
#include "Chunk.h"

class ChunkManager {
	std::unordered_map<glm::ivec2, std::shared_ptr<ChunkColumn>> chunk_column_map;

	glm::ivec3 last_camera_chunk_pos;

	std::vector<glm::ivec3> generate_request;
	std::vector<std::thread> generate_threads;

	std::vector<Chunk*> mesh_request;
	std::vector<std::thread> mesh_threads;

public:
	ChunkManager() {}

	ChunkColumn *GetChunkColumn(const glm::ivec2 pos) {
		auto id = (pos & ~15);

		if(chunk_column_map.find(id) == chunk_column_map.end())
			chunk_column_map[id] = std::make_shared<ChunkColumn>(id);

		return chunk_column_map[id].get();
	}

	ChunkColumn *GetChunkColumnAvailable(const glm::ivec2 pos) {
		auto id = (pos & ~15);

		if(chunk_column_map.find(id) != chunk_column_map.end())
			return chunk_column_map[id].get();

		return nullptr;
	}

	Chunk *GetChunk(const glm::ivec3 pos) {
		auto id = (pos & ~15);

		return GetChunkColumn(glm::ivec2(id.x, id.z))->GetChunk(id.y);
	}

	Chunk *GetChunkAvailable(const glm::ivec3 pos) {
		auto id = (pos & ~15);

		auto* column = GetChunkColumn(glm::ivec2(id.x, id.z));
		if (column != nullptr) 
			return column->GetChunkAvailable(id.y);
		return nullptr;
	}

	bool BlockSolid(glm::ivec3 pos) {
		auto* chunk = GetChunkAvailable(pos);

		if (chunk != nullptr)
			return (chunk->Get(pos) != AIR_BLOCK);
		return false;
	}

	void ChangeBlock(glm::ivec3 pos, BlockID id) {
		auto* chunk = GetChunkAvailable(pos);

		bool front_key, back_key, top_key, bottom_key, left_key, right_key;

		if (id != AIR_BLOCK) {
			pos.y++;

			chunk = GetChunk(pos);
		}
		
		// pos &= 15;

		if (chunk != nullptr) {
			chunk->Add(pos, id, front_key, back_key, top_key, bottom_key, left_key, right_key);

			if (!chunk->meshed) {
				mesh_request.push_back(chunk);

				if (front_key) {
					auto* front = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z - 16));
					if (front != nullptr) {
						front->meshed = false;
						mesh_request.push_back(front);
					}
				}
				if (back_key) {
					auto* back = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y, chunk->id.z + 16));
					if (back != nullptr) {
						back->meshed = false;
						mesh_request.push_back(back);
					}
				}
				if (top_key) {
					auto* top = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y + 16, chunk->id.z));
					if (top != nullptr) {
						top->meshed = false;
						mesh_request.push_back(top);
					}
				}
				if (bottom_key) {
					auto* bottom = GetChunkAvailable(glm::ivec3(chunk->id.x, chunk->id.y - 16, chunk->id.z));
					if (bottom != nullptr) {
						bottom->meshed = false;
						mesh_request.push_back(bottom);
					}
				}
				if (left_key) {
					auto* left = GetChunkAvailable(glm::ivec3(chunk->id.x - 16, chunk->id.y, chunk->id.z));
					if (left != nullptr) {
						left->meshed = false;
						mesh_request.push_back(left);
					}
				}
				if (right_key) {
					auto* right = GetChunkAvailable(glm::ivec3(chunk->id.x + 16, chunk->id.y, chunk->id.z));
					if (right != nullptr) {
						right->meshed = false;
						mesh_request.push_back(right);
					}
				}
			}
		}
	}

	void Generate() {
		for (int i = 0; i != 8; i++) {
			if (!generate_request.size()) break;

			auto id = generate_request[generate_request.size() - i - 1];

			auto* chunk = GetChunk(id);

			if (chunk->generated) continue;
			
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

			mesh_threads.push_back(std::thread(&MeshChunk, chunk, front, back, top, bottom, left, right));
			
			chunk->buffer.bind();
		}

		for (auto& thread: mesh_threads) {
			if (thread.joinable()) thread.join();
		}

		for (auto& chunk: mesh_request) {
			if (chunk->opaque_mesh.size() || chunk->transparent_mesh.size()) {
				if (!chunk->buffer.init) chunk->buffer.Init();

				// if (chunk->total_opaque_quads) chunk->buffer.Add(chunk->opaque_mesh);
				// if (chunk->total_transparent_quads > 0) chunk->buffer.Add(chunk->transparent_mesh);
				if (chunk->total_opaque_quads > 0 && chunk->total_transparent_quads > 0) {
					std::size_t element_size = chunk->opaque_mesh.size() + chunk->transparent_mesh.size();
					chunk->buffer.Allocate(element_size, sizeof(chunk->opaque_mesh[0]));
					chunk->buffer.Add(chunk->opaque_mesh, 0);
					chunk->buffer.Add(chunk->transparent_mesh, chunk->opaque_mesh.size()*sizeof(chunk->opaque_mesh[0]));
				} else {
					if (chunk->total_opaque_quads > 0) {
						chunk->buffer.Add(chunk->opaque_mesh);
					}
					if (chunk->total_transparent_quads > 0) {
						chunk->buffer.Add(chunk->transparent_mesh);
					}
				}

				chunk->opaque_mesh.clear();
				chunk->transparent_mesh.clear();
			}
		}

		mesh_threads.clear();
		mesh_request.clear();
	}

	void Render(const glm::ivec3 &camera) {
		auto pos = (camera & ~15);

		if (pos.x != last_camera_chunk_pos.x && pos.y != last_camera_chunk_pos.y) {
			last_camera_chunk_pos = pos;
		}

		for (int z = -16;  z != 16; z++) {
			for (int x = -16;  x != 16; x++) {
				bool top = false;

				auto* column = GetChunkColumn(glm::ivec2(pos.x + x*16, pos.z + z*16));

				if (!column->top_found) {
					for (int y = 0; true; y++) {
						glm::ivec3 n_id = glm::ivec3(pos.x + x*16, y*16, pos.z + z*16);
						auto* chunk = GetChunkAvailable(n_id);

						if (chunk == nullptr) {
							generate_request.push_back(n_id);
							break;
						} else if (!chunk->generated) {
							generate_request.push_back(n_id);
							break;
						}

						if (top) {
							column->top_found = true;
							break;
						}
						if (!top) top = chunk->top_chunk;
						if (!chunk->meshed) continue;
						// RenderChunk(chunk);
					}
				}

				// glm::ivec3 id = glm::ivec3(x*16, 0, z*16);
				// 	if (!column->top_found) {
				// 		column->id.y += 1;
				// 		generate_request.push_back(glm::ivec3(column->id.x, column->id.y*16, column->id.z));
				// 	}
				column->RenderOpaque();
				// } else {
				// 	generate_request.push_back(id);
			}
		}

		for (int z = -16;  z != 16; z++) {
			for (int x = -16;  x != 16; x++) {
				auto* column = GetChunkColumn(glm::ivec2(pos.x + x*16, pos.z + z*16));
				column->RenderTransparent();
			}
		}
	}

public:
	void AddBlock(const glm::ivec3 &pos, const BlockID &block) {
		GetChunk(pos)->Add(pos, block);
	}

	BlockID &GetBlock(const glm::ivec3 &pos) {
		return GetChunk(pos)->Get(pos);
	}
};
