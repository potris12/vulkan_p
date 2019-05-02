#pragma once

#include "GADRenderComponent.h"

namespace GADEngine
{
	class GADRenderSystem : public nomad::System
	{
	public:
		GADRenderSystem();

		void update(int dt);

	public:
		//GADRenderSystem();
		~GADRenderSystem();
	};
}

