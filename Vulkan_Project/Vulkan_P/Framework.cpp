#include "Framework.h"

void Framework::awake()
{
	DEVICE_MANAGER->awake();
	//RENDERER->awake();
}

void Framework::update()
{
	while (!glfwWindowShouldClose(DEVICE_MANAGER->getWindow())) {
		glfwPollEvents();
		//RENDERER->drawFrame();
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
	//RENDERER->destroy();
	DEVICE_MANAGER->destroy();
}