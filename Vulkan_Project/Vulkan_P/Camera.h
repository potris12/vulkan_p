#pragma once
#include "UniformBuffer.h"
//아직 이동지원 전 그냥 REnderer의 camera관련 기능을 가져오도록 함 

class Camera
{
public:
	struct CameraBufferData
	{
		glm::mat4 world;
		glm::mat4 view;
		glm::mat4 proj;

		CameraBufferData() {
			world = glm::mat4(1.0f);
			view = glm::mat4(1.0f);
			proj = glm::mat4(1.0f);
		};
	};


	void update();
	void setCameraDesc(VkPipelineViewportStateCreateInfo& viewportState);

	std::shared_ptr<UniformBuffer> getCameraBuffer();
private:
	VkViewport viewport_;
	VkRect2D scissor_rect_;

	std::shared_ptr<UniformBuffer> camera_buffer_{ nullptr };
	
public:
	Camera(VkViewport viewport, VkRect2D scissor_rect);
};