#include "GADEntityHandle.h"

using namespace GAD;


void GAD::GADEntityHandle::removeEntity()
{
	if (false == enable_) return;

	enable_ = false;
	//components clear
	world_->removeEntityComponents(entity_);
	//for (auto component_handle : entity_.components_) {
	//	component_handle.remove();
	//}
	world_->removeEntity(entity_);
}

GAD::GADEntityHandle::GADEntityHandle(const std::shared_ptr<GADWorld>& world, GADEntity& entity)
	: world_(world), entity_(entity)
{
	enable_ = true;
}

GADEntityHandle::~GADEntityHandle()
{
}
