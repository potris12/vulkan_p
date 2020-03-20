#include "world.h"
#include <iostream>
#include "entityHandle.h"

namespace nomad {
World::World(std::unique_ptr<EntityManager> entityManager) : entityManager(std::move(entityManager)) {}

void World::init() {
  for (auto &system : systems) {
    system->init();
  }
}

void World::update(float dt) {
  // TODO(taurheim) check to make sure that the world has called init
  for (auto &system : systems) {
    system->update(dt);
  }
}

void World::render() {
  for (auto &system : systems) {
    system->render();
  }
}

EntityHandle World::createEntity() { return {entityManager->createEntity(), this}; }

void World::destroyEntity(nomad::Entity entity) {
  for (auto &system : systems) {
    system->unRegisterEntity(entity);
  }

  entityManager->destroy(entity);
}

void World::addSystem(std::unique_ptr<nomad::System> system) {
  system->registerWorld(this);
  systems.push_back(std::move(system));
}

void World::updateEntityMask(nomad::Entity const &entity, nomad::ComponentMask oldMask) {
  ComponentMask newMask = entityMasks[entity];

  for (auto &system : systems) {
    ComponentMask systemSignature = system->getSignature();
    if (newMask.isNewMatch(oldMask, systemSignature)) {
      // We match but didn't match before
      system->registerEntity(entity);
    } else if (newMask.isNoLongerMatched(oldMask, systemSignature)) {
      system->unRegisterEntity(entity);
    }
  }
}
}  // namespace nomad
