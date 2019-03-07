#include "Buffer.h"
#include "Framework.h"


void Buffer::awake()
{
}

void Buffer::start()
{
}

void Buffer::update()
{
}

void Buffer::destroy()
{
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;

	vkDestroyBuffer(DEVICE_MANAGER->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(DEVICE_MANAGER->getDevice(), stagingBufferMemory, nullptr);
	vkDestroyBuffer(DEVICE_MANAGER->getDevice(), buffer, nullptr);
	vkFreeMemory(DEVICE_MANAGER->getDevice(), bufferMemory, nullptr);
}

void Buffer::map(void* data)
{
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;

	/*
	�� �Լ��� �����°� ũ��� ���ǵ� ������ �޸� ���ҽ��� ������ �׼��� �� �� �ְ���
	���⼭ �����°� ũ��� ���� 0�� bufferInfo.size�� Ư���� VK_WHOLE_SIZE�� �����Ͽ� ��� �޸𸮸� ���� �� ���� ����
	�� ���� �Ű������� �÷��׸� �����ϴµ� ����� �������� ���� API���� ���� ����� �� �ִ� �Ű������� ����
	������ ���� ���ε� �޸𸮿� ���� �������� ����� ����
	*/
	void* tmp_data;
	vkMapMemory(DEVICE_MANAGER->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &tmp_data);
	memcpy(tmp_data, data, (size_t)bufferSize);
}

void Buffer::unmap()
{

	/*
	���� vkUnmapMemory �� ����Ͽ� ���ؽ� �����͸� ���� �� �޸𸮿� memcpy�ϰ� �ٽ� ���� ������ �� ����
	�������� ����̹��� ���� ��� ĳ�� ������ ���� �޸𸮿� �����͸� ��� �������� ���� �� ����
	���ۿ� ���Ⱑ ���ε� �޸𸮿� ���� ǥ�õ��� ���� ���� ���� �� ������ �ذ��� �� �ִ� �ΰ��� ���
	- VK_MEMORY_PROPERTY_HOST_COHERENT_BIT�� �Բ� ǥ�õǴ� ȣ��Ʈ �ϰ��� �޸� ���� ����ض�
	- ���ε� �޸𸮿� ���� �Ŀ� vkFlushMappedMemoryRanges�� ȣ���ϰ� ���� �� �޸𸮿��� �б��� vkInvalidateMappedMemoryRanges�� ȣ���ض�

	�츮�� ���ε� �޸𸮰� �Ҵ� �� �޸��� ����� �һ� ��ġ�ϴ��� Ȯ���ϴ� ù ��° ����� ã�ư���
	����� �÷��� ���� ������ �ణ ������ �� ����
	*/

	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;

	vkUnmapMemory(DEVICE_MANAGER->getDevice(), stagingBufferMemory);
}

void Buffer::prepareBuffer()
{
	copyBuffer(stagingBuffer, buffer, bufferSize);
}

void Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
{
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(DEVICE_MANAGER->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(DEVICE_MANAGER->getDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(DEVICE_MANAGER->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(DEVICE_MANAGER->getDevice(), buffer, bufferMemory, 0);
}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	/*
	���� �����͸� �����ϰ� �����ϱ� ���� stagingBufferMemory�� �Բ� ���ο� stagingBuffer�� ����ϰ� ����
	�� �忡���� �� ���� ���ο� ���� ��� �÷��׸� ����� ��
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT  �޸� ���� ���ۿ��� ���۸� �ҽ��� ����� �� ����
	VK_BUFFER_USAGE_TRANSFER_DST_BIT  ���۴� �޸� ���� �۾����� ������� ����� �� ����

	���� vertexBuffer�� ��ġ�� ���� �޸� �������� �Ҵ��
	�̴� �Ϲ������� vkMapMemory�� ����� �� ������ �ǹ���
	�׷��� stagingBuffer���� vertexBuffer�� �����͸� ������ �� ����
	stagingBuffer�� ���� ���� �ҽ� �÷��׿� vertexBUffer�� ���� ���� ��� �÷��׸� ���� ���� ��� �÷��׿� �Բ� �����ۿ� �̸� ������ �������� ǥ���ؾ���

	*/
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = framework->getCommandPool();//���� command pool�� �ϳ��� �̷��� �ۼ�
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(DEVICE_MANAGER->getDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//�� ���� ����ϰ� ���� �۾��� ������ ��ĥ �� ���� �Լ����� ���ƿ��⸦ ��ٸ� �� 
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;//Optional
	copyRegion.dstOffset = 0;//Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(DEVICE_MANAGER->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(DEVICE_MANAGER->getGraphicsQueue());

	/*
	draw�� �޸� ��ٸ��� �ʾƵ� �� ��� ���� ���؝۷�����
	�� ������ �Ϸ� �� ������ ����� �� �ִ� �ΰ��� ����� ����
	��Ÿ���� ����Ͽ� vkWaitForFences�� ����ϰų� vkQueueWaitIdle�� ����Ͽ� ���� ��⿭�� ���� ���°� �� ������ ��ٸ� �� ����
	���� ��ġ�� ����ϸ� ���ÿ� ���� �� ������ �����ϰ� �� ���� �ϳ� �� �����ϴ� ��� �Ϸ�� ��� ������ ��ٸ� �� ����
	�װ��� �����ڿ��� �� ���� ��ȸ�� ����ȭ ��ų �� ����
	*/
	vkFreeCommandBuffers(DEVICE_MANAGER->getDevice(), framework->getCommandPool(), 1, &commandBuffer);//���� �Ϸ� �� ����
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return uint32_t{};

	/*
	VkPhysicalDeviceMemoryProperties ����ü���� �� ���� �迭 memoryTypes�� memoryHeaps�� ����
	�޸� ���� VRAM�� ������ ��� RAM�� ���� VRAM �� ���� ������ ���� ������ �޸� ���ҽ��Դϴ�. �� ������ ���� ������ �ޤǹа� �ֽ�
	���� �츮�� �޸� ������ ���ؼ��� ������ �������� �װ��� ��ġ�� ���� �ƴ϶� ���ɿ� ������ ��ĥ �� �ִٰ� ����� �� ����
	*/
	vkGetPhysicalDeviceMemoryProperties(DEVICE_MANAGER->getPhysicalDevice()
		, &framework->getPhysicalDeviceMemoryProperties());

	/*
	typeFilter �Ű� ������ ������ �޸� ������ ��Ʈ �ʵ带 �����ϴ� �� ���˴ϴ�. �� �ܼ��� �ݺ��Ͽ� �ش� ��Ʈ�� 1�� �����Ǿ� �ִ��� Ȯ���Ͽ� ������ �޸� ������ �ε����� ã�� �� ����
	���� �����͸� �ش� �޸𸮿� �� �� �־����
	memoryTypes �迭�� �� �޸� ������ �� �� �Ӽ��� �����ϴ� VkMemoryType ����ü�� ������
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT�� ǥ�õ�����
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT �Ӽ��� ����ؾ� ��
	���� �� �Ӽ��� ������ Ȯ���ϱ� ���� ������ ������ �� ����
	*/
	for (uint32_t i = 0; i < framework->getPhysicalDeviceMemoryProperties().memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) && (framework->getPhysicalDeviceMemoryProperties().memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	/*
	�� �̻��� �Ӽ��� ���� �� �����Ƿ� ��Ʈ AND�� ����� ���� 0�� �ƴ� ���ϴ� �Ӽ� ��Ʈ �ʵ�� ������ Ȯ���ؾ� ��
	�ʿ�� �ϴ� ��� ������Ƽ�� ������ ���ۿ� ������ �޸� ���� �����ϸ�, �� �ε����� �����־� �׷��� ���� ��� ����
	*/
	throw std::runtime_error("failed to find suitable memory type!");

}


Buffer::Buffer(std::weak_ptr<Framework> weak_framework, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
: weak_framework_(weak_framework), bufferSize(size), usage_(usage), properties_(properties), Object("buffer")
{
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;
	/*
	ó�� �� �Ű� ������ �������� �ٷ� �� �� ���� �� ��° �Ű������� �޸� ���� ���� ��������
	�� �޸𸮴� ���� ���ۿ� ���� Ư���� �Ҵ�Ǳ� ������ �������� �ܼ��� 0��
	�������� 0�� �ƴѰ�� memRequirements.alignment�� ���� �� �־����
	���� c++�� ���� �޸� �Ҵ�� �޸𸮴� ��� �������� �����Ǿ�� ��, ���� ��ü�� ���ε� �� �޸𸮴� ���۰� �� �̻� ������ ���� ��� �����ؾ� �ϹǷ� ���۰� ������ �� �����ؾ���
	*/

	/*
	filling the vertex buffer
	���� ���ؽ� �����͸� ���ۿ� ������ ����
	�̴� vkMapMemory�� ����� ���� �޸𸮸� CPU�� �׼��� ���� �ϵ��� �޸𸮿� �����Ͽ� ����
	*/

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	createBuffer(bufferSize, usage_, properties_, buffer, bufferMemory);

}


Buffer::~Buffer()
{
}
