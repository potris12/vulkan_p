#include "GADComponentMask.h"

using namespace GADBased;

void GADBased::GADComponentMask::setMask(unsigned int mask)
{
	mask_ = mask;
}

unsigned int GADBased::GADComponentMask::getMask()
{
	return mask_;
}

bool GADBased::GADComponentMask::isNewMatch(GADComponentMask oldMask, GADComponentMask systemMask)
{
	return matches(systemMask) && !oldMask.matches(systemMask);
}

bool GADBased::GADComponentMask::isNoLongerMatched(GADComponentMask oldMask, GADComponentMask systemMask)
{
	return oldMask.matches(systemMask) && !matches(systemMask);
}

bool GADBased::GADComponentMask::matches(GADComponentMask systemMask)
{
	return ((mask_ & systemMask.mask_) == systemMask.mask_);
}

GADComponentMask::GADComponentMask()
{
}


GADComponentMask::~GADComponentMask()
{
}
