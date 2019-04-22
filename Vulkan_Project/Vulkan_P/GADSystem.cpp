#include "stdafx.h"
#include "GADSystem.h"

using namespace GADBased;


void GADSystem::registerWorld(GADWorld* world){
	parentWorld = world;
}

void GADSystem::registerEntity(const GADEntity &entity) {
	registeredEntities.push_back(entity);
}

void GADSystem::unRegisterEntity(const GADEntity &entity) {
	for (auto it = registeredEntities.begin(); it != registeredEntities.end(); ++it) {
		GADEntity e = *it;
		if (e.getID() == entity.getID()) {
			registeredEntities.erase(it);
		}
	}
}

GADComponentMask GADBased::GADSystem::getSignature()
{
	return signature;
}
