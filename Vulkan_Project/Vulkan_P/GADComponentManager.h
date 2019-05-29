#pragma once

#include "GADBase.h"
#include "GADComponent.h"

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
	};

	template<class ComponentType>
	class GADComponentManager : public BaseComponentManager
	{
	public:
		void awake() {};
		void start() {};
		void update() {};
		void destroy() {};

		ComponentType& addComponent() {
			component_body_indies_[component_num_] = component_num_;
			components_[component_num_].setKey(component_num_);
			return components_[component_num_++];
		}

		void removeComponent(ComponentType& component) {
			auto key = component.getKey();
			if (key < 0 || key >= component_num_) return;

			auto tmp_component = component_body_indies_[key];
			component_body_indies_[key] = component_body_indies_[component_num_ - 1];
			component_body_indies_[component_num_ - 1] = tmp_component;
			component_num_--;
		}

		void removeComponent(GADComponentBase& component) {
			auto key = component.getKey();
			if (key < 0 || key >= component_num_) return;

			auto tmp_component = component_body_indies_[key];
			component_body_indies_[key] = component_body_indies_[component_num_ - 1];
			component_body_indies_[component_num_ - 1] = tmp_component;
			component_num_--;
		}
		//void removeComponent(std::string& name);
		//
		void forEash(std::function<void(ComponentType&)> f) {
			for (auto i = 0; i < component_num_; ++i) {
				f(components_[i]);
			}
		}

	private:
		using component_body_index = int64_t;

		static const int64_t max_component_num_ = 1024;

		std::vector<ComponentType> components_;
		std::vector<component_body_index> component_body_indies_;

		using entity_key = int64_t;
		std::map<entity_key, component_body_index> entity_map_;
		int64_t component_num_ = 0;

	public:
		GADComponentManager() : BaseComponentManager("component_manager") {
			components_.resize(max_component_num_);
			component_body_indies_.resize(max_component_num_);

		};
		GADComponentManager(const std::string& name) : BaseComponentManager(name) {
			components_.resize(max_component_num_);
			component_body_indies_.resize(max_component_num_);

		};
		~GADComponentManager() {};
	};

}
