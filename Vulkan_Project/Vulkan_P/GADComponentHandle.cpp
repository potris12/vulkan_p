#include "GADComponentHandle.h"

using namespace GADBased;


template<class ComponentType>
void GADBased::GADComponentHandle<ComponentType>::destroy()
{
	mgr->destroy(owner);
}

template<class ComponentType>
GADBased::GADComponentHandle<ComponentType>::GADComponentHandle(GADEntity e, ComponentType* component, GADComponentManager<ComponentType>* manager)
{
	this->owner = e;
	this->component = component;
	this->manager = manager;
}


template<class ComponentType>
GADComponentHandle<ComponentType>::~GADComponentHandle()
{
}
