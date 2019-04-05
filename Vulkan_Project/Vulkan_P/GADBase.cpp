#include "GADBase.h"

using namespace GAD;


const std::string& GAD::GADBase::getName()
{
	return name_;
}

GAD::GADBase::GADBase(const std::string & name) : name_(name)
{
}

GADBase::~GADBase()
{
}
