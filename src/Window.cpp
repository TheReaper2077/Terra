#include "Window.h"
#include "Camera.h"

#include "ChunkGeneration.h"
#include "ChunkManager.h"

#include "Raycasting.h"
#include "TileRegistry.h"
#include "render/Renderer.h"
#include "events/KeyListener.h"
#include "events/MouseListener.h"

Renderer *renderer = Renderer::SharedInstance();
TileRegistry *tile_registry = TileRegistry::SharedInstance();
Camera *camera = new Camera();
ChunkManager *chunk_manager = new ChunkManager();
Ray *ray = new Ray();
// World *world = world = new World();

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
	ray->Init(chunk_manager);

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

	tile_registry->RegisterBlock(GRASS_BLOCK, BlockTiles{GRASS, GRASS, GRASS_TOP, DIRT, GRASS, GRASS});
	tile_registry->RegisterBlock(STONE_BLOCK, BlockTiles{STONE, STONE, STONE, STONE, STONE, STONE});
	tile_registry->RegisterBlock(DIRT_BLOCK, BlockTiles{DIRT, DIRT, DIRT, DIRT, DIRT, DIRT});
	
	// for (int z = 0; z != 5; z++) {
	// 	for (int x = 0; x != 5; x++) {
	// 		for (int y = 0; y != 5; y++) {
	// 			chunk_manager->GetChunk(glm::ivec3(x*16, y*16, z*16));
	// 		}
	// 	}
	// }
}

void render_crosshair() {
	renderer->SetColor(255, 255, 255, 255);
	renderer->DrawLine(WIDTH/2 - 20, HEIGHT/2, WIDTH/2 + 20, HEIGHT/2);
	renderer->DrawLine(WIDTH/2, HEIGHT/2 - 20, WIDTH/2, HEIGHT/2 + 20);
}

void Window::Gameloop() {
	while (!glfwWindowShouldClose(window)) {
		const auto &t_start = std::chrono::high_resolution_clock::now();

		glfwPollEvents();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->Update(dt);

		renderer->SetColor(255, 0, 255, 255);

		chunk_manager->Generate();
		chunk_manager->Mesh();
		chunk_manager->Render((glm::ivec3)camera->camera_pos);
		
		// glm::vec3 tmp = camera->camera_pos + glm::vec3(camera->camera_front.x * 10.0f, camera->camera_front.y * 10.0f, camera->camera_front.z * 10.0f);
		// renderer->FillCube(tmp, 1, 1, 1);
		// renderer->SetColor(0, 255, 0, 255);
		// renderer->FillCube((glm::ivec3)tmp, 1, 1, 1);
		uint8_t face;
		ray->Update(camera->camera_pos, camera->camera_front, 20, face);
		renderer->FillCube(glm::vec3(ray->X - 0.1, ray->Y - 0.1, ray->Z - 0.1), 1.1, 1.1, 1.1, face);
		// renderer->FillCube(tmp, 1.1, 1.1, 1.1);
		
		render_crosshair();

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

