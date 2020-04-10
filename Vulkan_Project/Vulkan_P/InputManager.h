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
	void destroy() {};

	void processInput(GLFWwindow* window);

	bool keyDown(int32_t key_code);
	bool keyUp(int32_t key_code);
	bool keyPressed(int32_t key_code);

private:
	std::map<int32_t, KeyState> key_;

public:
	InputManager();
	~InputManager();
};
#define INPUTMANAGER InputManager::GetInstance()