#pragma once

#include "../define.h"
#include "../render/Renderer.h"

struct TextureComponent {
	Texture *texture;
	glm::vec2 pos;
	glm::vec2 scale;

	// TextureComponent() {}
};

struct Transform2D {
	glm::vec2 pos;
	glm::vec2 scale;

	// Transform2D() {}
};

struct Rigidbody2D {
	glm::vec2 velocity;
	glm::vec2 acceleration;
	float mass;
	bool simulated;

	Rigidbody2D() {}
};

struct MVP_Matrix {
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	// MVP_Matrix() {}
};

struct Movement {
	bool up;
	bool down;
	bool left;
	bool right;
	float speed = 3;
};

// struct Camera {
// 	bool move;
// 	glm::mat4 view = glm::mat4(1.0f);

// 	// Camera() {}
// };

struct MouseProperties {
	
};

struct KeyboardProperties {

};

struct TileSet {
	bool loaded;
	std::string filename;
	glm::vec2 tile_size;
	TextureComponent texture_component;

	TileSet() {}
};

struct Coordinates {
	glm::vec2 sceen_space;
	glm::vec2 world_space;

	// Coordinates() {}
};

struct Animation {
	
};