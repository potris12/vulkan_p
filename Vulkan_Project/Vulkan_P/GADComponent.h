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
		const int64_t family_ = -1;

		void setKey(int64_t key) { key_ = key; }
		int64_t getKey() { return key_; }

	private:
		int64_t key_ = -1;

	public:
		void awake() {};
		void start() {};
		void update() {};
		void destroy() {};

		GADComponentBase(const std::string& name, int64_t family)
			: GADBase(name), family_(family) {};
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


		inline static int64_t family()
		{
			static int64_t family = ComponentCounter::familyCounter++;
			return family;
		}

	public:
		GADComponent() : GADComponentBase("component") {
			
		};
		GADComponent(const std::string& name) : GADComponentBase(name, family()) {};
		virtual ~GADComponent() {};
	};
}
