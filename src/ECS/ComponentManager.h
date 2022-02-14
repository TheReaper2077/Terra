#pragma once

#include "Types.h"

class IComponentArray {
public:
	virtual void DestroyEntity(const Entity &entity) = 0;
};

template <typename T>
class ComponentArray: public IComponentArray {
private:
	unsigned int next_index;
	
	std::vector<T> component_data_array;
	std::map<Entity, unsigned int> entity_index_map;
	std::map<unsigned int, Entity> index_entity_map;

public:
	ComponentArray(): next_index(0) {}

	void Add(const Entity &entity, T data) {
		if (entity_index_map.find(entity) != entity_index_map.end()) {
			component_data_array[entity_index_map[entity]] = data;
		} else {
			component_data_array.push_back(data);
			entity_index_map[entity] = next_index;
			index_entity_map[next_index] = entity;
			
			next_index++;
		}
	}

	void Remove(const Entity &entity) {
		// assert(entity_index_map.find(entity) != entity_index_map.end());
		if (entity_index_map.find(entity) != entity_index_map.end()) {
			next_index--;

			unsigned int index_removed = entity_index_map[entity];
			Entity last_entity = index_entity_map[next_index];
			
			entity_index_map[index_removed] = last_entity;
			index_entity_map[last_entity] = next_index;

			component_data_array[index_removed] = component_data_array[next_index];

			component_data_array.pop_back();
			entity_index_map.erase(entity);
			index_entity_map.erase(next_index);
		}
	}

	T& Get(const Entity &entity) {
		assert(entity_index_map.find(entity) != entity_index_map.end());

		return component_data_array[entity_index_map[entity]];
	}

	void DestroyEntity(const Entity &entity) {
		Remove(entity);
	}
};


class ComponentManager {
private:
	std::vector<std::shared_ptr<IComponentArray>> component_array;
	std::map<const char*, ComponentType> registered_components;

	ComponentType next_component_type;

	ComponentManager(): next_component_type(0) {}
public:
	static ComponentManager *SharedInstance() {
		static ComponentManager instance;
		return &instance;
	}
	
	template <typename T>
	void Register() {
		assert(registered_components.find(typeid(T).name()) == registered_components.end() && "[ERROR] Component Already Registered");
		assert(next_component_type < 64 && "[ERROR] MAX COMPONENTS EXCEEDED");

		component_array.push_back(std::make_shared<ComponentArray<T>>());
		registered_components[typeid(T).name()] = next_component_type;
		next_component_type++;

		std::cout << "[REGISTER] " << typeid(T).name() << std::endl;
	}

	template <typename T>
	void AddComponent(const Entity &entity, T data) {
		Get<T>()->Add(entity, data);
	}

	template <typename T>
	void RemoveComponent(const Entity &entity) {
		Get<T>()->Remove(entity);
	}

	template <typename T>
	T& GetComponent(const Entity &entity) {
		return Get<T>()->Get(entity);
	}

	template <typename T>
	ComponentType GetComponentType() {
		return registered_components[typeid(T).name()];
	}

	ComponentType GetComponentType(const char* name) {
		return registered_components[name];
	}

	void DestroyEntity(const Entity &entity) {
		for (auto array: component_array) {
			array->DestroyEntity(entity);
		}
	}

private:
	template <typename T>
	std::shared_ptr<ComponentArray<T>> Get() {
		return std::static_pointer_cast<ComponentArray<T>>(component_array[registered_components[typeid(T).name()]]);
	}
};
