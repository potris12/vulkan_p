#pragma once

#include "DeviceManager.h"
#include "Object.h"
#include "GADBuffer.h"
#include "Buffer.h"

//struct UniformBufferBindingData
//{
//	uint32_t binding_slot_ = 0;
//	uint32_t binding_count_ = 0;
//	VkDeviceSize offset_ = 0;
//
//	UniformBufferBindingData(UniformBufferBindingData& data)
//	{
//		binding_slot_ = data.binding_slot_;
//		binding_count_ = data.binding_count_;
//		offset_ = data.offset_;
//	}
//	UniformBufferBindingData(uint32_t binding_slot = 0, uint32_t binding_count = 0, VkDeviceSize offset = 0)
//	{
//		binding_slot_ = binding_slot;
//		binding_count_ = binding_count;
//		offset_ = offset;
//	}
//
//};

class UniformBuffer : public Object
{
public:
	virtual void setBufferData(void* data) = 0;
	virtual void setDescWrites(VkDescriptorSet& descriptorSet, VkWriteDescriptorSet& descWrites) = 0;

protected:
	UniformBuffer(std::string name)
		:Object(name) {}
};

template <typename T>
class UniformBufferT : public UniformBuffer
{
public:
	virtual void awake() {
		if (buffer_) buffer_->awake();
	};
	virtual void start() {
		if (buffer_) buffer_->start();
	};
	virtual void update() {
		if (buffer_) buffer_->update();
	};
	virtual void destroy() {
		if (buffer_) buffer_->destroy();
	};

	void setBufferData(void* data)
	{
		buffer_->mapWithUnmap(data);
	}

	void setDescWrites(VkDescriptorSet& descriptor_set, VkWriteDescriptorSet& desc_write)
	{
		desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		desc_write.dstSet = descriptor_set;
		desc_write.dstBinding = binding_slot_;//이건 static 변수로 관리 
		desc_write.dstArrayElement = 0;
		desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		desc_write.descriptorCount = 1;//
		desc_write.pBufferInfo = &bufferInfo_;
	}

	
private:
	VkDescriptorBufferInfo bufferInfo_ = {};
	uint32_t binding_slot_ = 0;
	/*buffer property*/

	/*vk Objects*/
	std::shared_ptr<GADBuffer> buffer_;
	BufferData<T> buffer_data_;
	
public:
	UniformBufferT(uint32_t data_num, uint32_t binding_slot)
		: UniformBuffer("uniform_bufferT"), buffer_data_(data_num), binding_slot_(binding_slot)
	{
		buffer_ = std::make_shared<GADBuffer>(
			buffer_data_.data_num_ * buffer_data_.stride_,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		bufferInfo_.buffer = buffer_->getVkBuffer();
		//offset 과 사용영역은 0과 전체 로 통일함
		bufferInfo_.offset = 0;
		bufferInfo_.range = VK_WHOLE_SIZE;

	}
};