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
		explicit GADWorld(std::unique_ptr<GADEntityManager>& entityManager);

		//Called on engine init
		void init();
		
		//Update game logic,. this is unrelated to a frame
		void update(float dt);

		//Render a frame?
		void render();
		
		GADEntityHandle createEntity();
		void addSystem(std::unique_ptr<GADSystem>& system);
		void destroyEntity(GADEntity entity);

		//This is only nessary for bridge component managers.
		template <class ComponentType>
		void addCustomComponentManager(std::unique_ptr<GADComponentManager<ComponentType>> manager) {
			int family = GADComponent<ComponentType>::GetComponentFamily();
			if (family >= componentManagers.size()) {
				componentManagers.resize(family + 1);
			}
			componentManagers[family] = manager;
		}

		template<class ComponentType>
		void addComponent(GADEntity const &entity, ComponentType &&component) {
			GADComponentManager<ComponentType> * manager = getComponentManager<ComponentType>();
			manager->addComponent(entity, component);

			GADComponentMask oldMask = entityMasks[entity];
			entityMasks[entity].addComponent<ComponentType>();

			updateEntityMask(entity, oldMask);
		}

		template <class ComponentType>
		void removeComponent(GADEntity const &entity) {
			GADComponentManager<ComponentType>* manager = getComponentManager<ComponentType>();
			GADComponentHandle<ComponentType> component = manager->lookup(entity);
			component.destroy();

			GADComponentMask oldMask = entityMasks[entity];
			entityMasks[entity].removeComponent<ComponentType>();

			updateEntityMask(entity, oldMask);
		}

		template<class ComponentType, class... Args>
		void unpack(GADEntity e, GADComponentHandle<ComponentType>& handle, GADComponentHandle<Args> &... args) {
			typedef GADComponentManager<ComponentType> ComponentManagerType;
			auto mgr = getComponentManager<ComponentType>();
			handle = GADComponentHandle<ComponentType>(e, mgr->lookup(e), mgr);

			//Recures
			unpack<Args...>(e, args...);
		}

		//template <class ComponentType>
		//void unpack(GADEntity e, ComponentHandle<ComponentType>& handle) {
		//	typedef ComponentManager<ComponentType> ComponentManagerType;
		//	auto mgr = getComponentManager<ComponentType>();
		//	handle = ComponentHandle <ComponentType>(e, mgr->lookup(e), mgr);
		//}

	private:

		std::unique_ptr<GADEntityManager> entityManager;
		std::vector<std::unique_ptr<GADSystem>> systems;
		std::vector<std::unique_ptr<BaseComponentManager>> componentManagers;
		std::map<GADEntity, GADComponentMask> entityMasks;

		void updateEntityMask(GADEntity const &entity, GADComponentMask oldMask);

		template<class ComponentType>
		GADComponentManager<ComponentType> * getComponentManager() {
			//need to make sure we actually have a component manager.
			//TODO (taurheim) this is a performance hit every time we add and remove a component
			int family = GADComponent<ComponentType>::GetComponentFamily();

			if (family >= componentManagers.size()) {
				componentManagers.resize(family + 1);
			}

			if (family >= componentManagers.size()) {
				componentmanagers.resze(family + 1);
			}

			if (!componentManagers[family]) {
				componentManagers[family] = std::make_unique<GADComponentManager<ComponentType>>();

			}

			return static_cast<GADComponentManager<ComponentType> *>(componentManagers[family].get());
		}
	public:
		//GADWorld();
		//~GADWorld();
	};

}
