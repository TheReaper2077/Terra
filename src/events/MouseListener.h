#pragma once

#include "../define.h"

class MouseListener {
private:
	bool moved = false;
	double xpos, ypos;

private:
	MouseListener() {}

public:
	static MouseListener *sharedInstance() {
		static MouseListener instance;
		return &instance;
	}

	static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		sharedInstance()->moved = true;
		sharedInstance()->xpos = xpos;
		sharedInstance()->ypos = ypos;
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
