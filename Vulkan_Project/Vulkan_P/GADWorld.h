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
			auto& component = component_manager->addComponent();
			return component;
			//return GADComponentHandle<ComponentType>(thiz, &component);

			//return 
		}
		void removeComponent(GADComponentBase& component) {
			component_managers_[component.family_]->removeComponent(component);
		}

		void removeEntity(const GADEntity& entity);


		//void addComponent(std::shared_ptr<GADComponentHandle> component);
		//GADComponentHandle & addComponent(std::shared_ptr<GADWorld> world, GADEntity& entity, GADComponentBase* component);
		
		//entity가 가지는 componet family index 
		std::map<int64_t, std::list<int64_t>> entity_components_;

		void removeEntityComponents(GADEntity& entity);
	public:
		GADWorld(const std::string& name);
		~GADWorld();
	};

};
