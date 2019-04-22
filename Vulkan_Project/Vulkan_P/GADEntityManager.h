#pragma once

#include "GADEntity.h"

namespace GADBased
{


	/*
	¸ñÀû 
	 - Entity player = entityManager.create();

	 HealthCOmponent hp;
	 hp.currentHP = 7;
	 hp.maxHP = 10;

	 helthComponentManager->addComponent(player, hp);
	*/
	class GADEntityManager
	{
	public:
		GADEntity createEntity();
		void destroy(GADEntity entity);

	private:
		int lastEntity = 0;

	public:
		GADEntityManager();
		~GADEntityManager();
	};


}

