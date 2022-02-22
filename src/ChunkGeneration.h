#pragma once

#include "define.h"

class ChunkGeneration {
	std::deque<glm::ivec3> generate_request;
	std::deque<glm::ivec3> unload_request;

public:
	ChunkGeneration() {}

	void Update(double &dt) {

	}

	void AddGenerateQueue(glm::ivec3 id) {
		// generate_request.insert(id);
	}

	void AddUnloadQueue(glm::ivec3 id) {
		
	}
};