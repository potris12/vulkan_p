#pragma once

#include "GADEntity.h"

namespace GADBased{

	using ComponentInstance = unsigned int;

	class GADEntityMap
	{
	public:
		GADEntity getEntity(ComponentInstance instance);
		ComponentInstance getInstance(GADEntity entity);

		void add(GADEntity entity, ComponentInstance instance);
		void update(GADEntity entity, ComponentInstance instance);
		void remove(GADEntity entity);

	private:
		std::map<GADEntity, ComponentInstance> entityToInstance;
		std::array<GADEntity, MAX_COMPONETMANAGER_MANAGED_COMPONENT_NUM> instanceToEntity;
		
	public:
		GADEntityMap();
		~GADEntityMap();
	};

}
