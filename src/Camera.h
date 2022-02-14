#pragma once

#include "define.h"
#include "Window.h"
#include "events/MouseListener.h"
#include "render/Renderer.h"

class Camera {
private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	float cameraSpeed = 0.05;

	GLFWwindow *window;
	MouseListener *mouseListener = MouseListener::sharedInstance();
	Renderer *renderer = Renderer::SharedInstance();

	bool firstMouse = true;
	float yaw = -90.0, pitch = 0;
	float lastX = 400, lastY = 300;

public:
	void Init(GLFWwindow *window) {
		this->window = window;

		cameraPos = glm::vec3(0.0f, 0.0f,  0.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

		renderer->SetProjection(glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f));
		renderer->SetModel(glm::mat4(1.0f));
	}

	void Update(const double &dt) {
		mouseCallback();
		float speed = cameraSpeed*dt;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += speed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= speed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			cameraPos -= cameraUp * speed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			cameraPos += cameraUp * speed;

		renderer->SetView(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp));
	}

	void mouseCallback() {
		if (mouseListener->isMoved()) {
			double xpos = mouseListener->getX();
			double ypos = mouseListener->getY();

			// std::cout << xpos << " : " << ypos << std::endl;

			if (firstMouse) {
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}
		
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos; 
			lastX = xpos;
			lastY = ypos;

			float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw   += xoffset;
			pitch += yoffset;

			if(pitch > 89.0f)
				pitch = 89.0f;
			if(pitch < -89.0f)
				pitch = -89.0f;

			glm::vec3 direction;
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(direction);
		}
	}
};
