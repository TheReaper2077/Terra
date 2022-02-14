#pragma once

#include "define.h"

class Window {
	double dt;
	GLFWwindow *window;

public:
	Window();
	~Window();

	void Init();

	void Gameloop();
};