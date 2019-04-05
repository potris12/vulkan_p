#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADComponent.h"

namespace GAD {
	class GADEntity : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

	private:
		

	public:
		GADEntity(std::string& name);
		~GADEntity();
	};

}

