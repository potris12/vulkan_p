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
	for (auto entity : entitys_)
	{
		entity.second->update();
	}
}

void GAD::GADEntityManager::destroy()
{
}

GAD::GADEntityManager::GADEntityManager(std::string & name) : GADBase(name)
{
}

GADEntityManager::~GADEntityManager()
{
}
