
#include "stdafx.h"

#include "GADBuffer.h"


void GADBuffer::awake()
{
}

void GADBuffer::start()
{
}

void GADBuffer::update()
{
}

void GADBuffer::destroy()
{
	vkDestroyBuffer(DEVICE_MANAGER->getDevice(), buffer_, nullptr);
	vkFreeMemory(DEVICE_MANAGER->getDevice(), buffer_memory_, nullptr);
}

void GADBuffer::copyBuffer(VkCommandPool& command_pool, const std::shared_ptr<GADBuffer>& src_buffer)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = command_pool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(DEVICE_MANAGER->getDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//한 번만 사용하고 복사 작업이 실행을 마칠 떄 까지 함수에서 돌아오기를 기다릴 것 
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;//Optional
	copyRegion.dstOffset = 0;//Optional
	copyRegion.size = size_;
	vkCmdCopyBuffer(commandBuffer, src_buffer->getVkBuffer(), buffer_, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(DEVICE_MANAGER->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(DEVICE_MANAGER->getGraphicsQueue());

	vkFreeCommandBuffers(DEVICE_MANAGER->getDevice(), command_pool, 1, &commandBuffer);//전송 완료 후 정리

}

void GADBuffer::mapWithUnmap(void * data)
{
	void* tmp_data;
	vkMapMemory(DEVICE_MANAGER->getDevice(), buffer_memory_, 0, size_, 0, &tmp_data);
	memcpy(tmp_data, data, (size_t)size_);
	vkUnmapMemory(DEVICE_MANAGER->getDevice(), buffer_memory_);
}

GADBuffer::GADBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
	: Object("gad_buffer"), size_(size)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(DEVICE_MANAGER->getDevice(), &bufferInfo, nullptr, &buffer_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(DEVICE_MANAGER->getDevice(), buffer_, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = DEVICE_MANAGER->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(DEVICE_MANAGER->getDevice(), &allocInfo, nullptr, &buffer_memory_) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(DEVICE_MANAGER->getDevice(), buffer_, buffer_memory_, 0);
}

GADBuffer::~GADBuffer()
{
}
