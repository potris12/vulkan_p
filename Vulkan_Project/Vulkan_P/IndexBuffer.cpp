//#include "IndexBuffer.h"
//
//void IndexBuffer::awake()
//{
//	if (buffer_) buffer_->awake();
//	if (staging_buffer_) staging_buffer_->awake();
//}
//
//void IndexBuffer::start()
//{
//	if (buffer_) buffer_->start();
//	if (staging_buffer_) staging_buffer_->start();
//}
//
//void IndexBuffer::update()
//{
//	if (buffer_) buffer_->update();
//	if (staging_buffer_) staging_buffer_->update();
//}
//
//void IndexBuffer::destroy()
//{
//	if (buffer_) buffer_->destroy();
//	if (staging_buffer_) staging_buffer_->destroy();
//}
//
//void IndexBuffer::prepareBuffer(VkCommandPool & command_pool, void * data)
//{
//	staging_buffer_->mapWithUnmap(data);
//	buffer_->copyBuffer(command_pool, staging_buffer_);
//}
//
//void IndexBuffer::registeCommandBuffer(VkCommandBuffer & commandBuffer, VkDeviceSize offset)
//{
//	vkCmdBindIndexBuffer(commandBuffer, buffer_->getVkBuffer(), offset, index_type_);
//}
//
//
//
//IndexBuffer::IndexBuffer(VkDeviceSize size, VkIndexType index_type)
//	: Object("index_buffer"), buffer_size_(size), index_type_(index_type)
//{
//
//	buffer_ = std::make_shared<GADBuffer>(
//		buffer_size_,
//		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//
//	staging_buffer_ = std::make_shared<GADBuffer>(buffer_size_,
//		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//}
//
//IndexBuffer::~IndexBuffer()
//{
//}