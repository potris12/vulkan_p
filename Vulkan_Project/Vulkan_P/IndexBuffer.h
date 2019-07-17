#pragma once

#include "DeviceManager.h"
#include "Object.h"
#include "GADBuffer.h"

class IndexBuffer : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();

	void prepareBuffer(VkCommandPool& command_pool, void* data);
	void registeCommandBuffer(VkCommandBuffer & commandBuffer, VkDeviceSize offset);

private:

	/*buffer property*/
	VkDeviceSize buffer_size_;
	VkBufferUsageFlags usage_;
	VkMemoryPropertyFlags properties_;	
	VkIndexType index_type_;

	/*vk Objects*/
	std::shared_ptr<GADBuffer> staging_buffer_;
	std::shared_ptr<GADBuffer> buffer_;

public:
	IndexBuffer(VkDeviceSize size, VkIndexType indexType = VK_INDEX_TYPE_UINT16);
	~IndexBuffer();
};