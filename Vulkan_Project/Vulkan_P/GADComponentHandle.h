#pragma once

#include "GADEntity.h"
#include "GADComponentManager.h"

namespace GADBased {

	template<class ComponentType>
	class GADComponentHandle
	{
		using ExposedComponentType = typename = ComponentManager<ComponentType>::LookypType;

	public:
		void destroy();
		ExposedComponentType* operator->() const { return component; }
	private:
		GADEntity owner;
		ExposedComponentType * component;
		ComponentManager<ComponentType>* mgr;

	public:
		GADComponentHandle() {};
		GADComponentHandle(ComponentManager<ComponentType>* manager, ComponentInstance inst, Entity e);
		~GADComponentHandle();
	};


}
