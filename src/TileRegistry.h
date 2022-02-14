#pragma once

#include "define.h"
#include "ECS/Registry.h"
#include "Components/Components.h"

using TileID = uint32_t;

struct Tile {
	float x, y, w, h;
	Texture *texture;
};

class TileRegistry {
private:
	TileID next_tile_id = 0;
	std::vector<Tile> tile_array;
	std::map<std::string, TileID> tile_registry_map;

	TileRegistry() {}

public:
	static TileRegistry *SharedInstance() {
		static TileRegistry instance;
		return &instance;
	}

	TileID RegisterTile(const std::string &name, const Tile tile) {
		tile_registry_map[name] = next_tile_id;
		tile_array.push_back(tile);

		return next_tile_id++;
	}

	Tile &GetTile(const TileID &tile_id) {
		return tile_array[tile_id];
	}

	void Render() {
		for (const auto& tile: tile_array) {
			// ImGui::Image((void*)(intptr_t)tile.texture->id, ImVec2(80, 80), ImVec2((float)1*80.0/tile.texture->width, 0*80.0/tile.texture->height), ImVec2((float)1*80.0/tile.texture->width, (float)1*80.0/tile.texture->height));
		}
	}
};
