#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class Registry {
private:
	Registry() {}
	EntityManager *entity_manager = EntityManager::SharedInstance();
	ComponentManager *component_manager = ComponentManager::SharedInstance();
	SystemManager *system_manager = SystemManager::sharedInstance();

public:
	static Registry *SharedInstance() {
		static Registry instance;
		return &instance;
	}

	Entity CreateEntity() {
		return entity_manager->CreateEntity();
	}

	template <typename ...T>
	Entity CreateEntity() {
		ID temp_id = 0;
		for (const auto& component_name: {typeid(T).name()...}) {
			temp_id |= 1 << component_manager->GetComponentType(component_name);
		}

		auto entity = entity_manager->CreateEntity();
		entity_manager->SetEntityID(entity, temp_id);
		system_manager->EntityIDChanged(entity);
		return entity;
	}

	template <typename T>
	void Register() {
		component_manager->Register<T>();
	}

	template <typename T, typename ...M>
	void AddComponent(const Entity &entity, T data, M ...dataM) {
		entity_manager->AddComponent<T>(entity);
		component_manager->AddComponent<T>(entity, data);

		AddComponent(entity, dataM...);
	}

	void AddComponent(const Entity &entity) {
		system_manager->EntityIDChanged(entity);
	}

	template <typename T>
	void RemoveComponent(const Entity &entity) {
		entity_manager->RemoveComponent<T>(entity);
		component_manager->RemoveComponent<T>(entity);
		system_manager->EntityIDChanged(entity);
	}

	// template <typename ...T>
	// void RemoveComponent(const Entity &entity) {
	// 	ID temp_id = 0;
	// 	for (const auto& component_name: {typeid(T).name()...}) {
	// 		temp_id &= !(1 << component_manager->GetComponentType(component_name));
	// 	}

	// 	entity_manager->SetEntityID(entity, temp_id);
	// 	system_manager->EntityIDChanged(entity);
	// }

	template <typename T>
	T& GetComponent(const Entity &entity) {
		return component_manager->GetComponent<T>(entity);
	}

	void DestroyEntity(const Entity &entity) {
		entity_manager->DestroyEntity(entity);
		component_manager->DestroyEntity(entity);
	}

	template <typename ...T>
	std::shared_ptr<System> View() {
		return system_manager->CreateView<T...>();
	}
};