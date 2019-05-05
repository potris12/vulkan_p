#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADEntity.h"

namespace GAD {
	class GADEntityManager : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

		GADEntity& addEntity(const std::string& name);
		void removeEntity(const GADEntity& entity);
	private:
		int64_t entity_index_ = 0;
		std::map<const int64_t, GADEntity> entities_;
	public:
		GADEntityManager(const std::string& name);
		~GADEntityManager();
	};

}
