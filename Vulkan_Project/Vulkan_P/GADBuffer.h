#pragma once

#include "DeviceManager.h"
#include "Object.h"


class GADBuffer : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();
	
	void copyBuffer(VkCommandPool& command_pool, const std::shared_ptr<GADBuffer>& src_buffer);
	void mapWithUnmap(void* data);

	VkDeviceSize getBufferSize() { return size_; }
	VkBuffer& getVkBuffer() { return buffer_; }
private:
	VkBuffer buffer_;
	VkDeviceMemory buffer_memory_;
	VkDeviceSize size_;

public:
	GADBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	~GADBuffer();
};

