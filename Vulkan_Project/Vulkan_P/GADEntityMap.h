#pragma once

#include "GADEntity.h"
#include <array>
#include <map>

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
		std::array<GADEntity, 1024> instanceToEntity;
		
	public:
		GADEntityMap();
		~GADEntityMap();
	};

}
