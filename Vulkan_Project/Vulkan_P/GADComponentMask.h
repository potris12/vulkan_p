#pragma once

#include "GADComponent.h"

namespace GADBased {

	class GADComponentMask
	{
	public:
		void setMask(unsigned int mask);
		unsigned int getMask();

		template<class ComponentType>
		void addComponent() {
			mask |= (1 << GetComponentFamily<ComponentType>());
		}

		template<class ComponentType>
		void removeComponent() {
			mask &= ~(1 << GetComponentFamily<ComponentType>());
		}

		bool isNewMatch(GADComponentMask oldMask, GADComponentMask systemMask);
		bool isNoLongerMatched(GADComponentMask oldMask, GADComponentMask systemMask);
		bool matches(GADComponentMask systemMask);
	private:
		unsigned int mask_ = 0;

	public:
		GADComponentMask();
		~GADComponentMask();
	};


}
