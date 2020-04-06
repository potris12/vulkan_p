
#include "stdafx.h"
#include "Camera.h"

void Camera::update()
{

	CameraBufferData ubo = {};
	//ubo.world = glm::mat4(1.0f);//glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	float aspect = (float)swapChainExtent.width / (float)swapChainExtent.height;
	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

	ubo.proj[1][1] *= -1;//glm 은 원래 opengl용으로 설계되었으므로 클립 좌표의 y좌표가 반전됩 이렇게 안하면 이미지가 위아래 만전됨

	camera_buffer_->setBufferData(&ubo);
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

Camera::Camera(VkViewport viewport, VkRect2D scissor_rect)
	: viewport_(viewport), scissor_rect_(scissor_rect)
{
	camera_buffer_ = std::make_shared<UniformBufferT<CameraBufferData>>(1, 0);
}
