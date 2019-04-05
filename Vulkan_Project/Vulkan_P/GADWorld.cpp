#include "GADWorld.h"

using namespace GAD;

void GAD::GADWorld::awake()
{
	entity_manager_->awake();
	system_manager_->awake();
}

void GAD::GADWorld::start()
{
	entity_manager_->start();
	system_manager_->start();
}

void GAD::GADWorld::update()
{
	entity_manager_->update();
	system_manager_->update();
}

void GAD::GADWorld::destroy()
{
	entity_manager_->destroy();
	system_manager_->destroy();
}

GAD::GADWorld::GADWorld(std::string name) : GADBase(name)
{
}

GADWorld::~GADWorld()
{
}
