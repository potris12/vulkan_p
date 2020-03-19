#pragma once

#include "DeviceManager.h"
#include "Object.h"
#include "GADBuffer.h"
#include "Buffer.h"

struct InstancingBufferBindingData
{
	uint32_t binding_slot_= 0;
	uint32_t binding_count_= 0;
	VkDeviceSize offset_ = 0;

	InstancingBufferBindingData(InstancingBufferBindingData& data)
	{
		binding_slot_ = data.binding_slot_;
		binding_count_ = data.binding_count_;
		offset_ = data.offset_;
	}
	InstancingBufferBindingData(uint32_t binding_slot = 0, uint32_t binding_count = 0, VkDeviceSize offset = 0)
	{
		binding_slot_ = binding_slot;
		binding_count_ = binding_count;
		offset_ = offset;
	}
};

class InstancingBuffer : public Object
{
public:
	virtual void registeCommandBuffer(VkCommandBuffer& commandBuffer) = 0;

	virtual void addBufferDataStart() = 0;
	virtual void addBufferData(void* data) = 0;
	virtual void addBufferDataEnd() = 0;

protected:
	InstancingBuffer(std::string name)
		: Object(name) {};

};


template <typename T>
class InstancingBufferT : public InstancingBuffer
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

	void registeCommandBuffer(VkCommandBuffer& commandBuffer)
	{
		vkCmdBindVertexBuffers(commandBuffer, binding_buffer_.binding_slot_, binding_buffer_.binding_count_/*정점버퍼의의 수*/, &buffer_->getVkBuffer(), &binding_buffer_.offset_);
	}

	virtual void addBufferDataStart()
	{
		data_index_ = 0;
	}

	virtual void addBufferData(void* data)
	{
		if (data_index_ >= buffer_data_.data_num_) return;

		auto pData = static_cast<T*>(data);
		buffer_data_.datas_[data_index_++] = *pData;
	}

	virtual void addBufferDataEnd()
	{
		buffer_->mapWithUnmap(buffer_data_.datas_.data());
	}

private:
	/*vk Objects*/
	std::shared_ptr<GADBuffer> buffer_;

	//buffer data
	BufferData<T> buffer_data_;
	InstancingBufferBindingData binding_buffer_;
	uint32_t data_index_ = 0;
public:
	InstancingBufferT(int32_t data_num, InstancingBufferBindingData binding_buffer)
		:InstancingBuffer("instancing_bufferT"), buffer_data_(data_num), binding_buffer_(binding_buffer)
	{
		buffer_ = std::make_shared<GADBuffer>(
			buffer_data_.data_num_ * buffer_data_.stride_,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
};