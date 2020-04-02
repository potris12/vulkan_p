#include "Camera.h"

void Camera::update(float dt)
{
	camera_buffer_data_ = {};
	camera_buffer_data_.view = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	float aspect = (float)swapChainExtent.width / (float)swapChainExtent.height;
	camera_buffer_data_.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

	camera_buffer_data_.proj[1][1] *= -1;//glm 은 원래 opengl용으로 설계되었으므로 클립 좌표의 y좌표가 반전됩 이렇게 안하면 이미지가 위아래 만전됨

	camera_buffer_->setBufferData(&camera_buffer_data_);
}

void Camera::setViewportState(VkPipelineViewportStateCreateInfo& viewport_state)
{
	viewport_state = VkPipelineViewportStateCreateInfo{};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports = &viewport_;
	viewport_state.scissorCount = 1;
	viewport_state.pScissors = &scissor_;
}

void Camera::move(glm::vec3 dir, float distance)
{
	dir = glm::normalize(dir);
	dir *= distance;

	world_[3].x += dir.x;
	world_[3].y += dir.y;
	world_[3].z += dir.z;
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
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(y), getUp());
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetUp(), y));
	}
	if (z != 0.0f)
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다.
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

Camera::Camera(VkViewport viewport, VkRect2D scissor)
	: viewport_(viewport), scissor_(scissor)
{
	world_ = glm::mat4(1.0f);

	camera_buffer_ = std::make_shared<UniformBufferT<CameraBufferData>>(1, kCameraBufferSlot);
}
