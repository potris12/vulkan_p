
#include "stdafx.h"
#include "Camera.h"
#include "InputManager.h"

void Camera::update(float dt)
{
	//mouse input
	auto yaw = INPUTMANAGER->getYaw();
	auto pitch = INPUTMANAGER->getPitch();

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	world_[2].x = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); 
	world_[2].y = sin(glm::radians(pitch));
	world_[2].z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	world_[2] = glm::normalize(world_[2]);

	//key input
	constexpr static float camera_velocity = 10.f;
	glm::vec3 camera_dir = glm::vec3(0.f);
	if (INPUTMANAGER->keyPressed(GLFW_KEY_W))
	{
		camera_dir.z += 1.f;
	}
	if (INPUTMANAGER->keyPressed(GLFW_KEY_A))
	{
		camera_dir.x += 1.f;
	}
	if (INPUTMANAGER->keyPressed(GLFW_KEY_S))
	{
		camera_dir.z -= 1.f;
	}
	if (INPUTMANAGER->keyPressed(GLFW_KEY_D))
	{
		camera_dir.x -= 1.f;
	}
	move(camera_dir, camera_velocity * dt);

	//rotate(0, 0, 50.f * dt);
	//ubo.world = glm::mat4(1.0f);//glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//camera_buffer_data_.view = glm::lookAt(getPosition(),glm::vec3(0,0,0), getUp());//pos / see pos/ up
	camera_buffer_data_.view = glm::lookAt(getPosition(), getPosition() + getLook(), getUp());
	camera_buffer_->setBufferData(&camera_buffer_data_);
}

void Camera::setCameraDesc(VkPipelineViewportStateCreateInfo& viewportState)
{
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport_;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor_rect_;
}

std::shared_ptr<UniformBuffer> Camera::getCameraBuffer()
{
	return camera_buffer_;
}

void Camera::move(glm::vec3 dir, float distance)
{
	if (dir.x == glm::zero<float>() && dir.z == glm::zero<float>()) return;

	dir = glm::normalize(dir);
	dir *= distance;

	world_[3].x += dir.x;
	world_[3].y += dir.y;
	world_[3].z += dir.z;
}

void Camera::setPosition(float x, float y, float z)
{
	world_[3].x = x;
	world_[3].y = y;
	world_[3].z = z;
}

void Camera::rotate(glm::mat4 mtx) {
	world_ = world_ * mtx;
}

void Camera::rotate(float x, float y, float z) {
	glm::mat4 rotate_mtx = glm::mat4(1.f);
	if (x != 0.0f)
	{
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(x), getRight());
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetRight(), x));
	}
	if (y != 0.0f)
	{
		//�÷��̾��� ���� y-���� �������� ȸ���ϴ� ����� �����Ѵ�.
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(y), getUp());
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetUp(), y));
	}
	if (z != 0.0f)
	{
		//�÷��̾��� ���� z-���� �������� ȸ���ϴ� ����� �����Ѵ�.
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(z), getLook());
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetLook(), z));
	}

	rotate(rotate_mtx);
}

//--------------------------------getter--------------------------------
glm::vec3 Camera::getRight() {
	return world_[0];
}
glm::vec3 Camera::getUp() {
	return world_[1];
}
glm::vec3 Camera::getLook() {
	return world_[2];
}
glm::vec3 Camera::getPosition() {
	return world_[3];
}
glm::mat4 Camera::getWorldMtx() {
	return world_;
}

Camera::Camera(VkViewport viewport, VkRect2D scissor_rect)
	: viewport_(viewport), scissor_rect_(scissor_rect)
{
	world_ = glm::mat4(1.f);
	//set projection
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	float aspect = (float)swapChainExtent.width / (float)swapChainExtent.height;
	camera_buffer_data_.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
	camera_buffer_data_.proj[1][1] *= -1;//glm �� ���� opengl������ ����Ǿ����Ƿ� Ŭ�� ��ǥ�� y��ǥ�� ������ �̷��� ���ϸ� �̹����� ���Ʒ� ������

	camera_buffer_ = std::make_shared<UniformBufferT<CameraBufferData>>(1, 0);
}
