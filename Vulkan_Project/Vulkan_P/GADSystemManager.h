#pragma once

#include "stdafx.h"
#include "GADHelper.h"
#include "GADBase.h"
#include "GADSystem.h"

namespace GAD {
	class GADSystemManager : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

		void addSystem(std::shared_ptr<GADSystem> system);

		void removeSystem(std::shared_ptr<GADSystem> system);
		void removeSystem(std::string& name);

		std::shared_ptr<GADSystem> getSystem(std::string& name);
		
		template <class T>
		std::shared_ptr<T> getSystem() {
			for (auto system : systems_) {
				if (auto return_system = dynamic_cast<T>(system)) {
					return return_system;
				}
			}
			return nullptr;
		}

	private:
		std::vector<std::shared_ptr<GADSystem>> systems_;

	public:
		GADSystemManager(const std::string& name);
		~GADSystemManager();
	};


}