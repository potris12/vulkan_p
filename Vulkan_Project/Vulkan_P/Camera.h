#pragma once
#include "GameObject.h"
#include "UniformBuffer.h"

struct CameraBufferData
{
	glm::mat4 view;
	glm::mat4 proj;

	CameraBufferData() {
		view = glm::mat4(1.0f);
		proj = glm::mat4(1.0f);
	};
};

class Camera
{
public:
	static constexpr int32_t kCameraBufferSlot = 0;

	void update(float dt);

	void setViewportState(VkPipelineViewportStateCreateInfo& viewport_state);
	void move(glm::vec3 dir, float distance);
	void rotate(glm::mat4 mtx);
	void rotate(float x, float y, float z);


	glm::vec3 getRight();
	glm::vec3 getUp();
	glm::vec3 getLook();
	glm::vec3 getPosition();
	glm::mat4 getWorldMtx();

	CameraBufferData camera_buffer_data_;

	std::shared_ptr<UniformBuffer> getCameraBuffer() { return camera_buffer_; }
private:
	

	glm::mat4 world_;
	VkViewport viewport_ = {};
	VkRect2D scissor_ = {};

	std::shared_ptr<UniformBuffer> camera_buffer_;

public:
	Camera(VkViewport viewport, VkRect2D scissor);

};

