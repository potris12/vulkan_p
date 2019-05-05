#pragma once

#include "stdafx.h"

namespace GAD {
	class GADBase : public std::enable_shared_from_this<GADBase>
	{
	public:
		virtual void awake() = 0;
		virtual void start() = 0;
		virtual void update() = 0;
		virtual void destroy() = 0;

		const std::string& getName();

	protected:
		const std::string name_;

	public:
		GADBase(const std::string& name);
		~GADBase();
	};
}
