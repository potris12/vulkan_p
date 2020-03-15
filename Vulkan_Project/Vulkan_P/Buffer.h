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
	이 함수를 사용하여 다양한 유형의 버퍼를 만들 수 있도록 버퍼 크기, 메모리 속성 및 사용법에 대한 매개변수를 추가해야 함
	마지막 두 매개변수를 핸들을 쓰는 출력변수
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

