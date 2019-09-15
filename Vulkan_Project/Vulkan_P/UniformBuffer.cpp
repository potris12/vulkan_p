#include "UniformBuffer.h"

void UniformBuffer::awake()
{
	if (buffer_) buffer_->awake();
}

void UniformBuffer::start()
{
	if (buffer_) buffer_->start();
}

void UniformBuffer::update()
{
	if (buffer_) buffer_->update();
}

void UniformBuffer::destroy()
{
	if (buffer_) buffer_->destroy();
}

void UniformBuffer::prepareBuffer(VkCommandPool & command_pool, void * data)
{
	buffer_->mapWithUnmap(data);
}

void UniformBuffer::setDescWrites(VkDescriptorSet& descriptor_set, VkWriteDescriptorSet& desc_write)
{
	/*

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniform_buffers_[0]->buffer_->getVkBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	test_vec[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	test_vec[0].dstSet = descriptorSet;
	test_vec[0].dstBinding = 0;//이건 static 변수로 관리
	test_vec[0].dstArrayElement = 0;
	test_vec[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	test_vec[0].descriptorCount = 1;//
	test_vec[0].pBufferInfo = &bufferInfo;
	*/
	bufferInfo_.buffer = buffer_->getVkBuffer();
	bufferInfo_.offset = buffer_offset_;
	bufferInfo_.range = sizeof(buffer_size_);

	desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write.dstSet = descriptor_set;
	desc_write.dstBinding = binding_slot_;//이건 static 변수로 관리 
	desc_write.dstArrayElement = 0;
	desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	desc_write.descriptorCount = 1;//
	desc_write.pBufferInfo = &bufferInfo_;
}

//VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	//Buffer::createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
	//VkDeviceSize instancingBufferSize = sizeof(glm::mat4) * INSTANCING_BUFFER_SIZE;
	//Buffer::createBuffer(instancingBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, instancingBuffer, instancingBufferMemory);

UniformBuffer::UniformBuffer(uint32_t binding_slot, VkDeviceSize size, VkDeviceSize offset)
	: Object("uniform_buffer"), binding_slot_(binding_slot), buffer_size_(size), buffer_offset_(offset)
{
	buffer_ = std::make_shared<GADBuffer>(
		buffer_size_,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

}
