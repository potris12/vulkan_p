#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADEntityManager.h"
#include "GADSystemManager.h"
#include "GADComponentManager.h"
#include "GADComponent.h"

namespace GAD {

	class GADEntityHandle;

	class GADWorld : public GADBase
	{
		friend class GADEntityHandle;
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

		template<class ComponentType>
		ComponentType& addComponent() {
			auto component_manager = getComponentManager<ComponentType>();

			return component_manager->addComponent();
		}

		void removeEntity(const GADEntity& entity);

	public:
		GADWorld(const std::string& name);
		~GADWorld();
	};

};
