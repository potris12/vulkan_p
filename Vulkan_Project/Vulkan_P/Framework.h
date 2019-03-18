#include "stdafx.h"
#include "DeviceManager.h"
#include "Renderer.h"

class Framework : public std::enable_shared_from_this<Framework>
{
public:
	void run() {
		awake();
		mainLoop();
		cleanup();
	}

private:
	void awake();
	void mainLoop();
	void cleanup();
	
public:
	
	Framework() = default;
};
