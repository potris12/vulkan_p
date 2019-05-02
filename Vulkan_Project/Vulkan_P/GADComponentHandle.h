#pragma once

#include "GADEntity.h"
#include "GADComponentManager.h"

namespace GADBased {

	template<class ComponentType>
	class GADComponentHandle
	{
		//using ExposedComponentType = GADComponentManager<ComponentType>::LookupType;

	public:
		void destroy();
		ComponentType* operator->() const { return component; }
	private:
		GADEntity owner;
		ComponentType * component;
		GADComponentManager<ComponentType>* mgr;

	public:
		GADComponentHandle() {};
		GADComponentHandle(GADEntity e, ComponentType* component, GADComponentManager<ComponentType>* manager);
		~GADComponentHandle();
	};


}
