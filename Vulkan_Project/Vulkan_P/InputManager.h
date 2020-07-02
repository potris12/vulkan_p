#pragma once
#include "Singleton.h"

enum class KeyState
{
	NORMAL,
	DOWN,
	PRESSED,
	UP,
};


class InputManager : public CSingleTonBase<InputManager>
{
public:
	void awake();
	void start() {};
	void update();
	void destroy() { ReleseInstance(); };

	void processInput();

	bool keyDown(int32_t key_code);
	bool keyUp(int32_t key_code);
	bool keyPressed(int32_t key_code);

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	float getYaw();
	float getPitch();
private:
	std::map<int32_t, KeyState> key_;
	
	static float last_x_, last_y_ ;
	static float yaw_, pitch_ ;
	static bool first_mouse_ ;
	static float sensitivity_;
public:
	InputManager();
	~InputManager();
};
#define INPUTMANAGER InputManager::GetInstance()