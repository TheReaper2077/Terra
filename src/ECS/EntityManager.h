#pragma once

#include "Types.h"
#include "ComponentManager.h"

class EntityManager {
private:
	std::vector<Entity> available_entities;		// replace with deque in the future
	std::map<Entity, ID> entity_id_map;
	Entity next_entity;

	EntityManager(): next_entity(0) {}
	ComponentManager *component_manager = ComponentManager::SharedInstance();

public:
	static EntityManager *SharedInstance() {
		static EntityManager instance;
		return &instance;
	}

	Entity CreateEntity() {
		Entity entity;

		if (available_entities.size()) {
			entity = available_entities.back();
			available_entities.pop_back();
		} else {
			entity = next_entity;
			next_entity++;
		}

		return entity;
	}

	template <typename T>
	void AddComponent(const Entity &entity) {
		entity_id_map[entity] |= (1 << component_manager->GetComponentType<T>());
	}

	template <typename T>
	void RemoveComponent(const Entity &entity) {
		entity_id_map[entity] &= !(1 << component_manager->GetComponentType<T>());
	}

	ID& GetEntityID(const Entity &entity) {
		return entity_id_map[entity];
	}

	void SetEntityID(const Entity &entity, const ID entity_id) {
		entity_id_map[entity] = entity_id;
	}

	std::map<Entity, ID>& GetEntityIDArray() {
		return entity_id_map;
	}

	void DestroyEntity(const Entity &entity) {
		available_entities.push_back(entity);
		entity_id_map[entity] = 0;
	}
};