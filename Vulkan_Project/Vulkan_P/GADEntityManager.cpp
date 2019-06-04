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
	for (auto entity : entities_) {
		entity.second.destroy();
	}
	entities_.clear();
}

GADEntity & GAD::GADEntityManager::addEntity(const std::string & name)
{
	entities_.insert(std::make_pair(entity_key_value_, std::move(GADEntity(name, entity_key_value_))));
	return entities_[entity_key_value_++];
}

void GAD::GADEntityManager::removeEntity(const GADEntity & entity)
{
	const auto& entity_iter = entities_.find(entity.key_);
	if (entity_iter != entities_.end()) {
		entities_.erase(entity_iter);
		//entity_key_value_--; key 값은 계속 증가만 하도록 함 
	}
}


GAD::GADEntityManager::GADEntityManager(const std::string & name) : GADBase(name)
{
}

GADEntityManager::~GADEntityManager()
{
}
