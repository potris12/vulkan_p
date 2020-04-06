#include "Renderer.h"
#include "DeviceManager.h"
#include "Timer.h"

void Renderer::awake()
{
	//
	createRenderPass();

	//
	createCommandPool();
	//render target
	createDepthResources();
	createFramebuffers();

	//TODO UPDATER ���� ���� create object
	for (auto i = 0; i < INSTANCE_COUNT; i++) {
		game_objects_.push_back(std::make_shared<GameObject>(i));
	}

	//create mesh
	auto& swapChainExtent = DEVICE_MANAGER->getSwapChainExtent();
	camera_ = std::make_shared<Camera>(
		//camera info
		VkViewport{
			0.0f,
			0.0f,
			(float)swapChainExtent.width,
			(float)swapChainExtent.height,
			0.0f,
			1.0f
		},
		VkRect2D{
			{ 0, 0 },
			swapChainExtent
		}
	);

	render_container_ = std::make_shared<RenderContainer>(camera_);
	render_container_->setMesh(std::make_shared<Mesh>(commandPool, "rect_mesh"));

	render_container_->addTexture("texture/texture2.jpg");

	// create instancing buffer 
	render_container_->addInstancingBuffer<InstanceData>(INSTANCE_COUNT,
		{
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT
		}
	);
	render_container_->createDescriptorPool();
	render_container_->createDescriptorSetLayout();
	render_container_->createDescriptorSet();

	render_container_->createGraphicsPipeline(renderPass);
	render_container_->createCommandBuffers(swapChainFramebuffers, renderPass, commandPool);

	createSemaphores();
}

void Renderer::update()
{
	//TODO UPDATER���� �����ؾ��� �׸� �׸� ��ü�� ������ �ش� ��ü�� ������ �ִ� RenderContainer���� add
	for (auto game_object : game_objects_)
	{
		render_container_->addGameObject(game_object);
	}
	render_container_->update();
}

void Renderer::destroy()
{

	vkDestroySemaphore(DEVICE_MANAGER->getDevice(), renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(DEVICE_MANAGER->getDevice(), imageAvailableSemaphore, nullptr);

	render_container_->destroy();


	cleanupSwapChain();

	vkDestroyCommandPool(DEVICE_MANAGER->getDevice(), commandPool, nullptr);
}

void Renderer::updateUniformBuffer()
{
	camera_->update();

	//void* data;
	//vkMapMemory(DEVICE_MANAGER->getDevice(), uniformBufferMemory, 0, sizeof(ubo), 0, &data);
	//memcpy(data, &ubo, sizeof(ubo));
	//vkUnmapMemory(DEVICE_MANAGER->getDevice(), uniformBufferMemory);
	//uniform_buffer_->prepareBuffer();

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
	std::vector<VkCommandBuffer> command_buffers;
	//for(auto render_container : render_containers_)
	command_buffers.push_back(render_container_->submissionBuffer(imageIndex));
	submitInfo.commandBufferCount = command_buffers.size();
	submitInfo.pCommandBuffers = command_buffers.data();
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
	
	render_container_->cleanupSwapChain(commandPool);
	vkDestroyRenderPass(DEVICE_MANAGER->getDevice(), renderPass, nullptr);

	DEVICE_MANAGER->cleanupSwapChain();
}

void Renderer::recreateSwapChain()
{
	vkDeviceWaitIdle(DEVICE_MANAGER->getDevice());

	cleanupSwapChain();

	DEVICE_MANAGER->recreateSwapChain();
	createRenderPass();
	createDepthResources();
	createFramebuffers();

	render_container_->createGraphicsPipeline(renderPass);
	render_container_->createCommandBuffers(swapChainFramebuffers, renderPass, commandPool);
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
