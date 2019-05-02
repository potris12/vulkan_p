#include "GADEntity.h"

using namespace GAD;


void GAD::GADEntity::awake()
{
}

void GAD::GADEntity::start()
{
}

void GAD::GADEntity::update()
{
}

void GAD::GADEntity::destroy()
{
}

template<class ComponentType>
inline std::shared_ptr<GADComponent<ComponentType>> GADEntity::addComponent(std::shared_ptr<GADWorld> world)
{
	return world->addComponent<ComponentType>();
}

GAD::GADEntity::GADEntity(std::string & name) : GADBase(name)
{
}

GADEntity::~GADEntity()
{
}
