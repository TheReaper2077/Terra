#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#include "define.h"
#include "Window.h"

int main() {
	Window window;
	window.Init();
	window.Gameloop();
}
