#pragma once

#include "DeviceManager.h"
#include "Object.h"


class Texture : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();

	void setDescWrites(VkDescriptorSet& descriptor_set, VkWriteDescriptorSet& desc_write);
	void setDescSetLayout(VkDescriptorSetLayoutBinding& desc_set_layout);

	void createTextureImage(uint32_t binding_slot, const std::string& file_name);
	
	static bool hasStencilComponent(VkFormat format);
	
	static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory);
	static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void createTextureSampler();
	
private:
	uint32_t binding_slot_ = 0;

	/*image property*/
	static const std::string file_path_;
	std::string file_name_;
	int tex_width_, tex_height_, tex_channels_ = 0;
	VkDeviceSize image_size_ = 0;

	/* vk image property */
	VkFormat format_;
	VkImageTiling tiling_; 
	VkImageUsageFlags usage_;
	VkMemoryPropertyFlags properties_;
	VkImageType image_type_;
	VkImageLayout image_layout_;
	VkSampleCountFlagBits sampler_count_flagbit_;
	VkSharingMode sharing_mode_;

	/* vk image view property */
	VkImageAspectFlagBits image_aspect_flag_bits_;
	VkImageViewType image_view_type_;
	uint32_t base_mip_level_ = 0;
	uint32_t level_count_ = 1;
	uint32_t base_array_layer_ = 0;
	uint32_t layer_count = 1;

	/*vk Objects*/
	VkImageView image_view_;
	VkImage texture_image_;
	VkDeviceMemory texture_imagememory_;
	VkSampler texture_sampler_;
	VkDescriptorImageInfo image_info_;
public:
	Texture();
	virtual ~Texture();
};

