#include "Framework.h"

void Framework::awake()
{
	TIMER->awake();
	DEVICE_MANAGER->awake();
	RENDERER->awake();
	INPUTMANAGER->awake();
}

void Framework::update()
{
	while (!glfwWindowShouldClose(DEVICE_MANAGER->getWindow())) {
		glfwPollEvents();

		INPUTMANAGER->processInput();

		TIMER->update();
		RENDERER->update();//TODO UPDATE���� �� �ش� ��ü�� �Ѱܾ���
		RENDERER->drawFrame();


		//esc = exit
		if (INPUTMANAGER->keyPressed(GLFW_KEY_ESCAPE))
		{
			break;
		}
		INPUTMANAGER->update();
	}
	/*�޸� ����
	��ȿ�� �˻� ���̾ Ȱ��ȭ �� ���¿��� ���� ���α׷��� �����ϰ� �������α׷��� �����ϰ� �������α׷��� �޸� ��뷮�� ����͸��ϸ�
	�������α׷��� ������ �����ϴ°��� �� �� ���� �� ������ ��ȿ�� �˻� ���̾� ������ �������α׷��� gpu�� ��������� ����ȭ�Ǳ⸦ ����ϱ� ������
	��������� �ʼ��� �ƴ����� �� �� �������� �����ϴ� ���� ���ɿ� ū ������ ��ġ���� ����
	���� ������ �׸��⸦ �����ϱ� ���� ������ ���̼��� �����⸦ ��������� ��ٸ����ν� �̸� �ذ� �� ������ */

	vkDeviceWaitIdle(DEVICE_MANAGER->getDevice());
	/* vkQueueWaitIdle�� ����Ͽ� Ư�� ��� ��⿭�� �۾��� �Ϸ�� ������ ��ٸ� ���� �Ӥ���
	�� ��ɵ��� ����ȭ�� �����ϴ� �ſ� �������� ������� ���� �� ���� */
}

void Framework::destroy()
{	
	TIMER->destroy();
	RENDERER->destroy();
	DEVICE_MANAGER->destroy();
}