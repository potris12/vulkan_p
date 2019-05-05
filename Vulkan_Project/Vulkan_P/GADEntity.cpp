#include "GADEntity.h"
#include "GADWorld.h"

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

GAD::GADEntity::GADEntity() : GADBase("entity"), key_(-1)
{

}

GAD::GADEntity::GADEntity(const std::string & name, const int64_t key) : GADBase(name), key_(key)
{
}

GADEntity::~GADEntity()
{
}
