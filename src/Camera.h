#pragma once

#include "define.h"
#include "Window.h"
#include "events/MouseListener.h"
#include "render/Renderer.h"

// Most of the code here is shamelessly stolen from www.learnopengl.com
// WTF was I supposed to do ? I'm only 16, I don't even know Calcus, Linear Algerbra and all that bullshit

class Camera {
public:
	glm::vec3 camera_pos;
	glm::vec3 camera_front;
	glm::vec3 camera_up;

	float camera_speed = 0.55;

	GLFWwindow *window;
	MouseListener *mouseListener = MouseListener::SharedInstance();
	Renderer *renderer = Renderer::SharedInstance();

	bool firstMouse = true;
	float yaw = -90.0, pitch = 0;
	float lastX = 400, lastY = 300;

public:
	void Init(GLFWwindow *window) {
		this->window = window;

		camera_pos = glm::vec3(5.0f, 50.0f,  -5.0f);
		camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
		camera_up = glm::vec3(0.0f, 1.0f,  0.0f);

		renderer->SetProjection(glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f));
		renderer->SetModel(glm::mat4(1.0f));
	}

	void Update(const double &dt) {
		mouseCallback();
		float speed = camera_speed*dt;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera_pos += speed * camera_front;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera_pos -= speed * camera_front;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera_pos -= camera_up * speed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera_pos += camera_up * speed;

		renderer->SetView(glm::lookAt(camera_pos, camera_pos + camera_front, camera_up));
	}
	
	glm::vec3 direction;

	void mouseCallback() {
		if (mouseListener->isMoved()) {
			double xpos = mouseListener->getX();
			double ypos = mouseListener->getY();
			
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

			yaw += xoffset;
			pitch += yoffset;

			if(pitch > 89.0f)
				pitch = 89.0f;
			if(pitch < -89.0f)
				pitch = -89.0f;

			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

			camera_front = glm::normalize(direction);
			
			// std::printf("(%f, %f), (%f, %f, %f)\n", pitch, yaw, direction.x, direction.y, direction.z);
		}
	}
};
