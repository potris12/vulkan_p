#pragma once

#include "DeviceManager.h"
#include "Object.h"
#include "GADBuffer.h"

class UniformBuffer : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();

	void prepareBuffer(VkCommandPool& command_pool, void* data);
	void setDescWrites(VkDescriptorSet& descriptorSet, VkWriteDescriptorSet& descWrites);

	std::shared_ptr<GADBuffer> buffer_;
private:
	VkDescriptorBufferInfo bufferInfo_ = {};
	/*buffer property*/

	VkDeviceSize buffer_size_ = 0;// = range
	VkDeviceSize buffer_offset_ = 0;
	VkBufferUsageFlags usage_;
	VkMemoryPropertyFlags properties_;
	uint32_t binding_slot_ = 0;

	/*vk Objects*/
//	std::shared_ptr<GADBuffer> staging_buffer_;
//	std::shared_ptr<GADBuffer> buffer_;
	
public:
	UniformBuffer(uint32_t binding_slot, VkDeviceSize size, VkDeviceSize offset);
};