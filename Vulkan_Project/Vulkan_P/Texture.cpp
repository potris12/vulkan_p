#include "Texture.h"
#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../std_image/stb_image.h"

const std::string Texture::file_path_ = "texture/";

void Texture::awake()
{
}

void Texture::start()
{
}

void Texture::update()
{
}

void Texture::destroy()
{
	//image

	vkDestroySampler(DEVICE_MANAGER->getDevice(), texture_sampler_, nullptr);
	vkDestroyImageView(DEVICE_MANAGER->getDevice(), image_view_, nullptr);
	vkDestroyImage(DEVICE_MANAGER->getDevice(), texture_image_, nullptr);
	vkFreeMemory(DEVICE_MANAGER->getDevice(), texture_imagememory_, nullptr);
	//image

}

void Texture::setDescWrites(VkDescriptorSet & descriptor_set, VkWriteDescriptorSet & desc_write)
{
	desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write.dstSet = descriptor_set;
	desc_write.dstBinding = 1;
	desc_write.dstArrayElement = 0;
	desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	desc_write.descriptorCount = 1;
	desc_write.pImageInfo = &image_info_;

}

void Texture::createTextureImage(uint32_t binding_slot, const std::string& file_name)
{
	binding_slot_ = binding_slot;

	stbi_uc* pixels = stbi_load(file_name.c_str(), &tex_width_, &tex_height_, &tex_channels_, STBI_rgb_alpha);
	image_size_ = tex_width_ * tex_height_ * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}


	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	Buffer::createBuffer(image_size_, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
	//we can then directly copy the pixel values that we got from the image loading library to the buffer
	void* data;
	vkMapMemory(DEVICE_MANAGER->getDevice(), staging_buffer_memory, 0, image_size_, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(image_size_));
	vkUnmapMemory(DEVICE_MANAGER->getDevice(), staging_buffer_memory);

	stbi_image_free(pixels);

	//create image
	createImage(tex_width_, tex_height_,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture_image_, texture_imagememory_);

	transitionImageLayout(texture_image_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL/*VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/);
	copyBufferToImage(staging_buffer, texture_image_, static_cast<uint32_t>(tex_width_), static_cast<uint32_t>(tex_height_));


	vkDestroyBuffer(DEVICE_MANAGER->getDevice(), staging_buffer, nullptr);
	vkFreeMemory(DEVICE_MANAGER->getDevice(), staging_buffer_memory, nullptr);

	//view
	image_view_ = createImageView(texture_image_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	//sampler
	createTextureSampler();

	image_info_ = {};
	image_info_.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info_.imageView = image_view_;
	image_info_.sampler = texture_sampler_;
}

void Texture::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(DEVICE_MANAGER->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(DEVICE_MANAGER->getDevice(), image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = DEVICE_MANAGER->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(DEVICE_MANAGER->getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(DEVICE_MANAGER->getDevice(), image, imageMemory, 0);
}

VkImageView Texture::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.format = format;

	VkImageView imageView;
	if (vkCreateImageView(DEVICE_MANAGER->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
	return imageView;
}

void Texture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = RENDERER->beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


	RENDERER->endSingleTimeCommands(commandBuffer);
}

void Texture::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = RENDERER->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;//mipmaplevel
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;//todo
	barrier.dstAccessMask = 0;//todo

/*
��� ������ ���������� ������� ������ ����� ����Ͽ� ����� ��� ���� ������ ù ��° �Ű������� ������ ������ �߻��ؾ� �ϴ� �۾��� � ���������� �ܰ迡�� �߻��ϴ��� ������
�ι�° �Ű������� �۾��� ������� ����ϴ� ���������� �ܰ踦 ������ ������ ���Ŀ� ������ �� �ִ� ���������� �ܰ�� ������ ������ ���ҽ� ��� ����� ���� �ٸ�
���Ǵ� ���� �� ����� ǥ�� ������
VK_ACCESS_UNIFORM_READ_BIT - �������� �������� ���
VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT - ���������� ���������� �ܰ�� ���� ���� ������ ���̴�
�� ������ ����� ���� ���̴��� �ƴ� ���������� �ܰ踦 �����ϴ� ���� �ǹ̰� ������ ��� ������ ��ġ���� �ʴ� ���������� �ܰ踦 ������ �� ��ȿ�� �˻� ���̾ ��� ?
����° �Ű������� 0 �Ǵ� VK_DEPENDENCY_BY_REGIOIN_BIT�� ���ڴ� ������� ������ �������� �ٲ� �̰��� ������ �̹� ���ݱ��� �ۼ��� �ڿ��� �Ϻο��� �б⸦ ������ �� ������ �ǹ���
������ ������ �谳������ �޸� ������, ���� �޸� ������ �� ���⼭ ����ϴ� �Ͱ� ���� �̹��� �޸� �������� ������ ������ ���������� ������ �迭�� ������
���� VkFormat�Ű������� ����ϰ� ������ ������ ���̹��� ���� Ư�� ��ȯ�� �� �Ű������� ����Ұ�
*/
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	RENDERER->endSingleTimeCommands(commandBuffer);
}

void Texture::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	/*
	MIRRORED_REPEAT, CLAMP_TO_EDGE, MIRROR_CLAMP_TO_EDGE, CLAMP_TO_BORDER
	*/
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;//false = 0-1 true = 0 - texWidth,texHeight
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;


	if (vkCreateSampler(DEVICE_MANAGER->getDevice(), &samplerInfo, nullptr, &texture_sampler_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

bool Texture::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

Texture::Texture() : Object("texture")
{
}


Texture::~Texture()
{
}
