#pragma once

#include "GADWorld.h"
#include "GADEntity.h"

namespace GAD {
	class GADEntityHandle
	{

		friend class GADWorld;
		friend class GADEntity;

	public:
		template<class ComponentType>
		ComponentType& addComponent() {
			auto& component = world_->addComponent<ComponentType>();
			entity_.addComponent(&component);
			return component;
		}

		void removeEntity();
		
	private:
		std::shared_ptr<GADWorld> world_;
		GADEntity& entity_;

	public:
		GADEntityHandle(const std::shared_ptr<GADWorld>& world, GADEntity& entity);
		~GADEntityHandle();
	};


}
