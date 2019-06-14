#pragma once

#include "DeviceManager.h"
#include "Object.h"

class VertexBuffer : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();

private:

	/*buffer property*/
	VkDeviceSize bufferSize;
	VkBufferUsageFlags usage_;
	VkMemoryPropertyFlags properties_;

	/*vk Objects*/
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

public:
	VertexBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	~VertexBuffer();
};

