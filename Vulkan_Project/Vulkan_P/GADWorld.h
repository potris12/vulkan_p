#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADEntityManager.h"
#include "GADSystemManager.h"

namespace GAD {
	class GADWorld : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

	private:
		std::unique_ptr<GADEntityManager> entity_manager_;
		std::unique_ptr<GADSystemManager> system_manager_;

	public:
		GADWorld(std::string name);
		~GADWorld();
	};

};
