#include "stdafx.h"

#include "GADRenderSystem.h"
#include "GADRenderComponent.h"

using namespace GADEngine;

GADEngine::GADRenderSystem::GADRenderSystem()
{
	signature.addComponent<GADRenderComponent>();
}

void GADEngine::GADRenderSystem::update(int dt)
{
	for (auto & entity : registeredEntities) {
		nomad::ComponentHandle<GADRenderComponent> renderComponent;
		parentWorld->unpack(entity, renderComponent);
		renderComponent->drawFrame();
	}
}

GADRenderSystem::~GADRenderSystem()
{
}
