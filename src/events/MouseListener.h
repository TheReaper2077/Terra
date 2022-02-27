#pragma once

#include "../define.h"

class MouseListener {
private:
	bool moved = false;
	double xpos, ypos;

private:
	MouseListener() {}

public:
	static MouseListener *SharedInstance() {
		static MouseListener instance;
		return &instance;
	}

	static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		SharedInstance()->moved = true;
		SharedInstance()->xpos = xpos;
		SharedInstance()->ypos = ypos;
	}

	bool isMoved() {
		if (moved) {
			moved = false;
			return true;
		} else {
			return false;
		}
	}

	double getX() {
		return xpos;
	}

	double getY() {
		return ypos;
	}
};
