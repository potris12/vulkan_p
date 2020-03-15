#pragma once
#include "stdafx.h"

#include "DeviceManager.h"
#include "Object.h"

class Framework;

template <typename T>
struct BufferData
{
public:
	uint32_t stride_{ 0 };
	uint32_t data_num_{ 0 };
	std::vector<T> datas_;

public:
	BufferData(std::vector<T>& datas)
	{
		datas_.assign(datas.begin(), datas.end());
		stride_ = static_cast<uint32_t>(sizeof(T));
		data_num_ = static_cast<uint32_t>(datas.size());
	}
	
	BufferData(uint32_t data_num)
	{
		if (0 > data_num) return;

		datas_.resize(data_num);
		stride_ = static_cast<uint32_t>(sizeof(T));
		data_num_ = static_cast<uint32_t>(datas_.size());
	}
};


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

