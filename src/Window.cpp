#include "Window.h"
#include "Camera.h"
#include "World.h"
#include "TileRegistry.h"
#include "render/Renderer.h"
#include "events/KeyListener.h"
#include "events/MouseListener.h"

Renderer *renderer = Renderer::SharedInstance();
TileRegistry *tile_registry = TileRegistry::SharedInstance();
Camera *camera = new Camera();
World *world = world = new World();

Window::Window() {

}

void Window::Init() {
	assert(glfwInit());

	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);
	
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowPos(window, 50, 50);
	glfwSetKeyCallback(window, KeyListener::keyCallback);
	glfwSetCursorPosCallback(window, MouseListener::mouseCallback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glViewport(0, 0, WIDTH, HEIGHT);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderer->Init();
	camera->Init(window);

	Texture map_sprites = renderer->LoadTexture("D:\\C++\\Ortho Test\\res\\spritesheet\\Spritesheet.PNG");

	auto GRASS = tile_registry->RegisterTile("GRASS", Tile{0*80, 0*80, 80, 80, map_sprites});
	auto DIRT = tile_registry->RegisterTile("DIRT", Tile{1*80, 0*80, 80, 80, map_sprites});
	auto STONE = tile_registry->RegisterTile("STONE", Tile{2*80, 0*80, 80, 80, map_sprites});
	auto ICE = tile_registry->RegisterTile("ICE", Tile{3*80, 0*80, 80, 80, map_sprites});
	auto SAND = tile_registry->RegisterTile("SAND", Tile{4*80, 0*80, 80, 80, map_sprites});
	auto WOOD = tile_registry->RegisterTile("WOOD", Tile{5*80, 0*80, 80, 80, map_sprites});
	auto EMARALD_ORE = tile_registry->RegisterTile("EMARALD_ORE", Tile{0*80, 1*80, 80, 80, map_sprites});
	auto TNT = tile_registry->RegisterTile("TNT", Tile{1*80, 1*80, 80, 80, map_sprites});
	auto GLASS = tile_registry->RegisterTile("GLASS", Tile{2*80, 1*80, 80, 80, map_sprites});
	auto GOLD_ORE = tile_registry->RegisterTile("GOLD_ORE", Tile{3*80, 1*80, 80, 80, map_sprites});
	auto DIAMOND_ORE = tile_registry->RegisterTile("DIAMOND_ORE", Tile{4*80, 1*80, 80, 80, map_sprites});
	auto REDSTONE_ORE = tile_registry->RegisterTile("REDSTONE_ORE", Tile{5*80, 1*80, 80, 80, map_sprites});
	auto GRASS_TOP = tile_registry->RegisterTile("GRASS_TOP", Tile{2*80, 2*80, 80, 80, map_sprites});

	auto GRASS_BLOCK = tile_registry->RegisterBlock("GRASS_BLOCK", BlockTiles{GRASS, GRASS, GRASS_TOP, DIRT, GRASS, GRASS});
	auto STONE_BLOCK = tile_registry->RegisterBlock("STONE_BLOCK", BlockTiles{STONE, STONE, STONE, STONE, STONE, STONE});
	auto DIRT_BLOCK = tile_registry->RegisterBlock("DIRT_BLOCK", BlockTiles{DIRT, DIRT, DIRT, DIRT, DIRT, DIRT});

	// renderer->SetProjection(glm::ortho<float>(0, WIDTH, HEIGHT, 0, -1, 1));
	// renderer->SetView(glm::mat4(1.0f));
	// renderer->SetModel(glm::mat4(1.0f));

	// for (int z = -16*5; z != 16*5; z++) {
	// 	for (int x = -16*5; x != 16*5; x++) {
	// 		float value = glm::simplex(glm::vec2{x / 64.0, z / 64.0});
	// 		value = (value + 1) / 2;
	// 		value *= 16 + 16;
	// 		for (int y = 0; y <= value; y++) {
	// 			if (y > value - 1) {
	// 				world->AddBlock(Block{SOLID, GRASS_BLOCK}, glm::ivec3(x, y, z));
	// 			} else if (y > value - 4) {
	// 				world->AddBlock(Block{SOLID, DIRT_BLOCK}, glm::ivec3(x, y, z));
	// 			} else {
	// 				world->AddBlock(Block{SOLID, STONE_BLOCK}, glm::ivec3(x, y, z));
	// 			}
	// 		}
	// 	}
	// }

	// for (int z = -16*1; z != 16*1; z++) {
	// 	for (int x = -16*1; x != 16*1; x++) {
	// 		world->AddBlock(Block{SOLID, STONE}, glm::ivec3(x, x + z - 1, z));
	// 		world->AddBlock(Block{SOLID, STONE}, glm::ivec3(x, x + z - 2, z));
	// 		world->AddBlock(Block{SOLID, STONE}, glm::ivec3(x, x + z - 3, z));
	// 		world->AddBlock(Block{SOLID, STONE}, glm::ivec3(x, x + z - 4, z));
	// 	}
	// }
}

void Window::Gameloop() {
	while (!glfwWindowShouldClose(window)) {
		const auto &t_start = std::chrono::high_resolution_clock::now();

		glfwPollEvents();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->Update(dt);

		renderer->SetColor(255, 0, 255, 255);
		world->Render();

		renderer->Render();

		glfwSwapBuffers(window);

		const auto &t_end = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<double, std::ratio<1, 60>>(t_end-t_start).count();
	}
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

