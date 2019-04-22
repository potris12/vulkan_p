#pragma once

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

		template <class c>
		static int getComponentFamily() {
			return Component<typename std::remove_const<c>::type>::family();
		}
	public:
		GADComponent();
		~GADComponent();
	};

}
