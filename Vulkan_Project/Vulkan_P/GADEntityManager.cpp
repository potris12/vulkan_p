#include "stdafx.h"
#include "GADEntityManager.h"


using namespace GADBased;

GADEntity GADBased::GADEntityManager::createEntity()
{
	lastEntity++;

	return GADEntity(lastEntity);
}

void GADBased::GADEntityManager::destroy(GADEntity entity)
{
	// Do nothing for now. This will be covered in a future blog post.
}

GADEntityManager::GADEntityManager()
{
}


GADEntityManager::~GADEntityManager()
{
}
