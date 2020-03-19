#pragma once

#include "DeviceManager.h"
#include "Object.h"
#include "GADBuffer.h"
#include "Buffer.h"

class VertexBuffer : public Object
{
public:
	virtual void registeCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t firstBinding, uint32_t bindingCount, VkDeviceSize offsets) = 0;
	virtual uint32_t getDataCount() = 0;
	virtual uint32_t getDataStride() = 0;

protected:
	VertexBuffer(std::string name)
		: Object(name)
	{

	};

};

template <typename T>
class VertexBufferT : public VertexBuffer
{
public:
	virtual void awake() {
		if (buffer_) buffer_->awake();
		if (staging_buffer_) staging_buffer_->awake();
	};
	virtual void start() {
		if (buffer_) buffer_->start();
		if (staging_buffer_) staging_buffer_->start();
	};
	virtual void update() {
		if (buffer_) buffer_->update();
		if (staging_buffer_) staging_buffer_->update();
	};
	virtual void destroy() {
		if (buffer_) buffer_->destroy();
		if (staging_buffer_) staging_buffer_->destroy();
	};

	virtual void registeCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t firstBinding, uint32_t bindingCount, VkDeviceSize offset)
	{
		vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount/*정점버퍼의의 수*/, &buffer_->getVkBuffer(), &offset);
	}
	virtual uint32_t getDataCount()
	{
		return buffer_data_.data_num_;
	}

	virtual uint32_t getDataStride()
	{
		return buffer_data_.stride_;
	}

private:

	/*vk Objects*/
	std::shared_ptr<GADBuffer> staging_buffer_;
	std::shared_ptr<GADBuffer> buffer_;

	//buffer data
	BufferData<T> buffer_data_;

public:
	VertexBufferT(VkCommandPool& command_pool, std::vector<T>& datas)
		: VertexBuffer("vertex_bufferT"), buffer_data_(datas)
	{
		buffer_ = std::make_shared<GADBuffer>(
			buffer_data_.data_num_ * buffer_data_.stride_,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		staging_buffer_ = std::make_shared<GADBuffer>(
			buffer_data_.data_num_ * buffer_data_.stride_,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		staging_buffer_->mapWithUnmap(buffer_data_.datas_.data());
		buffer_->copyBuffer(command_pool, staging_buffer_);
	}
	~VertexBufferT() {};
};

