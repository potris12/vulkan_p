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
	//이건 각 객체에서 해야함
	float dt = TIMER->getDeltaTime();

	//UPDATE에서 각 객체를 순회하면서 instance_data_를 채움 

	auto instancing_buffers = mesh_->addBufferDataStart();

	// Distribute rocks randomly on two different rings
	for (auto game_object : game_objects_)
	{
		game_object->update(dt);
		game_object->setBufferData(instancing_buffers);
	}

	//for (auto i = 0; i < INSTANCE_COUNT; i++) {
	//	//object가 자기가 뭘 넣어야 할지 알고있다는 가정하에 진행 
	//	auto data = glm::translate(glm::mat4(1.0f), glm::vec3((i - INSTANCE_COUNT / 2) * 2, 0.0f, 0.0f)) * world;;
	//	instancing_buffers[0]->addBufferData(&data);
	//}

	//TODO 이부분은 나중에 Object에게 위임할꺼임 
	//GameObject에게 instance_data를 넘겨주면 알아서 처리하도록
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
	{//0번은 camera 버퍼로 공용
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

	//설명자 세트는 설명자 풀이 소멸될 떄 자동으로 해제되므로 따로 정리할 필요가 없음
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
	//이건 renderer 함수에서 진행 

	vkUpdateDescriptorSets(DEVICE_MANAGER->getDevice(), static_cast<uint32_t>(descriptor_writes_index), vec.data(), 0, nullptr);
}

void RenderContainer::createDescriptorSetLayout()
{
	/*
	binding 되어야 할 녀석들을 여기서 정의함
	 - 상수버퍼를 사용하려면 여기서 일단 등록해야함
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
	camera->setCameraDesc(viewportState);//camera 가 없을리 없음 
	//camera manager info

	//레스터 라이저
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
	pipelineLayoutInfo.setLayoutCount = 1;//상수버퍼 수 x 레이아웃의 수임.
	//pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;//상수버퍼, 텍스쳐 등의 정보/ 이것의 수가 setLayoutCount

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

										 //다음 공정 기능 단계를 설명하는 모든 구조를 참조
	pipelineInfo.layout = pipelineLayout;

	//그다은 구조체 포인터가 아닌 vulkan 핸들인 파이프라인 레이아웃이 나옴
	pipelineInfo.renderPass = render_pass;//그림의 output이 나오는 곳 
	pipelineInfo.subpass = 0;

	//마지막으로 랜더링 패스와 이 그래픽 파이프라인이 사용될 하위패스의 인덱스에 대한 참조가 있음
	/* 이 특정 인스턴스 대신 이 파이프라인에서 다른 랜더링 패스를 사용할 수도 있지만 renderPass와 호환 가능해야 한다.
	*/
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	/* 실제로 두개의 매개변수가 더있음 basePipelineHandel, basePipelineIndex임 Vulkan을 사용하면 기존 파이프라인에서 파생하여 새로운 그래픽 파이프라인을 만들 수 있음
	파이프 ㅏㄹ인 파생물에 대한 아이디어는 기존 파이프라인과 공통된 기능을 많이 갖추고 파이프라인을 설정하는데 드는 비용이 저렴하고 동일한 부모의 파이프라인간 전환도 더 빨리 수행될 수 있다
	basePipelineHandel로 기존 파이프라인의 핸들을 지정하거나 basePipelineIndex로 인덱스로 만들려는 다른 파이프라인을 참조할 수 있습니다. 현재는 파이프라인하나뿐이므로 null핸들과 인덱스 -1을 지정하기만 하면됨
	Vk_PIPELINE_CREATE_DERIVATIVE_BIT플래그가 VkGraphicsPipelineCreateInfo의 플래그 필드에도 지정된 경우에만 사용됨
	*/
	//이제 VkPipeLIne객체를 보유할 클래스 멤버를 만들어 마지막 단계를 준비

	/* vkCreateGraphicsPipelines함수는 실제로 vulkan의 일반적인 객체 생성 함수보다 많은 매개변수를 가미
	여러개의 VKGraphicsPipelineCreateinfo객체를 가져 와서 한번의 호출로 여러개의 VkPipeline개체를 만들수 있음!

	두번쨰 매개변수는 선택적 VkPipelineCache개체를 참조함 파이프라인 캐시는 vkCreateGraphicsPipeline에 대한 여러 호출에서 파이프라인 생성과 관련된 데이터를 저장하고 재사용하는데 사용할 수 있음
	캐시가 파일에 저장되는 경우 프로그램 실행 전체에서 사용할 수도 ㅣㅇㅆ음 따라서 나중에 파이프라인 생성 속도를 크게 높일 수 있음
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
	VK_COMMAND_BUFFER_LEVEL_PRIMARY = 0, 실행을 위해 대기열에 제출할 수 있지만 다른 명령 버퍼에는 호출 할 수 없음
	VK_COMMAND_BUFFER_LEVEL_SECONDARY = 1, 직접 제출할 수는 없지만 기본 명령 버퍼에서 호출할 수 있음
	VK_COMMAND_BUFFER_LEVEL_BEGIN_RANGE = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	VK_COMMAND_BUFFER_LEVEL_END_RANGE = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
	VK_COMMAND_BUFFER_LEVEL_RANGE_SIZE = (VK_COMMAND_BUFFER_LEVEL_SECONDARY - VK_COMMAND_BUFFER_LEVEL_PRIMARY + 1),
	VK_COMMAND_BUFFER_LEVEL_MAX_ENUM = 0x7FFFFFFF
	보조 명령 버퍼 기능을 사용하지는 않지만 기본 명령 버퍼에서 일반적인 작업을 재사용 할 수 있다는 것 기억해두자
	} VkCommandBufferLevel;*/
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	//이부분이 command buffer만드는 부분임 
	if (vkAllocateCommandBuffers(DEVICE_MANAGER->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}


	for (auto i = 0; i < commandBuffers.size(); ++i)
	{
		//명령 버퍼 기록 시작
		/* 드로잉은 vkCmdBeginRenderPass로 렌더 패스를 시작하여 시작 랜더패스는 VKRenderPassBeginInfo구조체의 일부 매개변수를 사용하여 구성
		*/
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = render_pass;
		renderPassInfo.framebuffer = swapchain_frame_buffer[i];
		//첫번쨰 매개변수는 랜더 패스 자체와 바인딩 할 어테치먼트, 각 스왑체인 이미지에 대해 color attachment로 지정하는 프레임 버퍼를 만들었
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = DEVICE_MANAGER->getSwapChainExtent();
		//다음 두 매개변수는 랜더링 여역의 크기를 정의 렌더링 영역은 셰이더 로드 및 저장이 수행되는 위치를 정의
		//이 영역 밖의 픽셀에는 정의되지 않은 값이 있음 최상의 성능을 위해 어테치먼트의 크기와 일치해야함
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
		/* 이제 렌더링 패스를 시작할 수 있음 명령을 기록하는 모든 기능은 vkCmd접두어로 알아 볼 수 있음
		모둔 void를 리턴함 우리가 기록을 마칠 떄 까지 오류 처리가 없을 것임
		모든 명령의 첫번째 매개변수는 항상 명령을 기록하는 명령 버퍼임
		두번째 매개변수는 방금 제공한 렌더링 패스의 세부 정보를 지정
		마지막 매개변수는 랜더 패스 내에서 드로잉 명령을 제공하는 방법을 제어
		VK_SUBPASS_CONTENTS_INLINE - 렌더링 패스 명령은 기본 명령 버퍼 자체에 포함되며 보조 명령 버퍼는 실행되지 않음
		VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  - 렌더 패스 명령은 보조 명령 버퍼에서 실행
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


		//command buffer에 뭔가 명령을 기록하는 건 begin command buffer /end command buffer 사이에 일어나야 함 
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
