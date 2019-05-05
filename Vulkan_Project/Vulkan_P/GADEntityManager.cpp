#include "stdafx.h"
#include "GADEntityManager.h"

using namespace GAD;

void GAD::GADEntityManager::awake()
{
}

void GAD::GADEntityManager::start()
{
}

void GAD::GADEntityManager::update()
{
	for (auto entity : entities_)
	{
		entity.second.update();
	}
}

void GAD::GADEntityManager::destroy()
{
	entities_.clear();
}

GADEntity & GAD::GADEntityManager::addEntity(const std::string & name)
{
	entities_.insert(std::make_pair(entity_index_, GADEntity(name, entity_index_)));
	return entities_[entity_index_++];
}

void GAD::GADEntityManager::removeEntity(const GADEntity & entity)
{
	const auto& entity_iter = entities_.find(entity.key_);
	if (entity_iter != entities_.end()) {
		entities_.erase(entity_iter);
	}
}


GAD::GADEntityManager::GADEntityManager(const std::string & name) : GADBase(name)
{
}

GADEntityManager::~GADEntityManager()
{
}
