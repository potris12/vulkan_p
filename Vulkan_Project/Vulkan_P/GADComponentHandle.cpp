#include "GADComponentHandle.h"

using namespace GADBased;


template<class ComponentType>
void GADBased::GADComponentHandle<ComponentType>::destroy()
{
	mgr->destroy(owner);
}

template<class ComponentType>
GADBased::GADComponentHandle<ComponentType>::GADComponentHandle(GADComponentManager<ComponentType>* manager, ComponentInstance inst, GADEntity e)
{
}


template<class ComponentType>
GADComponentHandle<ComponentType>::~GADComponentHandle()
{
}
