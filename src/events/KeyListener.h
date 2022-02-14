#pragma once

#include "../define.h"

class KeyListener {
public:
	unsigned int keyState[GLFW_KEY_LAST + 1] = {GLFW_RELEASE};

private:
	KeyListener() {}

public:
	static KeyListener *sharedInstance() {
		static KeyListener instance;
		return &instance;
	}

	bool toggle = true;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (sharedInstance()->keyState[key] == GLFW_RELEASE && action == GLFW_PRESS && key == GLFW_KEY_E) {
			auto &toggle = sharedInstance()->toggle;
			glPolygonMode(GL_FRONT_AND_BACK, (toggle) ? GL_LINE : GL_FILL);
			toggle = !toggle;
		}
		sharedInstance()->keyState[key] = action;
	}
	
	static bool isKeyPressed(int key) {
		return (sharedInstance()->keyState[key] == GLFW_PRESS || sharedInstance()->keyState[key] == GLFW_REPEAT);
	}

	static bool isKeyReleased(int key) {
		return (sharedInstance()->keyState[key] == GLFW_RELEASE);
	}

	static bool isKeyTyping(int key) {
		return (sharedInstance()->keyState[key] == GLFW_REPEAT);
	}

	static unsigned int getKeyAction(int key) {
		return sharedInstance()->keyState[key];
	}
};
