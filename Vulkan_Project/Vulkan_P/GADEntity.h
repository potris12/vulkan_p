#pragma once

#include "stdafx.h"
#include "GADBase.h"
//#include "GADComponent.h"

namespace GAD {
	class GADWorld;
	class GADComponentBase;
	//class GADEntityHandle;
	//class GADComponentHandle;

	class GADEntity : public GADBase
	{
		friend class GADEntityHandle;
	public:
		const int64_t key_ = 0;

		void awake();
		void start();
		void update();
		void destroy();

	private:

	public:
		GADEntity();
		GADEntity(const std::string& name, const int64_t key_);

		~GADEntity();
	};

}

