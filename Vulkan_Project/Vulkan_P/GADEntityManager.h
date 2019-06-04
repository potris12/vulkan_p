#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADEntity.h"

/*
entity manager는 entity만 관리하도록 함 
추가 삭제를 함 
*/
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
		int64_t entity_key_value_ = 0;
		std::map<const int64_t, GADEntity> entities_;
		//std::map<const int64_t, std::list<std::shared_ptr<GADComponentHandle>>> component_handles_;

	public:
		GADEntityManager(const std::string& name);
		~GADEntityManager();
	};

}
