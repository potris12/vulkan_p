#include "Renderer.h"
#include "DeviceManager.h"


void Renderer::awake()
{
	createSwapChain();
	createImageViews();
	createRenderPass();

	//uniform buffer
	createDescriptorSetLayout();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();
	//uniform buffer

	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();

	createCommandBuffers();
	//create mesh
	rect_mesh_ = std::make_shared<Mesh>("rect_mesh");
	rect_mesh_->awake();

	createSemaphores();
}

void Renderer::update()
{

	//��� ���� ��� ����
	/* ������� vkCmdBeginRenderPass�� ���� �н��� �����Ͽ� ���� �����н��� VKRenderPassBeginInfo����ü�� �Ϻ� �Ű������� ����Ͽ� ����
	*/
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex_];
	//ù���� �Ű������� ���� �н� ��ü�� ���ε� �� ����ġ��Ʈ, �� ����ü�� �̹����� ���� color attachment�� �����ϴ� ������ ���۸� �����
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = swapChainExtent;
	//���� �� �Ű������� ������ ������ ũ�⸦ ���� ������ ������ ���̴� �ε� �� ������ ����Ǵ� ��ġ�� ����
	//�� ���� ���� �ȼ����� ���ǵ��� ���� ���� ���� �ֻ��� ������ ���� ����ġ��Ʈ�� ũ��� ��ġ�ؾ���
	VkClearValue clearValue = {};
	clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearValue;

	VkCommandBufferBeginInfo commandBufferInfo = {};
	commandBufferInfo.pNext = nullptr;
	commandBufferInfo.pInheritanceInfo = nullptr;
	commandBufferInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(commandBuffers[imageIndex_], &commandBufferInfo);

	vkCmdBeginRenderPass(commandBuffers[imageIndex_], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	/* ���� ������ �н��� ������ �� ���� ����� ����ϴ� ��� ����� vkCmd���ξ�� �˾� �� �� ����
	��� void�� ������ �츮�� ����� ��ĥ �� ���� ���� ó���� ���� ����
	��� ����� ù��° �Ű������� �׻� ����� ����ϴ� ��� ������
	�ι�° �Ű������� ��� ������ ������ �н��� ���� ������ ����
	������ �Ű������� ���� �н� ������ ����� ����� �����ϴ� ����� ����
	VK_SUBPASS_CONTENTS_INLINE - ������ �н� ����� �⺻ ��� ���� ��ü�� ���ԵǸ� ���� ��� ���۴� ������� ����
	VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  - ���� �н� ����� ���� ��� ���ۿ��� ����
	*/
	//basic drawing commands
	vkCmdBindPipeline(commandBuffers[imageIndex_], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	//mesh update & draw
	rect_mesh_->update();

	updateUniformBuffer();

	rect_mesh_->draw();

	/*
	VkCommandBuffer                             commandBuffer,
	uint32_t                                    indexCount,
	uint32_t                                    instanceCount,
	uint32_t                                    firstIndex,
	int32_t                                     vertexOffset,
	uint32_t                                    firstInstance
	*/
	vkCmdEndRenderPass(commandBuffers[imageIndex_]);

	if (vkEndCommandBuffer(commandBuffers[imageIndex_]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

}

void Renderer::destroy()
{
	vkDestroySemaphore(DEVICE_MANAGER->getDevice(), renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(DEVICE_MANAGER->getDevice(), imageAvailableSemaphore, nullptr);

	rect_mesh_->destroy();
	vkDestroyBuffer(DEVICE_MANAGER->getDevice(), uniformBuffer, nullptr);
	vkFreeMemory(DEVICE_MANAGER->getDevice(), uniformBufferMemory, nullptr);

	cleanupSwapChain();

	vkDestroyDescriptorPool(DEVICE_MANAGER->getDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(DEVICE_MANAGER->getDevice(), descriptorSetLayout, nullptr);
	vkDestroyCommandPool(DEVICE_MANAGER->getDevice(), commandPool, nullptr);
}


void Renderer::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;

	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;//Optional

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(DEVICE_MANAGER->getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	
}

void Renderer::createUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	Buffer::createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
}

void Renderer::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;
	
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;

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

	//������ ��Ʈ�� ������ Ǯ�� �Ҹ�� �� �ڵ����� �����ǹǷ� ���� ������ �ʿ䰡 ������
	if (vkAllocateDescriptorSets(DEVICE_MANAGER->getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(DEVICE_MANAGER->getDevice(), 1, &descriptorWrite, 0, nullptr);
	
}

void Renderer::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.world = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	float aspect = (float)swapChainExtent.width / (float)swapChainExtent.height;
	ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

	ubo.proj[1][1] *= -1;//glm �� ���� opengl������ ����Ǿ����Ƿ� Ŭ�� ��ǥ�� y��ǥ�� ������ �̷��� ���ϸ� �̹����� ���Ʒ� ������

	void* data;
	vkMapMemory(DEVICE_MANAGER->getDevice(), uniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(DEVICE_MANAGER->getDevice(), uniformBufferMemory);
	//uniform_buffer_->prepareBuffer();

	vkCmdBindDescriptorSets(commandBuffers[imageIndex_], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}

void Renderer::drawFrame()
{
	//if (chooseSwapExtent().height == 0 || chooseSwapExtent().width) return;

	VkResult result = vkAcquireNextImageKHR(DEVICE_MANAGER->getDevice(), swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex_);

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

	//command buffer update 
	update();


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
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex_];
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

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex_;
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

void Renderer::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = DEVICE_MANAGER->querySwapChainSupport( DEVICE_MANAGER->getPhysicalDevice());

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = DEVICE_MANAGER->getSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = DEVICE_MANAGER->findQueueFamilies(DEVICE_MANAGER->getPhysicalDevice());
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {//present�� graphics�� queue�� �ٸ��� ó���� �ٸ�
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(DEVICE_MANAGER->getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(DEVICE_MANAGER->getDevice(), swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(DEVICE_MANAGER->getDevice(), swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Renderer::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(DEVICE_MANAGER->getDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

VkSurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Renderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D Renderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { DEVICE_MANAGER->getWIDTH(), DEVICE_MANAGER->getHEIGHT() };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void Renderer::createRenderPass()
{
	//depth, stencil, render target �ʱ�ȭ 
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = getSwapChainImageFormat();
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

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

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

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
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
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = Vertex::getBindingDesctiption();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

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

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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
	pipelineLayoutInfo.setLayoutCount = 1;
	//pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(DEVICE_MANAGER->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	/* pipeline info */
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;//optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;//optional

										 //���� ���� ��� �ܰ踦 �����ϴ� ��� ������ ����
	pipelineInfo.layout = pipelineLayout;

	//�״��� ����ü �����Ͱ� �ƴ� vulkan �ڵ��� ���������� ���̾ƿ��� ����
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	//���������� ������ �н��� �� �׷��� ������������ ���� �����н��� �ε����� ���� ������ ����
	/* �� Ư�� �ν��Ͻ� ��� �� ���������ο��� �ٸ� ������ �н��� ����� ���� ������ renderPass�� ȣȯ �����ؾ� �Ѵ�.
	*/
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	/* ������ �ΰ��� �Ű������� ������ basePipelineHandel, basePipelineIndex�� Vulkan�� ����ϸ� ���� ���������ο��� �Ļ��ۿ� ���ο� �׷��� ������������ ���� �� ����
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

	vkDestroyShaderModule(DEVICE_MANAGER->getDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(DEVICE_MANAGER->getDevice(), vertShaderModule, nullptr);

}
VkShaderModule Renderer::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(DEVICE_MANAGER->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void Renderer::createFramebuffers()
{
	//��� ������ ���۸� ������ �� ���ֵ��� �����̳��� ũ�⸦ �����Ͽ� ����!
	swapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
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
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
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

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(DEVICE_MANAGER->getDevice(), swapChainFramebuffers[i], nullptr);
	}
	vkFreeCommandBuffers(DEVICE_MANAGER->getDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	vkDestroyPipeline(DEVICE_MANAGER->getDevice(), graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(DEVICE_MANAGER->getDevice(), pipelineLayout, nullptr);
	vkDestroyRenderPass(DEVICE_MANAGER->getDevice(), renderPass, nullptr);
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(DEVICE_MANAGER->getDevice(), swapChainImageViews[i], nullptr);
	}
	vkDestroySwapchainKHR(DEVICE_MANAGER->getDevice(), swapChain, nullptr);
}

void Renderer::recreateSwapChain()
{
	vkDeviceWaitIdle(DEVICE_MANAGER->getDevice());

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
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

Renderer::Renderer() : CSingleTonBase<Renderer>("Renderer")
{

}


Renderer::~Renderer()
{

}
