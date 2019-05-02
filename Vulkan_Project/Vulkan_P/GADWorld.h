#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADEntityManager.h"
#include "GADSystemManager.h"
#include "GADComponentManager.h"

namespace GAD {
	class GADWorld : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

		template<class ComponentType>
		GADComponent<ComponentType> addComponent();
	private:
		std::unique_ptr<GADEntityManager> entity_manager_;
		std::map<int64_t, std::unique_ptr<BaseComponentManager>> component_managers_;
		std::unique_ptr<GADSystemManager> system_manager_;

	public:
		GADWorld(std::string name);
		~GADWorld();
	};

};
