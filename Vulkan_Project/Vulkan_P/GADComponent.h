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
		void awake() {};
		void start() {};
		void update() {};
		void destroy() {};

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


		static int64_t family()
		{
			static int64_t family = ComponentCounter::familyCounter++;
			return family;
		}

	public:
		GADComponent() : GADComponentBase("component") {};
		GADComponent(const std::string& name) : GADComponentBase(name) {};
		virtual ~GADComponent() {};
	};
}
