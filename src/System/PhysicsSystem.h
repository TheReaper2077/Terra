#pragma once

#include "../define.h"
#include "../ECS/Registry.h"
#include "../Components/Components.h"

class PhysicsSystem {
	Registry *registry = Registry::SharedInstance();
	std::shared_ptr<System> system;

public:
	void Init() {
		system = registry->View<Transform2D, Movement>();
	}

	void Update(const double &dt, GLFWwindow *window) {
		for (const auto& entity: system->entities) {
			auto& movement = registry->GetComponent<Movement>(entity);
			auto& transform = registry->GetComponent<Transform2D>(entity);

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				transform.pos.y -= movement.speed * dt;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				transform.pos.y += movement.speed * dt;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				transform.pos.x -= movement.speed * dt;
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				transform.pos.x += movement.speed * dt;
			}
		}
	}

	void FixedUpdate(const double &dt) {

	}
};