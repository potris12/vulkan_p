#include "stdafx.h"

#include "VertexBuffer.h"

void VertexBuffer::awake()
{
	if (buffer_) buffer_->awake();
	if (staging_buffer_) staging_buffer_->awake();
}

void VertexBuffer::start()
{
	if (buffer_) buffer_->start();
	if (staging_buffer_) staging_buffer_->start();
}

void VertexBuffer::update()
{
	if (buffer_) buffer_->update();
	if (staging_buffer_) staging_buffer_->update();
}

void VertexBuffer::destroy()
{
	if (buffer_) buffer_->destroy();
	if(staging_buffer_) staging_buffer_->destroy();
}

void VertexBuffer::prepareBuffer(VkCommandPool& command_pool, void * data)
{
	staging_buffer_->mapWithUnmap(data);
	buffer_->copyBuffer(command_pool, staging_buffer_);
}

void VertexBuffer::registeCommandBuffer(VkCommandBuffer & commandBuffer, uint32_t firstBinding, uint32_t bindingCount, VkDeviceSize offset)
{
	vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount/*정점버퍼의의 수*/, &buffer_->getVkBuffer(), &offset);
}

VertexBuffer::VertexBuffer(VkDeviceSize size)
	: Object("vertex_buffer"), buffer_size_(size)
{
	buffer_ = std::make_shared<GADBuffer>(
		buffer_size_, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	staging_buffer_ = std::make_shared<GADBuffer>(buffer_size_,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

VertexBuffer::~VertexBuffer()
{
}
