#pragma once

#include "DeviceManager.h"
#include "Object.h"
#include "Buffer.h"
#include "GADBuffer.h"


class IndexBuffer : public Object
{
public:
	virtual uint32_t getDataCount() = 0;
	virtual uint32_t getDataStride() = 0;
	virtual void registeCommandBuffer(VkCommandBuffer& commandBuffer, VkDeviceSize offset) = 0;

protected:
	IndexBuffer(std::string object_name)
		: Object(object_name)
	{

	}
};

template <typename T>
class IndexBufferT : public IndexBuffer
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

	virtual void registeCommandBuffer(VkCommandBuffer& commandBuffer, VkDeviceSize offset)
	{
		if (buffer_)
		{
			vkCmdBindIndexBuffer(commandBuffer, buffer_->getVkBuffer(), offset, index_type_);
		}
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

	/*buffer property*/
	VkIndexType index_type_{ VK_INDEX_TYPE_MAX_ENUM };//index type 

	/*vk Objects*/
	std::shared_ptr<GADBuffer> staging_buffer_{ nullptr };
	std::shared_ptr<GADBuffer> buffer_{ nullptr };

	//buffer data
	BufferData<T> buffer_data_;

	VkIndexType getIndexType()
	{
		switch (buffer_data_.stride_)
		{
		case 2:
			return VK_INDEX_TYPE_UINT16;

		case 4:
			return VK_INDEX_TYPE_UINT32;

		default:
			break;
		}
	}
public:
	IndexBufferT(VkCommandPool& command_pool, std::vector<T>& datas)
		: IndexBuffer("index_bufferT"), buffer_data_(datas)
	{
		buffer_ = std::make_shared<GADBuffer>(
			buffer_data_.data_num_ * buffer_data_.stride_,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		staging_buffer_ = std::make_shared<GADBuffer>(
			buffer_data_.data_num_ * buffer_data_.stride_,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		staging_buffer_->mapWithUnmap(buffer_data_.datas_.data());
		buffer_->copyBuffer(command_pool, staging_buffer_);
		index_type_ = getIndexType();
	}
	~IndexBufferT() {};
};
