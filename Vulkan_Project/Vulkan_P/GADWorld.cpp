#include "stdafx.h"
#include "GADWorld.h"
#include "GADEntityHandle.h"

using namespace GADBased;

GADBased::GADWorld::GADWorld(std::unique_ptr<GADEntityManager> entityManager) : entityManager(std::move(entityManager))
{
}

void GADBased::GADWorld::init()
{
	for (auto &system : systems) {
		system->init();
	}
}

void GADBased::GADWorld::update(float dt)
{
	for (auto &system : systems) {
		system->update(dt);
	}
}

void GADBased::GADWorld::render()
{
	for (auto &system : systems) {
		system->render();
	}
}

GADEntityHandle GADBased::GADWorld::createEntity()
{
	return { entityManager->createEntity(), this };
}

void GADBased::GADWorld::addSystem(std::unique_ptr<GADSystem> system)
{
	system->registerWorld(this);
	systems.push_back(std::move(system));
}

void GADBased::GADWorld::destroyEntity(GADEntity entity)
{
	for (auto& system : systems) {
		system->unRegisterEntity(entity);
	}
}

void GADBased::GADWorld::updateEntityMask(GADEntity const & entity, GADComponentMask oldMask)
{
	GADComponentMask newMask = entityMasks[entity];

	for (auto &system : systems) {
		GADComponentMask  systemSignature = system->getSignature();
		if (newMask.isNewMatch(oldMask, systemSignature)) {
			//We match but didn't match before'
			system->registerEntity(entity);
		}
		else if (newMask.isNoLongerMatched(oldMask, systemSignature)) {
			system->unRegisterEntity(entity);
		}
	}
}

GADBased::GADWorld::GADWorld()
{
}

GADBased::GADWorld::~GADWorld()
{
}
