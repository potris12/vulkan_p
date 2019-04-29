#pragma once

#include "GADRenderComponent.h"

namespace GADEngine
{
	class GADRenderSystem : public GADBased::GADSystem
	{
	public:
		GADRenderSystem() {
			signature.addComponent<GADRenderComponent>();
		}

		void update(int dt) {
			for (auto & entity : registeredEntities) {

			}
		}

	public:
		//GADRenderSystem();
		~GADRenderSystem();
	};
}

