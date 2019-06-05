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

			//TODO �̺κ� ��� ���� äũ�ؾ��� component�� ���� ���� entity�� ��ġ�� ������ ������ �ʿ䰡 ����
			//if (false == enable_) return GADComponentHandle();
			//assert(false == world_->hasComponent<ComponentType>());
			if (world_->hasComponent<ComponentType>(entity_)) return { world_, nullptr };

			GAD_LOGD("ADD component family : " + std::to_string(ComponentType::family()));

			auto& component = world_->addComponent<ComponentType>(entity_);
			
			//entity_.addComponent(component_handle);
			//entity_.addComponent(world_, &component);
			return { world_, &component };
		}

		template<class ComponentType>
		void removeComponent() {

			GAD_LOGD("REMOVE component family : " + std::to_string(ComponentType::family()));

			world_->removeComponent<ComponentType>(entity_);
		}

		template<class ComponentType>
		void removeComponent(GADComponentHandle<ComponentType> handle) {

			GAD_LOGD("REMOVE component family : " + std::to_string(ComponentType::family()));

			world_->removeComponent(entity_, handle->family());
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
