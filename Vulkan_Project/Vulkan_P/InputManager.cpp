#include "stdafx.h"
#include "InputManager.h"
#include "DeviceManager.h"

float InputManager::last_x_ = 400, InputManager::last_y_ = 400;
float InputManager::yaw_ = 0.f, InputManager::pitch_ = 0.f;
bool InputManager::first_mouse_ = true;
float InputManager::sensitivity_ = 0.05f;

void InputManager::awake()
{
	key_.insert({ GLFW_KEY_W , KeyState::NORMAL });
	key_.insert({ GLFW_KEY_S , KeyState::NORMAL });
	key_.insert({ GLFW_KEY_A , KeyState::NORMAL });
	key_.insert({ GLFW_KEY_D , KeyState::NORMAL });
	key_.insert({ GLFW_KEY_ESCAPE , KeyState::NORMAL });


	//mouse
	glfwSetInputMode(DEVICE_MANAGER->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(DEVICE_MANAGER->getWindow(), mouse_callback);
	sensitivity_ = 0.05f;

}

void InputManager::update()
{
}

void InputManager::processInput()
{

	
	for (auto& key : key_)
	{
		if (glfwGetKey(DEVICE_MANAGER->getWindow(), key.first) == GLFW_PRESS)
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

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (first_mouse_)
	{
		last_x_ = xpos;
		last_y_ = ypos;
		first_mouse_ = false;
	}

	float xoffset = last_x_ - xpos;
	float yoffset = last_y_ - ypos;
	last_x_ = xpos;
	last_y_ = ypos;

	xoffset *= sensitivity_;
	yoffset *= sensitivity_;

	yaw_ += xoffset;
	pitch_ += yoffset;
}

float InputManager::getYaw()
{
	return yaw_;
}

float InputManager::getPitch()
{
	return pitch_;
}

InputManager::InputManager()
	: CSingleTonBase("InputManager")
{
}

InputManager::~InputManager()
{
}
