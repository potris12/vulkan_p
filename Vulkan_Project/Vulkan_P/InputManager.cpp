#include "stdafx.h"
#include "InputManager.h"

void InputManager::awake()
{
	key_.insert({ GLFW_KEY_W , KeyState::NORMAL });
	key_.insert({ GLFW_KEY_S , KeyState::NORMAL });
	key_.insert({ GLFW_KEY_A , KeyState::NORMAL });
	key_.insert({ GLFW_KEY_D , KeyState::NORMAL });
}

void InputManager::update()
{
}

void InputManager::processInput(GLFWwindow* window)
{
	for (auto& key : key_)
	{
		if (glfwGetKey(window, key.first) == GLFW_PRESS)
		{//press
			switch (key.second)
			{
			case KeyState::NORMAL:
				key.second = KeyState::DOWN;
				break;
			case KeyState::DOWN:
				key.second = KeyState::PRESSED;
				break;
			case KeyState::PRESSED:
				key.second = KeyState::PRESSED;
				break;
			case KeyState::UP:
				key.second = KeyState::DOWN;
				break;
			}
		}
		else
		{//not press
			//std::cout << "up!! ";
			switch (key.second)
			{
			case KeyState::NORMAL:
				key.second = KeyState::NORMAL;
				break;
			case KeyState::DOWN:
				key.second = KeyState::UP;
				break;
			case KeyState::PRESSED:
				key.second = KeyState::UP;
				break;
			case KeyState::UP:
				key.second = KeyState::NORMAL;
				break;
			}
		}
	}
}

bool InputManager::keyDown(int32_t key_code)
{
	return key_[key_code] == KeyState::DOWN;
}

bool InputManager::keyUp(int32_t key_code)
{
	return key_[key_code] == KeyState::UP;
}

bool InputManager::keyPressed(int32_t key_code)
{
	return key_[key_code] == KeyState::PRESSED;
}

InputManager::InputManager()
	: CSingleTonBase("InputManager")
{
}

InputManager::~InputManager()
{
}
