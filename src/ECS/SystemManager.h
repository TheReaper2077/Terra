#pragma once

#include "Types.h"
#include "EntityManager.h"
#include "ComponentManager.h"

struct System {
	std::set<Entity> entities;
	ID id, filter_id;

	template <typename ...T>
	void Filter() {
		for (const auto& component_name: {typeid(T).name()...}) {
			filter_id |= 1 << ComponentManager::SharedInstance()->GetComponentType(component_name);
		}

		for (const auto& pair: EntityManager::SharedInstance()->GetEntityIDArray()) {
			if ((filter_id & pair.second) == filter_id) {
				entities.erase(pair.first);
			}
		}
	}

	template <typename ...T>
	void Except() {
		// for (const auto& component_name: {typeid(T).name()...}) {
		// 	id |= 1 << ComponentManager::SharedInstance()->GetComponentType(component_name);
		// }

		// for (const auto& pair: EntityManager::SharedInstance()->GetEntityIDArray()) {
		// 	if ((filter_id & pair.second) == filter_id) {
		// 		entities.erase(pair.first);
		// 	}
		// }
	}
};

class SystemManager {
private:
	std::vector<std::shared_ptr<System>> system_array;
	unsigned int next_index = 0;

	SystemManager() {}
	ComponentManager *component_manager = ComponentManager::SharedInstance();
	EntityManager *entity_manager = EntityManager::SharedInstance();

public:
	static SystemManager *sharedInstance() {
		static SystemManager instance;
		return &instance;
	}

	void EntityIDChanged(const Entity &entity) {
		const auto& entity_id = entity_manager->GetEntityID(entity);
		
		for (const auto& system: system_array) {
			if ((entity_id & system->id) == system->id) {
				if ((system->filter_id & entity_id) == 0) {
					system->entities.insert(entity);
				}
			} else {
				system->entities.erase(entity);
			}
		}
	}

	template <typename ...T>
	std::shared_ptr<System> CreateView() {
		ID temp_id = 0;
		for (const auto& component_name: {typeid(T).name()...}) {
			temp_id |= 1 << component_manager->GetComponentType(component_name);
		}
			
		std::set<Entity> temp_set;

		for (const auto& pair: entity_manager->GetEntityIDArray()) {
			if ((temp_id & pair.second) == temp_id) {
				temp_set.insert(pair.first);
			}
		}

		System system{temp_set, temp_id, 0};

		system_array.push_back(std::make_shared<System>(system));

		return system_array.back();
	}
};
