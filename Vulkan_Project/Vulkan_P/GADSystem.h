#pragma once

#include "stdafx.h"
#include "GADBase.h"

namespace GAD {
	class GADSystem : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

	private:

	public:
		GADSystem(const std::string& name);
		~GADSystem();
	};

}
