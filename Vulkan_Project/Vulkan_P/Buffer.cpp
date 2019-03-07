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
	이 함수는 오프셋과 크기로 정의된 지정된 메모리 리소스의 영역에 액세스 할 수 있게함
	여기서 오프셋과 크기는 각각 0과 bufferInfo.size임 특수값 VK_WHOLE_SIZE를 지정하여 모든 메모리를 매핑 할 수도 있음
	두 번쨰 매개변수는 플래그를 지정하는데 사용할 수있지만 지금 API에는 아직 사용할 수 있는 매개변수가 없음
	마지막 값은 매핑된 메모리에 대한 포인터의 출력을 지정
	*/
	void* tmp_data;
	vkMapMemory(DEVICE_MANAGER->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &tmp_data);
	memcpy(tmp_data, data, (size_t)bufferSize);
}

void Buffer::unmap()
{

	/*
	이제 vkUnmapMemory 를 사용하여 버텍스 데이터를 매핑 된 메모리에 memcpy하고 다시 매핑 해제할 수 있음
	불행히도 드라이버는 예를 들어 캐싱 때문에 버퍼 메모리에 데이터를 즉시 복사하지 않을 수 있음
	버퍼에 쓰기가 매핑된 메모리에 아직 표시되지 않을 수도 있음 이 문제를 해결할 수 있는 두가지 방법
	- VK_MEMORY_PROPERTY_HOST_COHERENT_BIT와 함께 표시되는 호스트 일관된 메모리 힙을 사용해라
	- 매핑된 메모리에 쓰기 후에 vkFlushMappedMemoryRanges를 호출하고 매핑 된 메모리에서 읽기전 vkInvalidateMappedMemoryRanges를 호출해라

	우리는 매핑된 메모리가 할당 된 메모리의 내용과 할상 일치하는지 확인하는 첫 번째 방법을 찾아갔음
	명시적 플러싱 보다 성능이 약간 떨어질 수 있음
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
	정점 데이터를 매핑하고 복사하기 위해 stagingBufferMemory와 함께 새로운 stagingBuffer를 사용하고 있음
	이 장에서는 두 개의 새로운 버퍼 사용 플래그를 사용할 것
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT  메모리 전송 동작에서 버퍼를 소스로 사용할 수 있음
	VK_BUFFER_USAGE_TRANSFER_DST_BIT  버퍼는 메모리 전송 작업에서 대상으로 사용할 수 있음

	이제 vertexBuffer는 장치의 로컬 메모리 유형으로 할당됨
	이는 일반적으로 vkMapMemory를 사용할 수 없음을 의미함
	그러나 stagingBuffer에서 vertexBuffer로 데이터를 복사할 수 있음
	stagingBuffer에 대한 전송 소스 플래그와 vertexBUffer에 대한 전송 대상 플래그를 정점 버퍼 사용 플래그와 함께 지저앟여 이를 수행할 예정임을 표시해야함

	*/
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = framework->getCommandPool();//아직 command pool이 하나라서 이렇게 작성
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
	draw와 달리 기다리지 않아도 됨 즉시 전송 실해앟려고함
	이 전송이 완료 될 때까지 대기할 수 있는 두가지 방법이 있음
	울타리를 사용하여 vkWaitForFences로 대기하거나 vkQueueWaitIdle을 사용하여 전송 대기열이 유휴 상태가 될 떄까지 기다릴 수 있음
	차단 장치를 사용하면 동시에 여러 번 전송을 예약하고 한 번에 하나 씩 실행하는 대신 완료된 모든 전송을 기다릴 수 있음
	그것은 운전자에게 더 많은 기회를 최적화 시킬 수 있음
	*/
	vkFreeCommandBuffers(DEVICE_MANAGER->getDevice(), framework->getCommandPool(), 1, &commandBuffer);//전송 완료 후 정리
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return uint32_t{};

	/*
	VkPhysicalDeviceMemoryProperties 구조체에는 두 개의 배열 memoryTypes및 memoryHeaps가 있음
	메모리 힙은 VRAM이 부족한 경우 RAM의 전용 VRAM 및 스왑 공간과 같은 별개의 메모리 리소스입니다. 이 힙에는 여러 유형의 메ㅗ밀가 있습
	지금 우리는 메모리 윻ㅇ에 대해서만 관심을 가질것임 그것이 위치한 힙이 아니라 성능에 영향을 미칠 수 있다고 상상할 수 있음
	*/
	vkGetPhysicalDeviceMemoryProperties(DEVICE_MANAGER->getPhysicalDevice()
		, &framework->getPhysicalDeviceMemoryProperties());

	/*
	typeFilter 매개 변수는 적절한 메모리 유형의 비트 필드를 지정하는 데 사용됩니다. 즉 단순히 반복하여 해당 비트가 1로 설정되어 있는지 확인하여 적절한 메모리 유형의 인덱스를 찾을 수 있음
	정점 데이터를 해당 메모리에 쓸 수 있어야해
	memoryTypes 배열은 각 메모리 유형의 힙 및 속성을 지정하는 VkMemoryType 구조체로 구성됨
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT로 표시되지만
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 속성도 사용해야 함
	이제 이 속성의 지원을 확인하기 위해 루프를 수정할 수 있음
	*/
	for (uint32_t i = 0; i < framework->getPhysicalDeviceMemoryProperties().memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) && (framework->getPhysicalDeviceMemoryProperties().memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	/*
	둘 이상의 속성을 가질 수 있으므로 비트 AND의 결과가 단지 0이 아닌 원하는 속성 비트 필드와 같은지 확인해야 함
	필요로 하는 모든 프로퍼티를 가지는 버퍼에 적절한 메모리 형이 존재하면, 그 인덱스를 돌려주어 그렇지 않은 경우 오류
	*/
	throw std::runtime_error("failed to find suitable memory type!");

}


Buffer::Buffer(std::weak_ptr<Framework> weak_framework, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
: weak_framework_(weak_framework), bufferSize(size), usage_(usage), properties_(properties), Object("buffer")
{
	auto framework = weak_framework_.lock();
	if (nullptr == framework) return;
	/*
	처음 세 매개 변수는 무엇인지 바로 알 수 있음 네 번째 매개변수는 메모리 영역 내의 오프셋임
	이 메모리는 정점 버퍼에 대해 특별히 할당되기 떄문에 오프셋은 단순히 0임
	오프셋이 0이 아닌경우 memRequirements.alignment로 나눌 수 있어야해
	물론 c++의 동적 메모리 할당과 메모리는 어느 시점에서 해제되어야 함, 버퍼 객체에 바인드 된 메모리는 버퍼가 더 이상 사용되지 않을 경우 해제해야 하므로 버퍼가 삭제된 후 해제해야해
	*/

	/*
	filling the vertex buffer
	이제 버텍스 데이터를 버퍼에 복사할 차례
	이는 vkMapMemory를 사용해 버퍼 메모리를 CPU가 액세스 가능 하도록 메모리에 매핑하여 수행
	*/

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	createBuffer(bufferSize, usage_, properties_, buffer, bufferMemory);

}


Buffer::~Buffer()
{
}
