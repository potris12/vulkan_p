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
			mask_ |= (1 << GADComponent<ComponentType>::getComponentFamily());
		}

		template<class ComponentType>
		void removeComponent() {
			mask_ &= ~(1 << GADComponent<ComponentType>::getComponentFamily());
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
