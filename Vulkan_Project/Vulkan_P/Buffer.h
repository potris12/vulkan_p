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

	static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

private:

	
	/*
	이 함수를 사용하여 다양한 유형의 버퍼를 만들 수 있도록 버퍼 크기, 메모리 속성 및 사용법에 대한 매개변수를 추가해야 함
	마지막 두 매개변수를 핸들을 쓰는 출력변수
	*/
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

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
	Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	virtual ~Buffer();
};

