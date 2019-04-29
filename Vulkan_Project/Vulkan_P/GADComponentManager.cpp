#include "stdafx.h"
#include "GADComponentManager.h"

using namespace GADBased;

template<class ComponentType>
ComponentInstance GADBased::GADComponentManager<ComponentType>::addComponent(GADEntity e, ComponentType & c)
{
	ComponentInstance newInstance = { componentData.size };
	componentData.data[newInstance] = c;
	entityMap.add(entity, newInstance);
	componentData.size++;
	return newInstance;
}

template<class ComponentType>
ComponentType GADBased::GADComponentManager<ComponentType>::lookup(GADEntity e)
{
	try
	{
		ComponentInstance inst = entityMap.at(e);
		return GADComponentHandle<ComponentType>(this, inst, e);
	}
	catch (std::exception e)
	{
		return nullptr;
	}
}

template<class ComponentType>
GADComponent<ComponentType> * GADBased::GADComponentManager<ComponentType>::lookupComponent(GADEntity e)
{
	try
	{
		ComponentInstance inst = entityMap.getInstance(e);
		return &componentData.data.at(inst);
	}
	catch (std::exception e)
	{
		return nullptr;
	}
}

template<class ComponentType>
void GADBased::GADComponentManager<ComponentType>::destroyComponent(GADEntity e)
{
	ComponentInstance inst = 0;
	try
	{
		ComponentInstance inst = entityMap.getInstance(e);
		return &componentData.data[inst.index];
	}
	catch (std::exception excep)
	{
		return;
	}
	
	ComponentInstance lastComponent = componentData.size - 1;
	componentData[inst] = componentData.data[lastComponent];
	GADEntity lastEntity = entityMap.getEntity(lastComponent);

	entityMap.remove(entity);
	entityMap.update(lastEntity, instance);

	componentData.size--;
}

template<class ComponentType>
void GADBased::GADComponentManager<ComponentType>::iterateAll(std::is_function<void(ComponentType component)> lambda)
{
	for (auto i = 0; i < componentData.size; ++i)
	{
		lambda(componentData.data[i]);
	}
}

template<class ComponentType>
GADEntity GADBased::GADComponentManager<ComponentType>::getEntityByComponentInstance(ComponentInstance inst)
{
	for (auto pair : entityMap)
	{
		if (pair.second == inst)
			return pair.first;
	}

	return GADEntity(0);
}

template<class ComponentType>
GADComponentManager<ComponentType>::GADComponentManager()
{
	componentData.data = static_cast<std::array<ComponentType, MAX_COMPONETMANAGER_MANAGED_COMPONENT_NUM> *>(malloc(sizeof(ComponentType) * MAX_COMPONETMANAGER_MANAGED_COMPONENT_NUM));
}

template<class ComponentType>
GADComponentManager<ComponentType>::~GADComponentManager()
{

}

