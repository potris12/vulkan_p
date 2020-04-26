
#include "stdafx.h"
#include "Camera.h"
#include "InputManager.h"

void Camera::update(float dt)
{
	//mouse input
	auto yaw = INPUTMANAGER->getYaw();
	auto pitch = INPUTMANAGER->getPitch();

	//key input
	constexpr static float camera_velocity = 10.f;
	glm::vec3 camera_dir = glm::vec3(0.f);
	if (INPUTMANAGER->keyPressed(GLFW_KEY_W))
	{
		camera_dir += getLook();
	}
	if (INPUTMANAGER->keyPressed(GLFW_KEY_A))
	{
		camera_dir += getRight();
	}
	if (INPUTMANAGER->keyPressed(GLFW_KEY_S))
	{
		camera_dir -= getLook();
	}
	if (INPUTMANAGER->keyPressed(GLFW_KEY_D))
	{
		camera_dir -= getRight();
	}
	move(camera_dir, camera_velocity * dt);


	//std::cout << "world_[2].z : [" << world_[2].z << "]" << std::endl;
	//std::cout << "world_[1].y : [" << world_[1].y << "]" << std::endl;

	if (yaw != glm::zero<float>() /*|| pitch != glm::zero<float>()*/)
	{
		rotate(0.f, yaw, 0.f);
		/*if (world_[1].y > 0.f)
			rotate(0.f, yaw, 0.f);
		else
			rotate(0.f, -yaw, 0.f);*/

		/*if (world_[2].z >= 0.f)
			rotate(-pitch, 0.f, 0.f);
		else
			rotate(pitch, 0.f, 0.f);*/
	}

	updateViewMtx();

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
		rotate(rotate_mtx);
	}
	if (y != 0.0f)
	{
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(y), getUp());
		rotate(rotate_mtx);
	}
	if (z != 0.0f)
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다.
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(z), getLook());
		rotate(rotate_mtx);
	}
}

void Camera::updateViewMtx()
{
	setLook(glm::normalize(getLook()));
	setRight(glm::cross(getUp(), getLook()));
	setRight(getRight());
	setUp(glm::cross(getLook(), getRight()));
	setUp(glm::normalize(getUp()));

	camera_buffer_data_.view = glm::lookAt(getPosition(), getPosition() + getLook(), getUp());
	camera_buffer_->setBufferData(&camera_buffer_data_);
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
void Camera::setRight(glm::vec3 vec)
{
	world_[0].x = vec.x;
	world_[0].y = vec.y;
	world_[0].z = vec.z;
}
void Camera::setUp(glm::vec3 vec)
{
	world_[1].x = vec.x;
	world_[1].y = vec.y;
	world_[1].z = vec.z;
}
void Camera::setLook(glm::vec3 vec)
{
	world_[2].x = vec.x;
	world_[2].y = vec.y;
	world_[2].z = vec.z;
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
	camera_buffer_data_.proj[1][1] *= -1;//glm 은 원래 opengl용으로 설계되었으므로 클립 좌표의 y좌표가 반전됩 이렇게 안하면 이미지가 위아래 만전됨

	rotate(0.f, 0.f, 0.f);
	camera_buffer_ = std::make_shared<UniformBufferT<CameraBufferData>>(1, 0);
}
