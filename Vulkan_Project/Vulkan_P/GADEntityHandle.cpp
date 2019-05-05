#include "GADEntityHandle.h"

using namespace GAD;


void GAD::GADEntityHandle::removeEntity()
{
	world_->removeEntity(entity_);
}

GAD::GADEntityHandle::GADEntityHandle(const std::shared_ptr<GADWorld>& world, GADEntity& entity)
	: world_(world), entity_(entity)
{
}

GADEntityHandle::~GADEntityHandle()
{
}
