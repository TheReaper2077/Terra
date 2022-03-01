#pragma once

#include "define.h"
#include "Chunk.h"

struct ChunkColumn {
	std::unordered_map<unsigned int, std::shared_ptr<Chunk>> chunk_store;

	bool top_found = false;
	glm::ivec3 id;

	ChunkColumn(glm::ivec2 id) {
		this->id.x = id.x;
		this->id.y = id.y;
		this->id.z = 0;
	}

	Chunk *GetChunk(unsigned int z) {
		z &= ~15;

		if (chunk_store.find(z) == chunk_store.end())
			chunk_store[z] = std::make_shared<Chunk>(glm::ivec3(id.x, id.y, z));
		
		return chunk_store[z].get();
	}

	Chunk *GetChunkAvailable(unsigned int z) {
		z &= ~15;

		if (chunk_store.find(z) != chunk_store.end())
			return chunk_store[z].get();

		return nullptr;
	}

	bool Render() {
		for (auto& chunk: chunk_store) {
			RenderChunk(chunk.second.get());
			if (chunk.second->top_chunk) {
				top_found = true;
			}
		}

		if (!top_found) {
			id.z++;
		}

		return top_found;
	}
};