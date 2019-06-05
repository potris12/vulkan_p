#pragma once

#include "GADWorld.h"
#include "GADComponent.h"

namespace GAD{
	
	template<class ComponentType>
	class GADComponentHandle
	{
		friend class GADWorld;
		//friend class GADComponentBase;

	public:
		//void remove();
		ComponentType* operator->() {
			return component_;
		}
	private:
		std::shared_ptr<GADWorld> world_;
		ComponentType* component_;

	public:
		GADComponentHandle(const std::shared_ptr<GADWorld>& world, ComponentType* component)
			: world_(world), component_(component)
		{

		}
		~GADComponentHandle() {};
	};
}
/*

GADComponentHandle::GADComponentHandle(const std::shared_ptr<GADWorld>& world, GADComponentBase* component)
: world_(world), component_(component)
{
}
*/