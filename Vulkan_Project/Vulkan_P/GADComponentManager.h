#pragma once

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
	};

	template<class ComponentType>
	class GADComponentManager : public BaseComponentManager
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();


		void addComponent(std::shared_ptr<GADComponent<ComponentType>> component);

		void removeComponent(std::shared_ptr<GADComponent<ComponentType>> component);
		void removeComponent(std::string& name);

		void forEash(std::function<void()> f);

	private:
		std::vector<GADComponent<ComponentType>> components_;

	public:
		GADComponentManager(const std::string& name);
		~GADComponentManager();
	};

}
