#pragma once
#include "stdafx.h"

#include "DeviceManager.h"
#include "Object.h"

class Framework;

class Buffer : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();

	void map(void* data);
	void unmap();

	void prepareBuffer();
	const VkBuffer& getBuffer() { return buffer; }
private:
	std::weak_ptr<Framework> weak_framework_;
	/*
	이 함수를 사용하여 다양한 유형의 버퍼를 만들 수 있도록 버퍼 크기, 메모리 속성 및 사용법에 대한 매개변수를 추가해야 함
	마지막 두 매개변수를 핸들을 쓰는 출력변수
	*/
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	/*
	그래픽 카드는 할당 할 수 있는 여러 유형의 메모리를 제공할 ㅅ ㅜ있음
	각 유형의 메모리는 허용되는 작업 및 성능 특성에 따라 다름
	버퍼의 요구 사항과 자체 애플리케이션 요구사항을 결합하여 사용할 올바른 유형의 메모리를 찾아야함
	*/
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	

	/*buffer property*/
	VkDeviceSize bufferSize;
	VkBufferUsageFlags usage_;
	VkMemoryPropertyFlags properties_;

	/*vk Objects*/
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

public:
	Buffer(std::weak_ptr<Framework> weak_framework, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	virtual ~Buffer();
};

