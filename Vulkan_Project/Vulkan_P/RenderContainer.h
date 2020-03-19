#pragma once
#include "Mesh.h"
#include "Object.h"
#include "GameObject.h"
#include "InstancingBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "Shader.h"

class RenderContainer : public Object
{
public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void destroy();

	//add render objects
	void setMesh(std::shared_ptr<Mesh> mesh);
	void addGameObject(std::shared_ptr<GameObject> game_object);

	template <typename T>
	std::shared_ptr<InstancingBuffer> addInstancingBuffer(int32_t data_num, const std::vector<VkFormat>& formats)
	{
		if (nullptr == mesh_) return nullptr;

		return mesh_->addInstancingBuffer<T>(data_num, formats);
	}

	template <typename T>
	std::shared_ptr<UniformBuffer> addUniformBuffer(uint32_t data_num)
	{
		static uint32_t binding_slot = 0;

		auto uniform_buffer = std::make_shared<UniformBufferT<T>>(data_num, binding_slot);
		uniform_buffers_.push_back(uniform_buffer);
		++binding_slot;

		return uniform_buffer;
	}

	std::shared_ptr<Texture> addTexture(const std::string& file_name);
	//add render objects

	//set buffer data
	void setUniformBufferData(uint32_t index, void* data);

	//create resources desc
	void createDescriptorSet();
	void createDescriptorSetLayout();

	//pipeline
	void createGraphicsPipeline(VkRenderPass& render_pass);
	//command buffer
	void createCommandBuffers(std::vector<VkFramebuffer>& swapchain_frame_buffer, VkRenderPass& render_pass, VkCommandPool& command_pool);

	//버퍼 제출
	VkCommandBuffer& submissionBuffer(uint32_t imageIndex);


	void cleanupSwapChain(VkCommandPool& command_pool);
private:
	std::shared_ptr<Mesh> mesh_ = nullptr;
	std::vector<std::shared_ptr<GameObject>> game_objects_;

	std::vector<std::shared_ptr<UniformBuffer>> uniform_buffers_;
	std::vector<std::shared_ptr<Texture>> textures_;

	VkDescriptorPool descriptorPool{};
	VkDescriptorSetLayout descriptorSetLayout{};
	VkDescriptorSet descriptorSet{};

	//pipeline
	VkPipelineLayout pipelineLayout{};
	VkPipeline graphicsPipeline{};

	//command buffer
	std::vector<VkCommandBuffer> commandBuffers;
public:
	RenderContainer();
	~RenderContainer();
};

