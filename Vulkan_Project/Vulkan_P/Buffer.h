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
	�� �Լ��� ����Ͽ� �پ��� ������ ���۸� ���� �� �ֵ��� ���� ũ��, �޸� �Ӽ� �� ������ ���� �Ű������� �߰��ؾ� ��
	������ �� �Ű������� �ڵ��� ���� ��º���
	*/
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	/*
	�׷��� ī��� �Ҵ� �� �� �ִ� ���� ������ �޸𸮸� ������ �� ������
	�� ������ �޸𸮴� ���Ǵ� �۾� �� ���� Ư���� ���� �ٸ�
	������ �䱸 ���װ� ��ü ���ø����̼� �䱸������ �����Ͽ� ����� �ùٸ� ������ �޸𸮸� ã�ƾ���
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

