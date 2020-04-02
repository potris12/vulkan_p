#pragma once
#include "Camera.h"
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
		auto uniform_buffer = std::make_shared<UniformBufferT<T>>(data_num, binding_slot_);
		uniform_buffers_.push_back(uniform_buffer);
		++binding_slot_;

		return uniform_buffer;
	}

	void addUniformBuffer(std::shared_ptr<UniformBuffer> uniform_buffer)
	{
		uniform_buffers_.push_back(uniform_buffer);
		++binding_slot_;
	}

	std::shared_ptr<Texture> addTexture(const std::string& file_name);
	//add render objects

	//set buffer data
	void setUniformBufferData(uint32_t index, void* data);

	//create resources desc
	void createDescriptorPool();
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


	uint32_t binding_slot_ = Camera::kCameraBufferSlot;// binding slot 시작index는 Camera buffer slot (camera buffer slot 은 일단 공통으로 0임 uniform buffer의 첫번째는 무조건 이녀석 )
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

	std::weak_ptr<Camera> weak_camera_;
public:
	RenderContainer(std::weak_ptr<Camera> camera);
	~RenderContainer();
};

