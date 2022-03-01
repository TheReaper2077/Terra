#include "Window.h"
#include "Camera.h"

#include "Chunk/ChunkManager.h"

#include "Raycasting.h"
#include "TileRegistry.h"
#include "render/Renderer.h"
#include "events/KeyListener.h"
#include "events/MouseListener.h"

Camera *camera = new Camera();
ChunkManager *chunk_manager = new ChunkManager();
Ray *ray = new Ray();
Renderer *renderer = Renderer::SharedInstance();
TileRegistry *tile_registry = TileRegistry::SharedInstance();
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

	
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);

	glViewport(0, 0, WIDTH, HEIGHT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

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

	// tile_registry->RegisterBlock(GRASS_BLOCK, BlockTiles{GRASS, GRASS, GRASS_TOP, DIRT, GRASS, GRASS});
	tile_registry->RegisterBlock(GRASS_BLOCK, BlockTiles{GRASS, GRASS, GRASS_TOP, DIRT, GRASS, GRASS});
	tile_registry->RegisterBlock(GLASS_BLOCK, BlockTiles{GLASS, GLASS, GLASS, GLASS, GLASS, GLASS});
	tile_registry->RegisterBlock(STONE_BLOCK, BlockTiles{STONE, STONE, STONE, STONE, STONE, STONE});
	tile_registry->RegisterBlock(DIRT_BLOCK, BlockTiles{DIRT, DIRT, DIRT, DIRT, DIRT, DIRT});
}

void render_crosshair() {
	renderer->SetColor(255, 255, 255, 255);
	renderer->DrawLine(WIDTH/2 - 20, HEIGHT/2, WIDTH/2 + 20, HEIGHT/2);
	renderer->DrawLine(WIDTH/2, HEIGHT/2 - 20, WIDTH/2, HEIGHT/2 + 20);
}

void render_axis_crosshair() {
	glm::vec3 tmp = camera->camera_pos + camera->camera_front * 15.0f;
	renderer->SetColor(255, 0, 0, 255);
	renderer->DrawLine(tmp.x, tmp.y, tmp.z, tmp.x + 1, tmp.y, tmp.z);
	renderer->SetColor(0, 0, 255, 255);
	renderer->DrawLine(tmp.x, tmp.y, tmp.z, tmp.x, tmp.y + 1, tmp.z);
	renderer->SetColor(0, 255, 0, 255);
	renderer->DrawLine(tmp.x, tmp.y, tmp.z, tmp.x, tmp.y, tmp.z + 1);
	glLineWidth(2.5);
	renderer->Render(LINE);
	glLineWidth(1);
}

void Window::Gameloop() {
	while (!glfwWindowShouldClose(window)) {
		const auto &t_start = std::chrono::high_resolution_clock::now();

		glfwPollEvents();
		glClearColor(0, 0.5, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		camera->Update(dt);

		renderer->SetColor(255, 0, 255, 255);

		chunk_manager->Generate();
		chunk_manager->Mesh();
		chunk_manager->Render((glm::ivec3)camera->camera_pos);

		glDisable(GL_DEPTH_TEST);

		render_axis_crosshair();
		
		glEnable(GL_DEPTH_TEST);

		uint8_t face;
		ray->Update(camera->camera_pos, camera->camera_front, 20, face);
		// renderer->FillCube(ray->voxel, 1.5, 1.5, 1.5, face);
		
		// renderer->FillCube(glm::vec3((float)ray->X - 0.1, (float)ray->Y - 0.1, (float)ray->Z - 0.1), 1.1, 1.1, 1.1);

		// glm::vec3 ray_nds = glm::vec3(0, 0, 0.0f);
		// glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, 0.0, 1.0);
		// glm::vec4 ray_eye = glm::inverse(renderer->projection) * ray_clip;
		// ray_eye = glm::vec4(ray_eye.x, ray_eye.y, ray_eye.z, 0.0);
		// glm::vec3 ray_wor = (glm::inverse(renderer->view) * ray_eye);

		// glm::vec3 res = glm::inverse(renderer->projection) * glm::inverse(renderer->view) * glm::inverse(renderer->model) * glm::vec4(0, 0, -1.0f, 1.0f);

		renderer->SetColor(0, 255, 0, 255);
		// renderer->DrawLine(camera->camera_pos.x - 1, camera->camera_pos.y, camera->camera_pos.z - 1, camera->camera_pos.x + 1, camera->camera_pos.y, camera->camera_pos.z + 1);
		// renderer->FillCube(tmp, 1.1, 1.1, 1.1);
		// don't forget to normalise the vector at some point
		// ray_wor = glm::normalize(ray_wor);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			chunk_manager->ChangeBlock(glm::ivec3(ray->X, ray->Y, ray->Z), AIR_BLOCK);
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
			chunk_manager->ChangeBlock(glm::ivec3(ray->X, ray->Y, ray->Z), GLASS_BLOCK);
		}

		// std::cout << camera->camera_front.x << " " << camera->camera_front.y << " " << camera->camera_front.z << "\n";

		// renderer->Render();

		glfwSwapBuffers(window);

		const auto &t_end = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<double, std::ratio<1, 60>>(t_end-t_start).count();
	}
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

