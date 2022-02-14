#pragma once

#include "../define.h"
#include "../ECS/Registry.h"
#include "../render/Renderer.h"
#include "../TileRegistry.h"
#include "../World.h"

class RenderSystem {
private:
	Registry *registry = Registry::SharedInstance();
	Renderer *renderer = Renderer::SharedInstance();
	TileRegistry *tile_registry = TileRegistry::SharedInstance();

	std::shared_ptr<System> texture_render, tile_render, world_render;
	
public:
	void Init() {
		texture_render = registry->View<TextureComponent, Transform2D>();
		tile_render = registry->View<Transform2D, TileID>();
		world_render = registry->View<World>();
	}

	void Render() {
		for (const auto& entity: world_render->entities) {
			const auto& world = registry->GetComponent<World>(entity);

			for (const auto& chunk: world.world_chunks) {
				for (int y = 0; y != 16; y++) {
					for (int x = 0; x != 16; x++) {
						if (y < 6 || y > 10) break;
						const auto& tile = tile_registry->GetTile(chunk.chunk_tiles[y][x]);
						renderer->TextureRect(x*32, y*32, 32, 32, tile);
					}
				}
			}
		}

		for (const auto& entity: texture_render->entities) {
			const auto& transform = registry->GetComponent<Transform2D>(entity);
			const auto& texture = registry->GetComponent<TextureComponent>(entity);

			renderer->TextureRect(transform.pos, transform.scale, texture.pos, texture.scale, texture.texture);
		}

		for (const auto& entity: tile_render->entities) {
			const auto& transform = registry->GetComponent<Transform2D>(entity);
			const auto& tile_id = registry->GetComponent<TileID>(entity);
			const auto& tile = tile_registry->GetTile(tile_id);

			renderer->TextureRect(transform.pos, transform.scale, tile);
		}

		renderer->Render();
	}
};
