#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADComponent.h"


namespace GAD {
	class GADWorld;

	class GADEntity : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

		template <class ComponentType>
		std::shared_ptr<GADComponent<ComponentType>> addComponent<ComponentType>(std::shared_ptr<GADWorld> world);
		template <class ComponentType>
		void removeComponent<ComponentType>();

	private:


	public:
		GADEntity(std::string& name);
		~GADEntity();
	};

	template<class ComponentType>
	inline void GADEntity::removeComponent()
	{
	}

}

