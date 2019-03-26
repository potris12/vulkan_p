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
	�� �Լ��� ����Ͽ� �پ��� ������ ���۸� ���� �� �ֵ��� ���� ũ��, �޸� �Ӽ� �� ������ ���� �Ű������� �߰��ؾ� ��
	������ �� �Ű������� �ڵ��� ���� ��º���
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

