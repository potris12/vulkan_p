#pragma once

#include "GADBase.h"

namespace GAD {

	class BaseComponentManager : public GADBase {
	public:
		BaseComponentManager(const std::string& name) : GADBase(name) {};
		virtual ~BaseComponentManager() = default;
		BaseComponentManager(const BaseComponentManager &) = default;
		BaseComponentManager &operator=(const BaseComponentManager &) = default;
		BaseComponentManager(BaseComponentManager &&) = default;
		BaseComponentManager &operator=(BaseComponentManager &&) = default;
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
			return components_[component_num_++];
		}

		//void removeComponent(ComponentType& component);
		//void removeComponent(std::string& name);
		//
		//void forEash(std::function<void()> f);

	private:
		static const int64_t max_component_num_ = 1024;

		std::vector<ComponentType> components_;
		int64_t component_num_ = 0;

	public:
		GADComponentManager() : BaseComponentManager("component_manager") {
			components_.resize(max_component_num_);
		};
		GADComponentManager(const std::string& name) : BaseComponentManager(name) {
			components_.resize(max_component_num_);
		};
		~GADComponentManager() {};
	};

}
