#pragma once

#include "ECS/Registry.h"
#include "Components/Components.h"
#include "TileRegistry.h"

struct Chunk {
	TileID chunk_tiles[16][16] = {0};

	Chunk() {
		for (int y = 0; y != 16; y++) {
			for (int x = 0; x != 16; x++) {
				chunk_tiles[y][x] = (TileID)2;
			}
		}
	}

	void Add(glm::ivec2 &pos) {
		// pos & 15;
		// ((pos & ~15) >> 4)
	}
};

struct World {
	std::vector<Chunk> world_chunks;
	std::map<glm::ivec2, Chunk> chunk_id;

	World() {
		world_chunks.push_back(Chunk{});
	}

	void Add(TileID tile_type, glm::ivec2 pos) {
		
	}
};

class WorldSystem {
	Registry *registry = Registry::SharedInstance();
	std::shared_ptr<System> system;

public:
	void Init() {
		// system = registry->View<Camera>();
	}

	void LoadWorld() {

	}

	void SaveWorld() {

	}

	void Update() {
		for (const auto& entity: system->entities) {
			
		}
	}
};
