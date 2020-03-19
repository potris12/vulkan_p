#include "stdafx.h"
#include "DeviceManager.h"
#include "Renderer.h"
#include "Timer.h"

class Framework : public std::enable_shared_from_this<Framework>
{
public:
	void run() {
		awake();
		update();
		destroy();
	}

private:
	// ��ĭ �ʱ�ȭ
	void awake();
	// update & draw
	void update();
	// ��ĭ �Ҹ�
	void destroy();
	
public:
	
	Framework() = default;
};
