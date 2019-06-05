//#include "Framework.h"
#include "GADWorld.h"
#include "TestComponent.h"
#include "GADEntityHandle.h"
#include "TestRenderComponent.h"

/*
TODO.
component관리 컨테이너 직렬화 컨테이너로 변경해야함 
*/
#include "GADComponentManager.h"
int main() {
	
	DEVICE_MANAGER->awake();

	auto world = std::make_shared<GAD::GADWorld>("test_world");
	world->awake();
	world->start();

	auto entity_handle = world->addEntity("test");
	auto component_handle = entity_handle.addComponent<GADEngine::TestRenderComponent>();
	component_handle->awake();
	
	
	//auto app = std::make_shared<Framework>();

	try {
		while (!glfwWindowShouldClose(DEVICE_MANAGER->getWindow())) {
			glfwPollEvents();
			component_handle->drawFrame();
		}
		/*메모리 누수
		유효성 검사 레이어가 활성화 된 상태에서 응용 프로그램을 실행하고 응용프로그램을 실행하고 응용프로그램의 메모리 사용량을 모니터링하면
		응용프로그램이 느리게 증가하는것을 할 수 있음 그 이유는 유효성 검사 레이어 구현이 응용프로그램이 gpu와 명시적으로 동기화되기를 기대하기 때문임
		기술적으로 필수는 아니지만 한 번 프레임을 수행하는 것이 성능에 큰 영향을 미치지는 않음
		다음 프레임 그리기를 시작하기 전에 프레젠 테이션이 끝나기를 명시적으로 기다림으로써 이를 해결 할 수있음 */

		vkDeviceWaitIdle(DEVICE_MANAGER->getDevice());
		/* vkQueueWaitIdle을 사용하여 특정 명령 대기열의 작업이 완료될 때까지 기다릴 수동 ㅣㅆ음
		이 기능들을 동기화를 수행하는 매우 기초적인 방법으로 사용될 수 있음 */
		//app->run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	component_handle->destroy();
	world->destroy();
	DEVICE_MANAGER->destroy();
	return EXIT_SUCCESS;
}