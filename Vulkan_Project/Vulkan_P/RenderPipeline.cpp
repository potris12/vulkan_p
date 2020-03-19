#include "RenderPipeline.h"



RenderPipeline::RenderPipeline()
{
}


RenderPipeline::~RenderPipeline()
{
}

void RenderPipeline::awake()
{
}

void RenderPipeline::update()
{
}

void RenderPipeline::destroy()
{
}

std::shared_ptr<UniformBuffer> RenderPipeline::addUniformBuffer(VkDeviceSize buffer_size, VkDeviceSize buffer_offset)
{
	return std::shared_ptr<UniformBuffer>();
}

void RenderPipeline::removeUniformBuffer(uint32_t binding_slot)
{
}

std::shared_ptr<Texture> RenderPipeline::addTexture(const std::string& file_name)
{
	return std::shared_ptr<Texture>();
}

void RenderPipeline::removeTexture(uint32_t binding_slot)
{
}
