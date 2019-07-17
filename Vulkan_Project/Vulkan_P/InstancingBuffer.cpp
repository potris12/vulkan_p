#include "InstancingBuffer.h"

void InstancingBuffer::awake()
{
	if (buffer_) buffer_->awake();
}

void InstancingBuffer::start()
{
	if (buffer_) buffer_->start();
}

void InstancingBuffer::update()
{
	if (buffer_) buffer_->update();
}

void InstancingBuffer::destroy()
{
	if (buffer_) buffer_->destroy();
}

void InstancingBuffer::prepareBuffer(VkCommandPool & command_pool, void * data)
{
	buffer_->mapWithUnmap(data);
}

void InstancingBuffer::registeCommandBuffer(VkCommandBuffer & commandBuffer, uint32_t firstBinding, uint32_t bindingCount, VkDeviceSize offset)
{
	vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount/*정점버퍼의의 수*/, &buffer_->getVkBuffer(), &offset);
}

InstancingBuffer::InstancingBuffer(VkDeviceSize size)
	: Object("instancing_buffer"), buffer_size_(size)
{
	buffer_ = std::make_shared<GADBuffer>(
		buffer_size_,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

//	staging_buffer_ = std::make_shared<GADBuffer>(buffer_size_,
//		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

InstancingBuffer::~InstancingBuffer()
{
}
