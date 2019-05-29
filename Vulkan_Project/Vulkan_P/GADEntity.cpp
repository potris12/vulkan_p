#include "GADEntity.h"
#include "GADWorld.h"
#include "GADComponentHandle.h"

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
	//components_.clear();
}


//void GAD::GADEntity::addComponent(std::shared_ptr<GADComponentHandle> component)
//{
//	components_.push_back(component);
//}

GAD::GADEntity::GADEntity() : GADBase("entity"), key_(-1)
{

}

GAD::GADEntity::GADEntity(const std::string & name, const int64_t key) : GADBase(name), key_(key)
{
}

GADEntity::~GADEntity()
{
}
