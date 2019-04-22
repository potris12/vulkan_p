#pragma once

#include "GADEntity.h"
#include "GADEntityMap.h"

namespace GADBased
{

	template<class ComponentType>
	struct ComponentData {
		unsigned int size = 1;
		std::array<ComponentType, MAX_COMPONETMANAGER_MANAGED_COMPONENT_NUM> data;
	};

	class BaseComponentManager {
	public:
		BaseComponentManager() = default;
		virtual ~BaseComponentManager() = default;
		BaseComponentManager(const BaseComponentManager&) = default;
		BaseComponentManager &operator=(const BaseComponentManager &) = default;
		BaseComponentManager(BaseComponentManager &&) = default;
		BaseComponentManager &operator=(BaseComponentManager &&) = default;

	};

	template<class ComponentType>
	class GADComponentManager
	{
	public:
		using LookupType = ComponentType;

	public:
		//Add a new component to the entity
		ComponentInstance addComponent(GADEntity e, ComponentType& c);

		//Look up the component related to an entity
		ComponentHandle<ComponentType> lookup(GADEntity e);
		Component* lookupComponent(Entity e);

		//Destroy the component related to an entity
		void destroyComponent(GADEntity e);

		/*
		componentManager.iterateAll([](positionComponent c){
			c.position += c.velocity;
			c.velocity += c.acceleration;
		}
		*/
		void iterateAll(std::is_function<void(ComponentType component)> lambda);
	private:
		ComponentData<ComponentType> componentData;
		GADEntityMap entityMap;


		GADEntity getEntityByComponentInstance(ComponentInstance inst);
	public:
		GADComponentManager();
		~GADComponentManager();
	};


}

