// #define STB_IMAGE_IMPLEMENTATION

// #include "define.h"
// #include "imgui/imgui.h"
// #include "imgui/imgui_impl_glfw.h"
// #include "imgui/imgui_impl_opengl3.h"

// #include "ECS/Registry.h"
// #include "Components/Components.h"

// #include "System/RenderSystem.h"
// #include "System/PhysicsSystem.h"

// #include "render/Renderer2D.h"
// #include "Camera2D.h"
// #include "World.h"
// #include "TileRegistry.h"

// Renderer2D *renderer = Renderer2D::SharedInstance();
// Registry *registry = Registry::SharedInstance();
// Camera2D camera;

// int main() {
// 	assert(glfwInit());

// 	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
// 	glfwMakeContextCurrent(window);
	
// 	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

// 	glEnable(GL_BLEND);

// 	// glViewport(0, 0, WIDTH, HEIGHT);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
// 	glm::mat4 projection = glm::ortho<float>(0, WIDTH, HEIGHT, 0, -1, 1);
// 	glm::mat4 view = glm::mat4(1.0f);
// 	glm::mat4 model = glm::mat4(1.0f);

// 	renderer->Init();
// 	renderer->SetProjection(projection);
// 	renderer->SetView(view);
// 	renderer->SetModel(model);

// 	Texture map_sprites = renderer->loadTexture("D:\\C++\\Lucy\\res\\spritesheet\\Spritesheet.PNG");

// 	TileRegistry *tile_registry = TileRegistry::SharedInstance();
// 	// tile_registry->Init();

// 	auto GRASS = tile_registry->RegisterTile("GRASS", Tile{0*80, 0*80, 80, 80, &map_sprites});
// 	auto DIRT = tile_registry->RegisterTile("DIRT", Tile{1*80, 0*80, 80, 80, &map_sprites});
// 	auto STONE = tile_registry->RegisterTile("STONE", Tile{2*80, 0*80, 80, 80, &map_sprites});
// 	auto ICE = tile_registry->RegisterTile("ICE", Tile{3*80, 0*80, 80, 80, &map_sprites});
// 	auto SAND = tile_registry->RegisterTile("SAND", Tile{4*80, 0*80, 80, 80, &map_sprites});
// 	auto WOOD = tile_registry->RegisterTile("WOOD", Tile{5*80, 0*80, 80, 80, &map_sprites});
// 	auto EMARALD_ORE = tile_registry->RegisterTile("EMARALD_ORE", Tile{0*80, 1*80, 80, 80, &map_sprites});
// 	auto TNT = tile_registry->RegisterTile("TNT", Tile{1*80, 1*80, 80, 80, &map_sprites});
// 	auto GLASS = tile_registry->RegisterTile("GLASS", Tile{2*80, 1*80, 80, 80, &map_sprites});
// 	auto GOLD_ORE = tile_registry->RegisterTile("GOLD_ORE", Tile{3*80, 1*80, 80, 80, &map_sprites});
// 	auto DIAMOND_ORE = tile_registry->RegisterTile("DIAMOND_ORE", Tile{4*80, 1*80, 80, 80, &map_sprites});
// 	auto REDSTONE_ORE = tile_registry->RegisterTile("REDSTONE_ORE", Tile{5*80, 1*80, 80, 80, &map_sprites});

// 	registry->Register<Transform2D>();
// 	registry->Register<Rigidbody2D>();
// 	registry->Register<TextureComponent>();
// 	registry->Register<TileSet>();
// 	registry->Register<Movement>();
// 	registry->Register<Coordinates>();
// 	registry->Register<World>();
// 	registry->Register<TileID>();

// 	auto player = registry->CreateEntity();
// 	registry->AddComponent(player, Transform2D{glm::vec2(0, 0), glm::vec2(80, 80)});
// 	registry->AddComponent(player, Movement{false, false, false, false, 3});
// 	registry->AddComponent(player, TNT);

// 	auto world = registry->CreateEntity();
// 	registry->AddComponent(world, World{});

// 	RenderSystem render_system;
// 	PhysicsSystem physics_system;

// 	render_system.Init();
// 	physics_system.Init();

// 	IMGUI_CHECKVERSION();
// 	ImGui::CreateContext();
// 	// ImGuiIO& io = ImGui::GetIO(); (void)io;
//     // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

// 	ImGui::StyleColorsDark();
// 	ImGui_ImplGlfw_InitForOpenGL(window, true);
// 	ImGui_ImplOpenGL3_Init("#version 400");

// 	double dt = 0;
// 	while (!glfwWindowShouldClose(window)) {
// 		const auto &t_start = std::chrono::high_resolution_clock::now();

// 		ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

// 		// ImGui::Image((void*)(intptr_t)map_sprites.id, ImVec2(512,512));
// 		// ImGui::Image((void*)(intptr_t)map_sprites.id, ImVec2(512,512));

// 		glfwPollEvents();

// 		glClearColor(0, 0, 0, 1);
// 		glClear(GL_COLOR_BUFFER_BIT);

// 		tile_registry->Render();

// 		ImGui::Render();

// 		physics_system.Update(dt, window);
// 		render_system.Render();
// 		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

// 		glfwSwapBuffers(window);

// 		const auto &t_end = std::chrono::high_resolution_clock::now();
// 		dt = std::chrono::duration<double, std::ratio<1, 60>>(t_end-t_start).count();
// 	}

// 	ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();

// 	glfwDestroyWindow(window);
// 	glfwTerminate();
// }

#define STB_IMAGE_IMPLEMENTATION

#include "define.h"
#include "Window.h"

int main() {
	Window window;
	window.Init();
	window.Gameloop();
}
