#include "Framework.h"
#include "GADWorld.h"
#include "TestComponent.h"
#include "GADEntityHandle.h"

#include "GADComponentManager.h"
int main() {
	
	auto world = std::make_shared<GAD::GADWorld>("test_world");
	world->awake();
	world->start();

	auto entity_handle = world->addEntity("test");
	auto component_handle = entity_handle.addComponent<TestComponent>();
	
	//world->addEntity("test1");
	//world->addEntity("test2");
	//world->addEntity("test3");
	//world->addEntity("test4");
	//
	////component_handle.remove();
	//
	//entity_handle.removeEntity();
	//component_handle = entity_handle.addComponent<TestComponent>();

	auto app = std::make_shared<Framework>();

	try {
		app->run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	world->destroy();
	return EXIT_SUCCESS;
}