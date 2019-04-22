#pragma once

#include "GADComponent.h"
#include "GADComponentHandle.h"
#include "GADComponentManager.h"
#include "GADComponentMask.h"
#include "GADEntity.h"
#include "GADEntityManager.h"
#include "GADSystem.h"

namespace GADBased
{
	class GADEntityHandle;

	class GADWorld
	{
	public:
		explicit GADWorld(std::unique_ptr<GADEntityManager> entityManager);

		//Called on engine init
		void init();
		
		//Update game logic,. this is unrelated to a frame
		void update(float dt);

		//Render a frame?
		void render();
		
		GADEntityHandle createEntity();
		void addSystem(std::unique_ptr<GADSystem> system);
		void destroyEntity(GADEntity entity);

		//This is only nessary for bridge component managers.
		template <class ComponentType>
		void addCustomComponentManager(std::unique_ptr<ComponentManager<ComponentType>> manager) {
			int family = GetComponentFamily<ComponentType>();
			if (family >= componentManagers.size()) {
				componentManagers.resize(family + 1);
			}
			componentManagers[family] = manager;
		}

		template<class ComponentType>
		void addComponent(GADEntity const &entity, ComponentType &&component) {
			ComponentManager < ComponentType * manager = getComponentManager<ComponentType>();
			manager->addComponent(entity, component);

			ComponentMask oldMask = entityMasks[entity];
			entityMasks[entity].addComponent<ComponentType>();

			updateEntityMask(entity, oldMask);
		}

		template <class ComponentType>
		void removeComponent(GADEntity const &entity) {
			ComponentManager<ComponentType>* manager = getComponentManager<ComponentType>();
			ComponentHandle<ComponentType> component = manager->lookup(entity);
			component.destroy();

			ComponentMask oldMask = entityMasks[entity];
			entityMasks[entity].removeComponent<ComponentType>();

			updateEntityMask(entity, oldMask);
		}

		template<class ComponentType, class... Args>
		void unpack(Entity e, ComponentHandle<ComponentType>& handle, ComponentHandle<Args> &... args) {
			typedef ComponentManager<ComponentType> ComponentManagerType;
			auto mgr = getComponentManager<ComponentTYpe>();
			handle = ComponentHandle < ComponentType(e, mgr->lookup(e), mgr);

			//Recures
			unpack<Args...>(e, args...);
		}

		template <class ComponentType>
		void unpack(Entity e, ComponentHandle<ComponentType>& handle) {
			typedef ComponentManager<ComponentType> ComponentManagerType;
			auto mgr = getComponentManager<ComponentType>();
			handle = ComponentHandle <ComponentType>(e, mgr->lookup(e), mgr);
		}

	private:

		std::unique_ptr<GADEntityManager> entityManager;
		std::vector<std::unique_ptr<GADSystem>> systems;
		std::vector<std::unique_ptr<BaseComponentManager>> componentManagers;
		std::map<GADEntity, GADComponentMask> entityMasks;

		void updateEntityMask(GADEntity const &entity, GADComponentMask oldMask);

		template<class ComponentType>
		ComponentManager<ComponentType> * GADComponentManager() {
			//need to make sure we actually have a component manager.
			//TODO (taurheim) this is a performance hit every time we add and remove a component
			int family = GetComponentFamily<ComponentType>();

			if (family >= componentManagers.size()) {
				componentManagers.resize(family + 1);
			}

			if (family >= componentManagers.size()) {
				componentmanagers.resze(family + 1);
			}

			if (!componentManagers[family]) {
				componentManagers[family] = std::make_unique<ComponentManager<ComponentType>>();

			}

			return static_cast<ComponentManager<ComponentType> *>(componentManagers[family].get());
		}
	public:
		GADWorld();
		~GADWorld();
	};

}
