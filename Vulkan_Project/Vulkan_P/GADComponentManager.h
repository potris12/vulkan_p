#pragma once

#include "GADBase.h"
#include "GADComponent.h"
#include "GADEntity.h"

namespace GAD {

	class BaseComponentManager : public GADBase {
	public:
		BaseComponentManager(const std::string& name) : GADBase(name) {};
		virtual ~BaseComponentManager() = default;
		BaseComponentManager(const BaseComponentManager &) = default;
		BaseComponentManager &operator=(const BaseComponentManager &) = default;
		BaseComponentManager(BaseComponentManager &&) = default;
		BaseComponentManager &operator=(BaseComponentManager &&) = default;

		virtual void removeComponent(GADComponentBase& component) {};
		virtual void removeComponent(GADEntity& entity) {};
	};

	template<class ComponentType>
	class GADComponentManager : public BaseComponentManager
	{
	public:
		void awake() {};
		void start() {};
		void update() {};
		void destroy() {};

		ComponentType& addComponent(GADEntity& entity) {
			
			entity_map_.insert({ entity.key_, ComponentType{} });

			GAD_LOGD("component family : " + std::to_string(ComponentType::family()) + "ADD component ");

			return entity_map_[entity.key_];
		}

		ComponentType& getComponent(GADEntity& entity) {
			return entity_map_[entity.key_];
		}

		void removeComponent(GADEntity& entity, ComponentType& component) {
			auto iter = entity_map_.find(entity.key_);
			if (iter != entity_map_.end()) {
				entity_map_.erase(iter);
			}
		}

		void removeComponent(GADEntity& entity) {
			auto component = entity_map_[entity.key_];
			removeComponent(entity, component);
		}

		void forEach(std::function<void(ComponentType&)> f) {

			for (auto component : entity_map_) {
				f(component.second);
			}
		}

	private:
		/* entity에 대응되는 component!  */
		using entity_key = const int64_t;
		/* 재활용? x 그냥 만들고 지우자 */
		std::map<entity_key, ComponentType> entity_map_;
		
	public:
		GADComponentManager() : BaseComponentManager("component_manager") {
			
		};
		GADComponentManager(const std::string& name) : BaseComponentManager(name) {
			
		};
		~GADComponentManager() {};
	};

}
