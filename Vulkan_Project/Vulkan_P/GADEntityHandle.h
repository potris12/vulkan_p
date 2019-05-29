#pragma once

#include "GADWorld.h"
#include "GADEntity.h"
#include "GADComponentHandle.h"

namespace GAD {
	class GADEntityHandle
	{

		friend class GADWorld;
		friend class GADEntity;

	public:
		template<class ComponentType>
		GADComponentHandle<ComponentType> addComponent() {
			//TODO 이부분 어떻게 할지 채크해야함 
			//if (false == enable_) return GADComponentHandle();

			auto component = world_->addComponent<ComponentType>(entity_);
			auto component_handle = GADComponentHandle(world_, &component);
			//entity_.addComponent(component_handle);
			//entity_.addComponent(world_, &component);
			return component_handle;
		}

		void removeEntity();
		
	private:
		std::shared_ptr<GADWorld> world_;
		GADEntity& entity_;
		bool enable_ = false;

	public:
		GADEntityHandle(const std::shared_ptr<GADWorld>& world, GADEntity& entity);
		~GADEntityHandle();
	};


}
