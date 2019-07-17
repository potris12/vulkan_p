#pragma once

#include "DeviceManager.h"
#include "Object.h"
#include "GADBuffer.h"

class InstancingBuffer : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();

	void prepareBuffer(VkCommandPool& command_pool, void* data);
	void registeCommandBuffer(VkCommandBuffer & commandBuffer, uint32_t firstBinding, uint32_t bindingCount, VkDeviceSize offsets);

private:

	/*buffer property*/
	VkDeviceSize buffer_size_;
	VkBufferUsageFlags usage_;
	VkMemoryPropertyFlags properties_;

	/*vk Objects*/
	//std::shared_ptr<GADBuffer> staging_buffer_;
	std::shared_ptr<GADBuffer> buffer_;

public:
	InstancingBuffer(VkDeviceSize size);
	~InstancingBuffer();
};