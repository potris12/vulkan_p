#include "stdafx.h"
#include "GADEntityMap.h"

using namespace GADBased;

GADEntity GADBased::GADEntityMap::getEntity(ComponentInstance instance)
{
	try {
		return instanceToEntity.at(instance);
	}
	catch (std::exception ex) {
		return GADEntity(-1);
	}
	
}

ComponentInstance GADBased::GADEntityMap::getInstance(GADEntity entity)
{
	try {
		return entityToInstance.at(entity);
	}
	catch (std::exception ex) {
		return ComponentInstance{};
	}
}

void GADBased::GADEntityMap::add(GADEntity entity, ComponentInstance instance)
{
	if (entityToInstance.end() == entityToInstance.find(entity)) {
		std::cout << "error this entity already here";
	}

	//TODO Error when called on an entity already in the map
	entityToInstance.insert({ entity, instance });
	instanceToEntity.at(instance) = entity;
}

void GADBased::GADEntityMap::update(GADEntity entity, ComponentInstance instance)
{
	entityToInstance.at(entity) = instance;
	instanceToEntity.at(instance) = entity;
}

void GADBased::GADEntityMap::remove(GADEntity entity)
{
	entityToInstance.erase(entity);
	// why do not delate instanceToEntity entity
}

//GADEntityMap::GADEntityMap()
//{
//}


GADBased::GADEntityMap::GADEntityMap()
{
	//instanceToEntity = std::array<GADEntity, MAX_COMPONETMANAGER_MANAGED_COMPONENT_NUM>();
}

GADEntityMap::~GADEntityMap()
{
}
