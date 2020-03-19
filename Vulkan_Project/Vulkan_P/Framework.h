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
	// ¹úÄ­ ÃÊ±âÈ­
	void awake();
	// update & draw
	void update();
	// ¹úÄ­ ¼Ò¸ê
	void destroy();
	
public:
	
	Framework() = default;
};
