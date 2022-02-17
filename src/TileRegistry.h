#pragma once

#include "define.h"
#include "ECS/Registry.h"
#include "Components/Components.h"

using TileID = uint32_t;

enum BlockID {
	AIR_BLOCK,
	GRASS_BLOCK,
	STONE_BLOCK,
	DIRT_BLOCK,
};

struct Tile {
	float x, y, w, h;
	float tx, ty, tw, th;
	Texture texture;

	Tile(float x, float y, float w, float h, Texture &texture) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		this->texture = texture;

		tx = x/texture.width;
		ty = y/texture.height;
		tw = w/texture.width;
		th = h/texture.height;
	}
};

struct BlockTiles {
	TileID front;
	TileID back;
	TileID top;
	TileID bottom;
	TileID left;
	TileID right;
};

class TileRegistry {
private:
	TileID next_tile_id = 0;
	std::vector<Tile> tile_array;
	std::map<std::string, TileID> tile_registry_map;

	unsigned int next_block_id_index;
	std::map<BlockID, unsigned int> block_registry_map;
	std::vector<BlockTiles> block_array;

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

	void RegisterBlock(const BlockID &id, const BlockTiles block_tiles) {
		block_registry_map[id] = next_block_id_index++;
		block_array.push_back(block_tiles);
	}

	Tile &GetTile(const TileID &tile_id) {
		return tile_array[tile_id];
	}

	BlockTiles &GetBlock(const BlockID &block_id) {
		return block_array[block_registry_map[block_id]];
	}

	void Render() {
		for (const auto& tile: tile_array) {
			// ImGui::Image((void*)(intptr_t)tile.texture->id, ImVec2(80, 80), ImVec2((float)1*80.0/tile.texture->width, 0*80.0/tile.texture->height), ImVec2((float)1*80.0/tile.texture->width, (float)1*80.0/tile.texture->height));
		}
	}
};
