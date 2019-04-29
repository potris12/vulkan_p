#include "stdafx.h"
#include "GADEntity.h"


using namespace GADBased;

void GADBased::GADEntity::setID(int id)
{
	id_ = id;
}

const int GADBased::GADEntity::getID()  const
{
	return id_;
}

GADEntity::GADEntity(unsigned int id) : id_(id)
{
}


GADEntity::~GADEntity()
{
}

bool GADBased::operator<(const GADEntity & l, const GADEntity & r)
{
	return l.id_ < r.id_;
}
