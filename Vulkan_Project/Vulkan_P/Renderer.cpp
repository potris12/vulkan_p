#include "Renderer.h"
#include "DeviceManager.h"

void Renderer::awake()
{
	//
	createRenderPass();

	//
	createCommandPool();
	//render target
	createDepthResources();
	createFramebuffers();

	createDescriptorSetLayout();
	createDescriptorPool();

	//create mesh
	rect_mesh_ = std::make_shared<Mesh>(commandPool, "rect_mesh");
	rect_mesh_->awake();

	createUniformBuffer();
	createInstanceBuffer();
	createTexture();
	
	createDescriptorSet();
	
	createGraphicsPipeline();

	createCommandBuffers();

	createSemaphores();
}

void Renderer::update()
{
	//TODO 
	//�̰� �� ��ü���� �ؾ���
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


	
	//UPDATE���� �� ��ü�� ��ȸ�ϸ鼭 instance_data_�� ä�� 
	auto instancing_buffers = rect_mesh_->addBufferDataStart();

	glm::mat4 world = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));	
	// Distribute rocks randomly on two different rings
	for (auto i = 0; i < INSTANCE_COUNT; i++) {
		//object�� �ڱⰡ �� �־�� ���� �˰��ִٴ� �����Ͽ� ���� 
		auto data = glm::translate(glm::mat4(1.0f), glm::vec3((i - INSTANCE_COUNT / 2) * 2, 0.0f, 0.0f)) * world;;
		instancing_buffers[0]->addBufferData(&data);
	}

	//TODO �̺κ��� ���߿� Object���� �����Ҳ��� 
	//GameObject���� instance_data�� �Ѱ��ָ� �˾Ƽ� ó���ϵ���
	rect_mesh_->addBufferDataEnd();
}

void Renderer::destroy()
{

	vkDestroySemaphore(DEVICE_MANAGER->getDevice(), renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(DEVICE_MANAGER->getDevice(), imageAvailableSemaphore, nullptr);


	//take
	rect_mesh_->destroy();
	//uniform buffers
	for (auto uniform_buffer : uniform_buffers_) {
		uniform_buffer->destroy();
	}

	cleanupSwapChain();

	//image
	for (auto texture : textures_) {
		texture->destroy();
	}
	//image


//take
	vkDestroyDescriptorPool(DEVICE_MANAGER->getDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(DEVICE_MANAGER->getDevice(), descriptorSetLayout, nullptr);
	vkDestroyCommandPool(DEVICE_MANAGER->getDevice(), commandPool, nullptr);
}

//take
std::shared_ptr<Texture> Renderer::addTexture(const std::string & file_name)
{
	static uint32_t binding_slot = 0;

	auto texture = std::make_shared<Texture>();
	texture->createTextureImage(binding_slot++, file_name);
	textures_.push_back(texture);
	return texture;
}

//take
//�������� ���ε��� �༮�� 
void Renderer::createDescriptorSetLayout()
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

#define INSTANCING_BUFFER_SIZE 10

void Renderer::createUniformBuffer()
{
	addUniformBuffer<UniformBufferObject>(1);
	//VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	//Buffer::createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
	//VkDeviceSize instancingBufferSize = sizeof(glm::mat4) * INSTANCING_BUFFER_SIZE;
	//Buffer::createBuffer(instancingBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, instancingBuffer, instancingBufferMemory);
}

void Renderer::createInstanceBuffer()
{
	instance_data_.resize(INSTANCE_COUNT);
	/*
		std::default_random_engine rndGenerator(benchmark.active ? 0 : (unsigned)time(nullptr));
		std::uniform_real_distribution<float> uniformDist(0.0, 1.0);
		std::uniform_int_distribution<uint32_t> rndTextureIndex(0, textures.rocks.layerCount);
	*/
	// Distribute rocks randomly on two different rings
	for (auto i = 0; i < INSTANCE_COUNT; i++) {

		instance_data_[i].world_mtx = glm::translate(glm::mat4(1.0f), glm::vec3((i - INSTANCE_COUNT / 2) * 2, 0.0f, 0.0f));
	}

	rect_mesh_->addInstancingBuffer<InstanceData>(INSTANCE_COUNT, 
		{
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT
		}
	);

	//instancing_buffer->prepareBuffer(commandPool, (void*)instance_data_.data());
}

void Renderer::createDescriptorPool()
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

void Renderer::createDescriptorSet()
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

void Renderer::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.world = glm::mat4(1.0f);//glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	float aspect = (float)swapChainExtent.width / (float)swapChainExtent.height;
	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

	ubo.proj[1][1] *= -1;//glm �� ���� opengl������ ����Ǿ����Ƿ� Ŭ�� ��ǥ�� y��ǥ�� ������ �̷��� ���ϸ� �̹����� ���Ʒ� ������

	uniform_buffers_[0]->setBufferData(&ubo);
	//void* data;
	//vkMapMemory(DEVICE_MANAGER->getDevice(), uniformBufferMemory, 0, sizeof(ubo), 0, &data);
	//memcpy(data, &ubo, sizeof(ubo));
	//vkUnmapMemory(DEVICE_MANAGER->getDevice(), uniformBufferMemory);
	//uniform_buffer_->prepareBuffer();

	rect_mesh_->update();
}

void Renderer::createTexture()
{
	addTexture("texture/texture2.jpg");
}

void Renderer::drawFrame()
{
	auto imageIndex = DEVICE_MANAGER->getImageIndex();
	updateUniformBuffer();
	//if (chooseSwapExtent().height == 0 || chooseSwapExtent().width) return;

	VkResult result = vkAcquireNextImageKHR(DEVICE_MANAGER->getDevice(), DEVICE_MANAGER->getSwapChain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	/* ����ü���� �̹����� ȹ���Ϸ��� �õ��� �� ������ ������ �Ǹ�Ǹ� �� �̻� ���������̼� �� �� ���� ���� �츮�� ��� ����ü���� �ٽ� �����
	���� dawFrameȣ���� �ٽ� �õ��ؾ� ��
	���� ü���� �������� ��� �̸� �����ϱ�� ������ ���� ������ �̹� �̹����� ȹ���߱� ������ ��� �����ϵ��� �����߽� */
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	

	/*
	vkAcquireNextImageKHR �� ó�� �� �Ű������� �̹����� ���������� �� ��ġ�� ����ü���� ����° �Ű������� �̹����� ��밡���ϰ� �Ǵ� �ð��� �����ʷ� ����
	���� �� �Ű� ������ ���������̼� ������ �̹��� ����� ��ġ�� ��ȣ�� ���� ����ȭ ��ü�� ����
	�װ��� �츮�� �׸��� ������ �� �ִ� ������ ��������, �潺 �� �� ������ �� ���� �츮�� ���⼭ �Ʊ��� �����
	������ �Ű������� ��� �����ϰ� �� ����ü�� �̹����� �ε����� ����ϴ� ������ ����
	�ε����� swapChainImages�迭�� vKiMAGE�� ���� �� �ε����� ����Ͽ� �ùٸ� ��� ���۸� ����
	*/
	//��� ���� ����
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	/*
	ó�� �� �Ű������� ������ ���۵Ǳ� ���� ����� ���������� �ܰ迡�� ��ٸ� ������� ����
	������ ����� ������ �̹����� ���� ���� �;��ϹǷ� color attchment�� ���� �׷��� ������������ �ܰ踦 �����ϰ� ����
	�̴� �̷������� �̤Ӥ��� ������ ���� �Ϸ���� ���� ���¿� �ñ����� �̹� ���ؽ� ���̴� ���� ������ �� ������ �ǹ�
	wiatStages�迭�� �� �׸��� pWaitSemaphores���� ������ ������ ���� ������� �ش���

	���� ��
	*/
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	/*
	���� �� �Ű������� ������ ���� ������ ������ ��� ���۸� ����
	�ռ� ����ߵ��� ��� ������ ����ü���̹����� ���� ÷�η� ���ε��ϴ� ��ɹ��۸� ����
	*/
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	/*
	signalSemaphore�� ��� ���۰� ������ �Ϸ��ϸ� ��ȣ�� ���� ������� ����
	*/
	if (vkQueueSubmit(DEVICE_MANAGER->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	/*
	d���� vkQueue Submit �� ����Ͽ� ��� ���۸� �׷��� ��⿭�� ������ �� ���� �� �Լ��� �۾� ���ϰ� �ξ� Ŭ �� vkSubmitInfo����ü �迭�� ȿ������ ���� �μ��� �����
	������ �Ű������� ��� ���۰� ���ष�� �Ϸ��� �� ��ȣ�� ���� ������ �潺�� ������ ����ȭ�� ���� ������� ����ϱ� ������ VK_NULL_HANDLE�� ������

	*/

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { DEVICE_MANAGER->getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	//�̹����� ǥ���� ���� ü�ΰ� �� ���� ü���� �̹��� �ε����� ���� �̰� ���� �ϳ���

	presentInfo.pResults = nullptr;
	/* pResults ��� ������ ������ �Ű������� �ϳ����� �̰� ���������̼��� ������ ��� VkResult���� �迭��
	�����Ͽ� ��� ���� ����ü���� �˻��� �� ���� ���� �Լ��� ��ȯ ���� ����� �� �ֱ� ������ ���� ����ü�� �� ����ϴ°�� �ʿ����� ����
	*/

	result = vkQueuePresentKHR(DEVICE_MANAGER->getPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	/* vkQueuePresentKHR �Լ��� ����ü�ο� �̹����� ǥ���϶�� ��û�� ����
	���� �忡�� vkAcquireNextImageKHR�� vkQueuePresentKHR��ο� ���� ���� ó���� �߰��� ��
	�ֳĸ� ���д� ���α׷��� ����� �̾����� �ϱ⶧���� */

	/* 1203 ���� �ﰢ���� �׷��� drawFrame�� ��� �۾��� �񵿱��� �� mainLoop���� ������ �����ص� �׸��� �� ���������̼� �۾��� ��� ����� �� ����
	�׷����� �Ͼ�� ���� �ڿ��� �����ϴ°� ������ ����

	�� ������ �ذ��ϱ� ���� mainLoop�� �����ϰ� â�� �����ϱ� ���� �� ��ġ�� �۾��� ��ĥ �� ���� ��ٷ�����
	*/

	//����ȭ �׸� �׸��� wait
	vkQueueWaitIdle(DEVICE_MANAGER->getPresentQueue());//�̰ŵ� ���� ������ �Ȱ��� ��ȯ�� 

													   /*
													   �̷��� �׸��°� ȿ�����̴� ���� �������� �������Ǵ� ���� ���ӿ��� AI��ƾ�� �����ϴ� �� �������α׷��� ���¸� ������Ʈ �� ������
													   �̷��� �ϸ� �׻� gpu cpu busy�� ������ ������
													   updateAppState();

													   vkQueueWaitIdle(presentQueue);

													   vkAcquireNextImageKHR(...)

													   submitDrawCommands();

													   vkQueuePresentKHR(presentQueue, &presentInfo);
													   */
}

VkCommandBuffer& Renderer::getCurCommandBuffer()
{
	return commandBuffers[DEVICE_MANAGER->getImageIndex()];
}

void Renderer::createRenderPass()
{
	//render target �ʱ�ȭ 
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = DEVICE_MANAGER->getSwapChainImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//depth, stencil,  
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	/*
	���� �н��� ���� �н��� �̹��� ���̾ƿ� ��ȯ�� �ڵ����� ó����
	�̷��� ��ȯ�� �����н� ���� �޸𸮹� ���� ���Ӽ��� �����ϴ� ���� �н� ���Ӽ��� ���� �����
	������ �ϳ��� �����н� �� �������� �����н� ������ �۾��� �Ͻ��� �����н��� ����
	������ �н��� ���۰� ������ �н��� ������ ��ȯ�� ó���ϴ� �� ���� �⺻ ���Ӽ��� ������, ���ÿ� �߻����� ����
	�װ��� ��ȯ�� ������������ ���ۿ��� �߻��Ѵٰ� ���������� �� �������� ���� �̹����� ���� ���߾ �׷�
	�� ������ ó���ϴµ� �ΰ��� ����� ����
	�̹����� ����� �� ���� ������ ������ �н��� ���۵��� �ʵ��� �ϰų�, Ư�� �ܰ谡�� ���� �н��� ����ų �� ����

	*/

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	/*
	ó�� �� �ʵ�� ���Ӽ��� ���� �����н��� ������ ���� Ư���� VK_SUBPASS_EXTERNAL�� srcSubpass�Ǵ� dstSubpass�� �����Ǿ����� ���ο� ���� ���� �н� ������ �Ͻ��� ���� �н��� ����������
	�ε��� 0 �� ó������ ������ �ϳ��� �� �츮�� �����н��� ������
	dstSubpass�� ���Ӽ� �׷������� ��ȯ�� �����ϱ� ���� �׻� srcSubpass���� Ŀ����
	*/
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	/*
	���� �� �ʵ�� ����� �۾��� �̷��� �۾��� ����Ǵ� �ܰ踦 ����
	����ü���� �̹����� �����ϱ� ���� ����ü���� �̹��� �б⸦ �����⸦ ��ٷ��� ��
	�� �۾��� ���� ÷�� ��� �ܰ���ü�� ��٤Ť� ������ �� ����
	*/
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	/*
	�� �۾��� ��ٷ��� �ϴ� �۾��� �÷� ����ġ��Ʈ �ܰ迡 ������ �÷� ����ġ��Ʈ�� �а� ���� �۾��� �ʿ���
	�̷��� ������ ������ �ʿ��ϰ� �Ͼ�� ��ȭ�� ������
	*/

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(DEVICE_MANAGER->getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void Renderer::createGraphicsPipeline()
{
	Shader vs(ShaderType::VS, "shaders/vert.spv", "main");
	Shader ps(ShaderType::PS, "shaders/frag.spv", "main");

	VkPipelineShaderStageCreateInfo shaderStages[] = { vs.getDesc(), ps.getDesc() };
	//shader info

	//TODO 0313 vertex layout 
	//
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	rect_mesh_->setVertexInputState(vertexInputInfo);

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	rect_mesh_->setInputAssemblyState(inputAssembly);

	//camera info
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	//camera info

	//camera manager info
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
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
	pipelineInfo.renderPass = renderPass;//�׸��� output�� ������ �� 
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

void Renderer::createFramebuffers()
{
	//��� ������ ���۸� ������ �� ���ֵ��� �����̳��� ũ�⸦ �����Ͽ� ����!
	auto& swapChainImageViews = DEVICE_MANAGER->getSwapChainImageViews();
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	swapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		/*VkImageView attachments[] = {
			swapChainImageViews[i]
		};*/

		std::array<VkImageView, 2> attachments = {
			swapChainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(DEVICE_MANAGER->getDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
	/* ���� ���� ������ ���� ����
	render Pass�� ������ ���۰� ȣȯ�Ǿ�� �ϴ����� �����ؾ���
	������ ���۴� ȣȯ ������ ������ �н��� �Բ��� ����� �� ����
	�̴� �뷫 ������ ������ Ÿ���� ����ġ��Ʈ�� ����ؾ� �Ѵٴ� ���� �ǹ�
	attachmentCount �� pAttachments�Ű� ������ ������ �н� pAttachment �迭�� �� ÷�� ���� ���ε� �Ǿ�� �ϴ� VkImageView��ü�� ����
	width �� height �Ű� ������ ��ü ������ �����ϸ� ���̾�� �̹��� �迭�� ���̾� ���� ��Ÿ��
	����ü�� �̹����� ���� �̹��� �̹Ƿ� ���̾���� 1

	�������� ��*/
}

void Renderer::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = DEVICE_MANAGER->findQueueFamilies(DEVICE_MANAGER->getPhysicalDevice());

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0;//VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	/* ��� ���۴� �츮�� �˻��� �׷��� �� ���������̼� ��⿭�� ���� ��ġ ��⿭ �� �ϳ��� �̸� �����Ͽ� �����
	�� ��� Ǯ�� ���� ������ ť�� ����� ��� ���۸� �Ҵ� �� �� ����
	�츮�� �׸� �׸��� ����� ����� ���̱� ������ �׷��ȴ�⿭ �йи��� �����ؾ���

	��� Ǯ���� �ΰ��� �÷��װ� ����
	VK_COMMAND_POOL_CREATE_TRANSIENT_BIT ���ο� Ŀ�ǵ�� Ŀ�ǵ� ���۰� ���� �ٽ� ��ϵɰ��� �Ͻ� (�޸� �Ҵ� ������ �����ų �� ����
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ��� ���۸� ������������ ���� �� �� ���� �� �÷��׸� ��������� ������
	��� �ٽ� �����ؾ� ��*/

	if (vkCreateCommandPool(DEVICE_MANAGER->getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void Renderer::createCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
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


	for (auto i = 0; i<commandBuffers.size(); ++i)
	{
		//��� ���� ��� ����
		/* ������� vkCmdBeginRenderPass�� ���� �н��� �����Ͽ� ���� �����н��� VKRenderPassBeginInfo����ü�� �Ϻ� �Ű������� ����Ͽ� ����
		*/
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
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
		commandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
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

		rect_mesh_->draw(commandBuffers[i]);


		//command buffer�� ���� ����� ����ϴ� �� begin command buffer /end command buffer ���̿� �Ͼ�� �� 
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	}
}

void Renderer::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(DEVICE_MANAGER->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(DEVICE_MANAGER->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphores!");
	}
}

void Renderer::cleanupSwapChain()
{
	vkDestroyImageView(DEVICE_MANAGER->getDevice(), depthImageView, nullptr);
	vkDestroyImage(DEVICE_MANAGER->getDevice(), depthImage, nullptr);
	vkFreeMemory(DEVICE_MANAGER->getDevice(), depthImageMemory, nullptr);

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(DEVICE_MANAGER->getDevice(), swapChainFramebuffers[i], nullptr);
	}
	vkFreeCommandBuffers(DEVICE_MANAGER->getDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	vkDestroyPipeline(DEVICE_MANAGER->getDevice(), graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(DEVICE_MANAGER->getDevice(), pipelineLayout, nullptr);
	vkDestroyRenderPass(DEVICE_MANAGER->getDevice(), renderPass, nullptr);

	DEVICE_MANAGER->cleanupSwapChain();
}

void Renderer::recreateSwapChain()
{
	vkDeviceWaitIdle(DEVICE_MANAGER->getDevice());

	cleanupSwapChain();

	DEVICE_MANAGER->recreateSwapChain();
	createRenderPass();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
	/* vkDeviceWaitIdle�� ȣ���ϱ� ������ ������� ���ҽ��� �������� ����
	��ǥ ����ü�� ��ü�� �ٽ� ����°�
	�̹��� ��� ����ü�� �̹����� ���� ������� �ϹǷ� �ٽ� ��������
	���� �н��� ����ü�� �̹��� ���Ŀ� ���� �ٸ��� ������ �ٽ� �ۼ��ؾ���
	����ü�� �̹��� ������ â ũ�� ������ ���� �۾� �߿� ����Ǵ� ���� ���� ������ ������ ó���ؾ���
	����Ʈ �� ���� ũ��� �׷��� ���������� ���� �� �����ǹǷ� ���������ε� �ٽ� �ۼ�
	����Ʈ�� ���� �簢���� ���� ���¸� ����Ͽ� �̸� ���� �� ����
	���������� ������ ���ۿ� ��� ���۴� ����ü�� �̹����� ���� ������

	�̷��� ��ü���� �ٽ� ����� ���� ������ ���ҽ��� �����ؾ� �ϴµ� �̸� �Ϸ��� cleanup�ڵ��� �Ϻθ� recreateS
	*/
}

VkCommandBuffer Renderer::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(DEVICE_MANAGER->getDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Renderer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo(submitInfo) = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(DEVICE_MANAGER->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(DEVICE_MANAGER->getGraphicsQueue());

	vkFreeCommandBuffers(DEVICE_MANAGER->getDevice(), commandPool, 1, &commandBuffer);
}

void Renderer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void Renderer::createDepthResources()
{
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	VkFormat depthFormat = findDepthFormat();
	Texture::createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = Texture::createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT/*VK_IMAGE_ASPECT_DEPTH_BIT*/);

	Texture::transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

VkFormat Renderer::findDepthFormat()
{
	return DEVICE_MANAGER->findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool Renderer::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

Renderer::Renderer() : CSingleTonBase<Renderer>("Renderer")
{

}


Renderer::~Renderer()
{

}
