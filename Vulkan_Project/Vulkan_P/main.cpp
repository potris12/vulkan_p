#include "Framework.h"

#include "GADRenderSystem.h"

int main() {
	DEVICE_MANAGER->awake();
	//auto app = std::make_shared<Framework>();

	auto entityManager = std::make_unique<nomad::EntityManager>();
	auto world = std::make_unique<nomad::World>(std::move(entityManager));


	//add system
	std::unique_ptr<nomad::System> renderSystem = (std::make_unique<GADEngine::GADRenderSystem>());
	world->addSystem(std::move(renderSystem));

	//initialize game
	world->init();

	// Add an entity with a position
	auto tumbleweed = world->createEntity();
	auto render_component = GADEngine::GADRenderComponent();
	render_component.awake();
	tumbleweed.addComponent(render_component);

	try {

		float fTimeElapced = 0.f;

		while (true) {
			std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

			world->update(fTimeElapced);

			std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
			fTimeElapced = sec.count();
		}
		//app->run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	DEVICE_MANAGER->destroy();
	return EXIT_SUCCESS;
}