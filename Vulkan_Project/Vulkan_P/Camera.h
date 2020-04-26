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


	void update(float dt);
	void setCameraDesc(VkPipelineViewportStateCreateInfo& viewportState);
	std::shared_ptr<UniformBuffer> getCameraBuffer();

	void move(glm::vec3 dir, float distance);
	void setPosition(float x, float y, float z);
	void rotate(glm::mat4 mtx);
	void rotate(float x, float y, float z);
	void updateViewMtx();

	glm::vec3 getRight();
	glm::vec3 getUp();
	glm::vec3 getLook();

	void setRight(glm::vec3 vec);
	void setUp(glm::vec3 vec);
	void setLook(glm::vec3 vec);

	glm::vec3 getPosition();
	glm::mat4 getWorldMtx();
private:
	CameraBufferData camera_buffer_data_;
	glm::mat4 world_;
	VkViewport viewport_;
	VkRect2D scissor_rect_;

	std::shared_ptr<UniformBuffer> camera_buffer_{ nullptr };
	
public:
	Camera(VkViewport viewport, VkRect2D scissor_rect);
};