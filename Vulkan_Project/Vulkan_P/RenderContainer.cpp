#include "stdafx.h"
#include "RenderContainer.h"
#include "Camera.h"
#include "Timer.h"


void RenderContainer::awake()
{
	
}

void RenderContainer::start()
{
}

void RenderContainer::update()
{

	//TODO 
	//�̰� �� ��ü���� �ؾ���
	float dt = TIMER->getDeltaTime();

	//UPDATE���� �� ��ü�� ��ȸ�ϸ鼭 instance_data_�� ä�� 

	auto instancing_buffers = mesh_->addBufferDataStart();

	// Distribute rocks randomly on two different rings
	for (auto game_object : game_objects_)
	{
		game_object->update(dt);
		game_object->setBufferData(instancing_buffers);
	}

	//for (auto i = 0; i < INSTANCE_COUNT; i++) {
	//	//object�� �ڱⰡ �� �־�� ���� �˰��ִٴ� �����Ͽ� ���� 
	//	auto data = glm::translate(glm::mat4(1.0f), glm::vec3((i - INSTANCE_COUNT / 2) * 2, 0.0f, 0.0f)) * world;;
	//	instancing_buffers[0]->addBufferData(&data);
	//}

	//TODO �̺κ��� ���߿� Object���� �����Ҳ��� 
	//GameObject���� instance_data�� �Ѱ��ָ� �˾Ƽ� ó���ϵ���
	mesh_->addBufferDataEnd();
}

void RenderContainer::destroy()
{
	mesh_->destroy();
	//image
	for (auto texture : textures_) {
		texture->destroy();
	}
	//uniform buffers
	for (auto i = 1; i < uniform_buffers_.size(); ++i)
	{//0���� camera ���۷� ����
		uniform_buffers_[i]->destroy();
	}

	vkDestroyDescriptorPool(DEVICE_MANAGER->getDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(DEVICE_MANAGER->getDevice(), descriptorSetLayout, nullptr);
}

void RenderContainer::setMesh(std::shared_ptr<Mesh> mesh)
{
	mesh_ = mesh;
}

void RenderContainer::addGameObject(std::shared_ptr<GameObject> game_object)
{
	game_objects_.push_back(game_object);
}

std::shared_ptr<Texture> RenderContainer::addTexture(const std::string& file_name)
{
	auto texture = std::make_shared<Texture>();
	texture->createTextureImage(binding_slot_++, file_name);
	textures_.push_back(texture);
	return texture;
}

void RenderContainer::setUniformBufferData(uint32_t index, void* data)
{
	if (index < uniform_buffers_.size())
	{
		uniform_buffers_[0]->setBufferData(data);
	}
}

void RenderContainer::createDescriptorPool()
{
	auto& swapChainImages = DEVICE_MANAGER->getSwapChainImages();
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

	if (vkCreateDescriptorPool(DEVICE_MANAGER->getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void RenderContainer::createDescriptorSet()
{
	VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	//������ ��Ʈ�� ������ Ǯ�� �Ҹ�� �� �ڵ����� �����ǹǷ� ���� ������ �ʿ䰡 ����
	if (vkAllocateDescriptorSets(DEVICE_MANAGER->getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	std::vector< VkWriteDescriptorSet> vec;
	for (auto& test : vec) {
		test = {};
	}

	vec.resize(uniform_buffers_.size() + textures_.size());

	auto descriptor_writes_index = 0;
	for (auto uniform_buffer : uniform_buffers_) {
		uniform_buffer->setDescWrites(descriptorSet, vec[descriptor_writes_index++]);
	}

	for (auto texture : textures_) {
		texture->setDescWrites(descriptorSet, vec[descriptor_writes_index++]);
	}
	//
	//�̰� renderer �Լ����� ���� 

	vkUpdateDescriptorSets(DEVICE_MANAGER->getDevice(), static_cast<uint32_t>(descriptor_writes_index), vec.data(), 0, nullptr);
}

void RenderContainer::createDescriptorSetLayout()
{
	/*
	binding �Ǿ�� �� �༮���� ���⼭ ������
	 - ������۸� ����Ϸ��� ���⼭ �ϴ� ����ؾ���
	 sampler,
	*/
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;//Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(DEVICE_MANAGER->getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}

}

void RenderContainer::createGraphicsPipeline(VkRenderPass& render_pass)
{
	auto camera = weak_camera_.lock();
	if (nullptr == camera) return;


	Shader vs(ShaderType::VS, "shaders/vert.spv", "main");
	Shader ps(ShaderType::PS, "shaders/frag.spv", "main");

	VkPipelineShaderStageCreateInfo shaderStages[] = { vs.getDesc(), ps.getDesc() };
	//shader info

	//TODO 0313 vertex layout 
	//
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	mesh_->setVertexInputState(vertexInputInfo);

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	mesh_->setInputAssemblyState(inputAssembly);

	//camera manager info
	VkPipelineViewportStateCreateInfo viewportState = {};
	camera->setCameraDesc(viewportState);//camera �� ������ ���� 
	//camera manager info

	//������ ������
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	//multi sampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//blend op
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;//������� �� x ���̾ƿ��� ����.
	//pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;//�������, �ؽ��� ���� ����/ �̰��� ���� setLayoutCount

	if (vkCreatePipelineLayout(DEVICE_MANAGER->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;//optional
	depthStencil.maxDepthBounds = 1.0f;//optional
	//last three op is stencil i=option
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // optional
	depthStencil.back = {}; // optional

	/* pipeline info */
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	//mesh
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	//mesh
	//camera
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	//camera
	//pipeline
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	//pipeline
	pipelineInfo.pDynamicState = nullptr;//optional

										 //���� ���� ��� �ܰ踦 �����ϴ� ��� ������ ����
	pipelineInfo.layout = pipelineLayout;

	//�״��� ����ü �����Ͱ� �ƴ� vulkan �ڵ��� ���������� ���̾ƿ��� ����
	pipelineInfo.renderPass = render_pass;//�׸��� output�� ������ �� 
	pipelineInfo.subpass = 0;

	//���������� ������ �н��� �� �׷��� ������������ ���� �����н��� �ε����� ���� ������ ����
	/* �� Ư�� �ν��Ͻ� ��� �� ���������ο��� �ٸ� ������ �н��� ����� ���� ������ renderPass�� ȣȯ �����ؾ� �Ѵ�.
	*/
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	/* ������ �ΰ��� �Ű������� ������ basePipelineHandel, basePipelineIndex�� Vulkan�� ����ϸ� ���� ���������ο��� �Ļ��Ͽ� ���ο� �׷��� ������������ ���� �� ����
	������ ������ �Ļ����� ���� ���̵��� ���� ���������ΰ� ����� ����� ���� ���߰� ������������ �����ϴµ� ��� ����� �����ϰ� ������ �θ��� ���������ΰ� ��ȯ�� �� ���� ����� �� �ִ�
	basePipelineHandel�� ���� ������������ �ڵ��� �����ϰų� basePipelineIndex�� �ε����� ������� �ٸ� ������������ ������ �� �ֽ��ϴ�. ����� �����������ϳ����̹Ƿ� null�ڵ�� �ε��� -1�� �����ϱ⸸ �ϸ��
	Vk_PIPELINE_CREATE_DERIVATIVE_BIT�÷��װ� VkGraphicsPipelineCreateInfo�� �÷��� �ʵ忡�� ������ ��쿡�� ����
	*/
	//���� VkPipeLIne��ü�� ������ Ŭ���� ����� ����� ������ �ܰ踦 �غ�

	/* vkCreateGraphicsPipelines�Լ��� ������ vulkan�� �Ϲ����� ��ü ���� �Լ����� ���� �Ű������� ����
	�������� VKGraphicsPipelineCreateinfo��ü�� ���� �ͼ� �ѹ��� ȣ��� �������� VkPipeline��ü�� ����� ����!

	�ι��� �Ű������� ������ VkPipelineCache��ü�� ������ ���������� ĳ�ô� vkCreateGraphicsPipeline�� ���� ���� ȣ�⿡�� ���������� ������ ���õ� �����͸� �����ϰ� �����ϴµ� ����� �� ����
	ĳ�ð� ���Ͽ� ����Ǵ� ��� ���α׷� ���� ��ü���� ����� ���� �Ӥ����� ���� ���߿� ���������� ���� �ӵ��� ũ�� ���� �� ����
	*/
	if (vkCreateGraphicsPipelines(DEVICE_MANAGER->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void RenderContainer::createCommandBuffers(std::vector<VkFramebuffer>& swapchain_frame_buffer, VkRenderPass& render_pass, VkCommandPool& command_pool)
{
	commandBuffers.resize(swapchain_frame_buffer.size());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = command_pool;
	/*
	typedef enum VkCommandBufferLevel {
	VK_COMMAND_BUFFER_LEVEL_PRIMARY = 0, ������ ���� ��⿭�� ������ �� ������ �ٸ� ��� ���ۿ��� ȣ�� �� �� ����
	VK_COMMAND_BUFFER_LEVEL_SECONDARY = 1, ���� ������ ���� ������ �⺻ ��� ���ۿ��� ȣ���� �� ����
	VK_COMMAND_BUFFER_LEVEL_BEGIN_RANGE = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	VK_COMMAND_BUFFER_LEVEL_END_RANGE = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
	VK_COMMAND_BUFFER_LEVEL_RANGE_SIZE = (VK_COMMAND_BUFFER_LEVEL_SECONDARY - VK_COMMAND_BUFFER_LEVEL_PRIMARY + 1),
	VK_COMMAND_BUFFER_LEVEL_MAX_ENUM = 0x7FFFFFFF
	���� ��� ���� ����� ��������� ������ �⺻ ��� ���ۿ��� �Ϲ����� �۾��� ���� �� �� �ִٴ� �� ����ص���
	} VkCommandBufferLevel;*/
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	//�̺κ��� command buffer����� �κ��� 
	if (vkAllocateCommandBuffers(DEVICE_MANAGER->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}


	for (auto i = 0; i < commandBuffers.size(); ++i)
	{
		//��� ���� ��� ����
		/* ������� vkCmdBeginRenderPass�� ���� �н��� �����Ͽ� ���� �����н��� VKRenderPassBeginInfo����ü�� �Ϻ� �Ű������� ����Ͽ� ����
		*/
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = render_pass;
		renderPassInfo.framebuffer = swapchain_frame_buffer[i];
		//ù���� �Ű������� ���� �н� ��ü�� ���ε� �� ����ġ��Ʈ, �� ����ü�� �̹����� ���� color attachment�� �����ϴ� ������ ���۸� �����
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = DEVICE_MANAGER->getSwapChainExtent();
		//���� �� �Ű������� ������ ������ ũ�⸦ ���� ������ ������ ���̴� �ε� �� ������ ����Ǵ� ��ġ�� ����
		//�� ���� ���� �ȼ����� ���ǵ��� ���� ���� ���� �ֻ��� ������ ���� ����ġ��Ʈ�� ũ��� ��ġ�ؾ���
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.2f, 0.2f, 0.2f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VkCommandBufferBeginInfo commandBufferInfo = {};
		commandBufferInfo.pNext = nullptr;
		commandBufferInfo.pInheritanceInfo = nullptr;
		commandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;//
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(commandBuffers[i], &commandBufferInfo);

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		/* ���� ������ �н��� ������ �� ���� ����� ����ϴ� ��� ����� vkCmd���ξ�� �˾� �� �� ����
		��� void�� ������ �츮�� ����� ��ĥ �� ���� ���� ó���� ���� ����
		��� ����� ù��° �Ű������� �׻� ����� ����ϴ� ��� ������
		�ι�° �Ű������� ��� ������ ������ �н��� ���� ������ ����
		������ �Ű������� ���� �н� ������ ����� ����� �����ϴ� ����� ����
		VK_SUBPASS_CONTENTS_INLINE - ������ �н� ����� �⺻ ��� ���� ��ü�� ���ԵǸ� ���� ��� ���۴� ������� ����
		VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  - ���� �н� ����� ���� ��� ���ۿ��� ����
		*/
		//basic drawing commands
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);


		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		/*
		VkCommandBuffer                             commandBuffer,
		uint32_t                                    indexCount,
		uint32_t                                    instanceCount,
		uint32_t                                    firstIndex,
		int32_t                                     vertexOffset,
		uint32_t                                    firstInstance
		*/
		mesh_->draw(commandBuffers[i]);


		//command buffer�� ���� ����� ����ϴ� �� begin command buffer /end command buffer ���̿� �Ͼ�� �� 
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	}
}

VkCommandBuffer& RenderContainer::submissionBuffer(uint32_t imageIndex)
{
	game_objects_.clear();
	return commandBuffers[imageIndex];
}

void RenderContainer::cleanupSwapChain(VkCommandPool& command_pool)
{
	vkFreeCommandBuffers(DEVICE_MANAGER->getDevice(), command_pool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	vkDestroyPipeline(DEVICE_MANAGER->getDevice(), graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(DEVICE_MANAGER->getDevice(), pipelineLayout, nullptr);
}

RenderContainer::RenderContainer(std::weak_ptr<Camera> weak_camera)
	: Object("render_container"), weak_camera_(weak_camera)
{
	if (auto camera = weak_camera_.lock())
	{
		addUniformBuffer(camera->getCameraBuffer());
	}
}


RenderContainer::~RenderContainer()
{
}
