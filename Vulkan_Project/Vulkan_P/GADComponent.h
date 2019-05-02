#pragma once

#include "stdafx.h"
#include "GADBase.h"

namespace GAD {
	
	struct ComponentCounter {
		static int64_t familyCounter;
	};

	class GADComponentBase : public GADBase 
	{
		
	public:
		void awake() = 0;
		void start() = 0;
		void update() = 0;
		void destroy() = 0;

		GADComponentBase(const std::string& name) : GADBase(name) {};
		~GADComponentBase() {};
	};

	template<class ComponentType>
	class GADComponent : public GADComponentBase
	{
	public:
		void awake() {};
		void start() {};
		void update() {};
		void destroy() {};


		static inline int64_t family()
		{
			static int64_t family = ComponentCounter::familyCounter++;
			return family;
		}

	public:
		GADComponent(const std::string& name) : GADBase(name) {};
		~GADComponent() {};
	};

}
