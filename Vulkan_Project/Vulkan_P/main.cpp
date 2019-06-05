//#include "Framework.h"
#include "GADWorld.h"
#include "TestComponent.h"
#include "GADEntityHandle.h"
#include "TestRenderComponent.h"

/*
TODO.
component���� �����̳� ����ȭ �����̳ʷ� �����ؾ��� 
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
		/*�޸� ����
		��ȿ�� �˻� ���̾ Ȱ��ȭ �� ���¿��� ���� ���α׷��� �����ϰ� �������α׷��� �����ϰ� �������α׷��� �޸� ��뷮�� ����͸��ϸ�
		�������α׷��� ������ �����ϴ°��� �� �� ���� �� ������ ��ȿ�� �˻� ���̾� ������ �������α׷��� gpu�� ��������� ����ȭ�Ǳ⸦ ����ϱ� ������
		��������� �ʼ��� �ƴ����� �� �� �������� �����ϴ� ���� ���ɿ� ū ������ ��ġ���� ����
		���� ������ �׸��⸦ �����ϱ� ���� ������ ���̼��� �����⸦ ��������� ��ٸ����ν� �̸� �ذ� �� ������ */

		vkDeviceWaitIdle(DEVICE_MANAGER->getDevice());
		/* vkQueueWaitIdle�� ����Ͽ� Ư�� ��� ��⿭�� �۾��� �Ϸ�� ������ ��ٸ� ���� �Ӥ���
		�� ��ɵ��� ����ȭ�� �����ϴ� �ſ� �������� ������� ���� �� ���� */
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