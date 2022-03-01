#pragma once

#include "../define.h"
#include "Chunk.h"

class ChunkColumn {
	std::unordered_map<unsigned int, std::shared_ptr<Chunk>> chunk_store;

public:
	bool top_found = false;
	glm::ivec3 id;

	ChunkColumn(glm::ivec2 id) {
		this->id.x = id.x;
		this->id.y = 0;
		this->id.z = id.y;
	}

	Chunk *GetChunk(unsigned int y) {
		y &= ~15;

		if (chunk_store.find(y) == chunk_store.end())
			chunk_store[y] = std::make_shared<Chunk>(glm::ivec3(id.x, y, id.z));
		
		return chunk_store[y].get();
	}

	Chunk *GetChunkAvailable(unsigned int y) {
		y &= ~15;

		if (chunk_store.find(y) != chunk_store.end())
			return chunk_store[y].get();

		return nullptr;
	}

	void Render() {
		for (auto& chunk: chunk_store) {
			if (!chunk.second->meshed && chunk.second->visible) continue;
			RenderChunk(chunk.second.get());

			// if (chunk.second.get()->top_chunk && !top_found) {
			// 	top_found = true;
			// 	std::cout << "found\n";
			// }
		}
	}
};