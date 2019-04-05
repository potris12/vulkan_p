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

	private:
		std::map<GAD_KEY, std::shared_ptr<GADEntity>> entitys_;

	public:
		GADEntityManager(std::string& name);
		~GADEntityManager();
	};

}
