#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADEntityManager.h"
#include "GADSystemManager.h"
#include "GADComponentManager.h"

#include "GADComponent.h"
//#include "GADComponentHandle.h"
/*
world 는 
entity, component, system manager를 관리하는데
중요한건 entity가 어떤 component를 들고있는지 이놈이? 관리하도록 함 ???
*/

namespace GAD {

	class GADEntityHandle;

	class GADWorld : public GADBase
	{
		friend class GADEntityHandle;
		//friend class GADComponentHandle;

	public:
		void awake();
		void start();
		void update();
		void destroy();

		GADEntityHandle addEntity(const std::string& name);
		

	private:
		std::unique_ptr<GADEntityManager> entity_manager_;
		std::map<int64_t, std::unique_ptr<BaseComponentManager>> component_managers_;
		std::unique_ptr<GADSystemManager> system_manager_;


		template <typename ComponentType>
		GADComponentManager<ComponentType>* getComponentManager() {
			int64_t family = ComponentType::family();

			if (family >= component_managers_.size()) {
				component_managers_[family] = std::make_unique<GADComponentManager<ComponentType>>();
			}

			return static_cast<GADComponentManager<ComponentType>*>(component_managers_[family].get());
		}

		//template<class ComponentType>
		//GADComponentHandle<ComponentType> addComponent(GADEntity& entity) {
		//	auto thiz = std::static_pointer_cast<GADWorld>(shared_from_this());
		//	auto component_manager = getComponentManager<ComponentType>();
		//	auto& component = component_manager->addComponent();
		//	
		//	return GADComponentHandle<ComponentType>(thiz, &component);
		//
		//	//return 
		//}
		template<class ComponentType>
		ComponentType& addComponent(GADEntity& entity) {
			auto thiz = std::static_pointer_cast<GADWorld>(shared_from_this());
			auto component_manager = getComponentManager<ComponentType>();
			auto& component = component_manager->addComponent(entity);

			entity_component_familys_[entity.key_].push_back(ComponentType::family());
			return component;
			//return GADComponentHandle<ComponentType>(thiz, &component);

			//return 
		}

		template<class ComponentType>
		bool hasComponent(GADEntity& entity) {
			auto family = ComponentType::family();

			for (auto component_family : entity_component_familys_[entity.key_]) {
				if (family == component_family)
					return true;
			}
			return false;
		}

		template<class ComponentType>
		void removeComponent(GADEntity& entity) {
			auto thiz = std::static_pointer_cast<GADWorld>(shared_from_this());

			auto family = ComponentType::family();

			auto has_component = hasComponent<ComponentType>(entity);

			// 2. 있으면 지움 
			if (has_component) {
				entity_component_familys_[entity.key_].remove_if([family](int64_t my_family) {
					return family == my_family;
				});
				auto component_manager = getComponentManager<ComponentType>();
				component_manager->removeComponent(entity);
			}
		}

		bool hasComponent(GADEntity& entity, int64_t family) {
			
			for (auto component_family : entity_component_familys_[entity.key_]) {
				if (family == component_family)
					return true;
			}
			return false;
		}
		void removeComponent(GADEntity& entity, int64_t component_family) {
			auto thiz = std::static_pointer_cast<GADWorld>(shared_from_this());

			auto has_component = hasComponent(entity, component_family);

			// 2. 있으면 지움 
			if (has_component) {
				entity_component_familys_[entity.key_].remove_if([component_family](int64_t my_family) {
					return component_family == my_family;
				});
				component_managers_[component_family]->removeComponent(entity);
			}
		}

		void removeComponent(GADComponentBase& component) {
			component_managers_[component.family_]->removeComponent(component);
		}

		void removeEntity(const GADEntity& entity);


		//void addComponent(std::shared_ptr<GADComponentHandle> component);
		//GADComponentHandle & addComponent(std::shared_ptr<GADWorld> world, GADEntity& entity, GADComponentBase* component);
		
		//entity가 가지는 componet family index 
		std::map<int64_t, std::list<int64_t>> entity_component_familys_;

		void removeEntityComponents(GADEntity& entity);
	public:
		GADWorld(const std::string& name);
		~GADWorld();
	};

};
