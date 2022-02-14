#include "Window.h"
#include "Camera.h"
#include "render/Renderer.h"
#include "events/KeyListener.h"
#include "events/MouseListener.h"

Renderer *renderer = Renderer::SharedInstance();
Camera *camera = new Camera();

Window::Window() {

}

void Window::Init() {
	assert(glfwInit());

	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);
	
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowPos(window, 20, 20);
	glfwSetKeyCallback(window, KeyListener::keyCallback);
	glfwSetCursorPosCallback(window, MouseListener::mouseCallback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glViewport(0, 0, WIDTH, HEIGHT);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderer->Init();
	camera->Init(window);

	// renderer->SetProjection(glm::ortho<float>(0, WIDTH, HEIGHT, 0, -1, 1));
	// renderer->SetView(glm::mat4(1.0f));
	// renderer->SetModel(glm::mat4(1.0f));
}

void Window::Gameloop() {
	auto texture = renderer->LoadTexture("D:\\C++\\Ortho Test\\res\\spritesheet\\Spritesheet.PNG");
	while (!glfwWindowShouldClose(window)) {
		const auto &t_start = std::chrono::high_resolution_clock::now();

		glfwPollEvents();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->Update(dt);

		renderer->SetColor(255, 0, 255, 255);
		// renderer->FillRect(0, 0, 10, 10);
		renderer->TextureRect(0, 0, 1, 1, 0, 0, 80, 80, &texture);

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
