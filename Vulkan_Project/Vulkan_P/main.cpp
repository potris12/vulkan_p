#include "Framework.h"

#include "GADRenderSystem.h"

int main() {
	auto app = std::make_shared<Framework>();

	auto entityManager = std::make_unique<GADBased::GADEntityManager>();
	auto world = GADBased::GADWorld(entityManager);


	//add system
	std::unique_ptr<GADBased::GADSystem> renderSystem = std::make_unique<GADEngine::GADRenderSystem>();
	world.addSystem(renderSystem);

	//initialize game
	world.init();

	try {
		app->run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}