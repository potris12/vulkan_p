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

	private:
		std::shared_ptr<GADWorld> world_;
		GADComponent<ComponentType>* component_;

	public:
		GADComponentHandle(const std::shared_ptr<GADWorld>& world, GADComponent<ComponentType>* component)
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