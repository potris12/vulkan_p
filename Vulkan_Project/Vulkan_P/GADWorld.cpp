#include "GADWorld.h"
#include "GADEntityHandle.h"

using namespace GAD;

void GAD::GADWorld::awake()
{
	entity_manager_->awake();
	for (auto& component_manager : component_managers_) {
		component_manager.second->awake();
	}
	system_manager_->awake();
}

void GAD::GADWorld::start()
{
	entity_manager_->start();
	for (auto& component_manager : component_managers_) {
		component_manager.second->start();
	}
	system_manager_->start();
}

void GAD::GADWorld::update()
{
	system_manager_->update();
}

void GAD::GADWorld::destroy()
{
	system_manager_->destroy();
	for (auto& component_manager : component_managers_) {
		component_manager.second->destroy();
	}
	entity_manager_->destroy();
}

GADEntityHandle GAD::GADWorld::addEntity(const std::string& name)
{
	static auto thiz = std::static_pointer_cast<GADWorld>(shared_from_this());
	return { thiz, entity_manager_->addEntity(name) };
}

void GAD::GADWorld::removeEntity(const GADEntity& entity)
{
	//component Á¤¸® 
	entity_manager_->removeEntity(entity);
}

GAD::GADWorld::GADWorld(const std::string& name) : GADBase(name)
{
	entity_manager_ = std::make_unique<GADEntityManager>(name_ + "_entity_manager");
	system_manager_ = std::make_unique<GADSystemManager>(name_ + "_system_manager");
}

GADWorld::~GADWorld()
{
}
