#pragma once

#include "stdafx.h"
#include "GADBase.h"

namespace GAD {
	class GADComponent : public GADBase
	{
	public:
		void awake();
		void start();
		void update();
		void destroy();

	public:
		GADComponent(const std::string& name);
		~GADComponent();
	};

}
