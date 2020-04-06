#pragma once

#include <type_traits>

namespace GADBased {

	struct ComponentCounter {
		static int familyCounter;
	};

	template<class ComponentType>
	class GADComponent
	{
	public:
		static inline int family() {
			static int family = ComponentCounter::familyCounter++;
			return family;
		}

	public:
		GADComponent();
		~GADComponent();
	};
}