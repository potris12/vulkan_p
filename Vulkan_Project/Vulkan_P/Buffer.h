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

	void mapWithUnmap(void* data);

	void prepareBuffer(void* data);
	void registeCommandBuffer(VkCommandBuffer & commandBuffer, uint32_t firstBinding, uint32_t bindingCount, VkDeviceSize offsets);

	static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

private:

	
	/*
	�� �Լ��� ����Ͽ� �پ��� ������ ���۸� ���� �� �ֵ��� ���� ũ��, �޸� �Ӽ� �� ������ ���� �Ű������� �߰��ؾ� ��
	������ �� �Ű������� �ڵ��� ���� ��º���
	*/
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	/*buffer property*/
	VkDeviceSize buffer_size_;
	VkBufferUsageFlags usage_;
	VkMemoryPropertyFlags properties_;

	/*vk Objects*/
	VkBuffer staging_buffer_;
	VkDeviceMemory staging_buffer_memory_;
	VkBuffer buffer_;
	VkDeviceMemory buffer_memory_;

public:
	Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	virtual ~Buffer();
};

