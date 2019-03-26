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

	//명령 버퍼 기록 시작
	/* 드로잉은 vkCmdBeginRenderPass로 렌더 패스를 시작하여 시작 랜더패스는 VKRenderPassBeginInfo구조체의 일부 매개변수를 사용하여 구성
	*/
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex_];
	//첫번쨰 매개변수는 랜더 패스 자체와 바인딩 할 어테치먼트, 각 스왑체인 이미지에 대해 color attachment로 지정하는 프레임 버퍼를 만들었
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = swapChainExtent;
	//다음 두 매개변수는 랜더링 여역의 크기를 정의 렌더링 영역은 셰이더 로드 및 저장이 수행되는 위치를 정의
	//이 영역 밖의 픽셀에는 정의되지 않은 값이 있음 최상의 성능을 위해 어테치먼트의 크기와 일치해야함
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
	/* 이제 렌더링 패스를 시작할 수 있음 명령을 기록하는 모든 기능은 vkCmd접두어로 알아 볼 수 있음
	모둔 void를 리턴함 우리가 기록을 마칠 떄 까지 오류 처리가 없을 것임
	모든 명령의 첫번째 매개변수는 항상 명령을 기록하는 명령 버퍼임
	두번째 매개변수는 방금 제공한 렌더링 패스의 세부 정보를 지정
	마지막 매개변수는 랜더 패스 내에서 드로잉 명령을 제공하는 방법을 제어
	VK_SUBPASS_CONTENTS_INLINE - 렌더링 패스 명령은 기본 명령 버퍼 자체에 포함되며 보조 명령 버퍼는 실행되지 않음
	VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS  - 렌더 패스 명령은 보조 명령 버퍼에서 실행
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

	//설명자 세트는 설명자 풀이 소멸될 떄 자동으로 해제되므로 따로 정리할 필요가 없음ㄴ
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

	ubo.proj[1][1] *= -1;//glm 은 원래 opengl용으로 설계되었으므로 클립 좌표의 y좌표가 반전됩 이렇게 안하면 이미지가 위아래 만전됨

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

	/* 스왑체인이 이미지를 획득하려고 시도할 때 오래된 것으로 판명되면 더 이상 프레젠테이션 할 수 없음 따라서 우리는 즉시 스왑체인을 다시 만들고
	다음 dawFrame호출을 다시 시도해야 함
	스왑 체인이 부차적인 경우 이를 수행하기로 결정할 수도 있지만 이미 이미지를 획득했기 때문에 계속 진행하도록 선택했슴 */
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
	vkAcquireNextImageKHR 의 처음 두 매개변수는 이미지를 가져오려는 논리 장치와 스왑체인임 세번째 매개변수는 이미지가 사용가능하게 되는 시간을 나노초로 지정
	다음 두 매개 변수는 프레젠테이션 엔진이 이미지 사용을 마치면 신호를 보낼 동기화 개체를 지정
	그것인 우리가 그리기 시작할 수 있는 시점임 세마포어, 펜스 둘 다 지정할 수 있음 우리는 여기서 아까만든거 사용함
	마지막 매개변수는 사용 가능하게 된 스왑체인 이미지의 인덱스를 출력하는 변수를 지정
	인덱스는 swapChainImages배열의 vKiMAGE를 참조 이 인덱스를 사용하여 올바른 명령 버퍼를 선택
	*/
	//명령 버퍼 제출
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	/*
	처음 세 매개변수는 실행이 시작되기 전에 대기할 파이프라인 단계에서 기다릴 세마포어를 지정
	색상을 사용할 떄까지 이미지에 색을 쓰고 싶어하므로 color attchment에 쓰는 그래픽 파이프라인의 단계를 지정하고 있음
	이는 이론적으로 이ㅣㅁ지 구현이 아직 완료되지 않은 상태엣 ㅓ구현이 이미 버텍스 쉐이더 등을 실행할 수 있음을 의미
	wiatStages배열의 각 항목은 pWaitSemaphores에서 동일한 색인을 가진 세마포어에 해당함

	다음 두
	*/
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex_];
	/*
	다음 두 매개변수는 실행을 위해 실제로 제출할 명령 버퍼를 지정
	앞서 언급했듯이 방금 가져온 스왑체인이미지를 색상 첨부로 바인딩하는 명령버퍼를 제출
	*/
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	/*
	signalSemaphore는 명령 버퍼가 실행을 완료하면 신호를 보낼 세마포어를 지정
	*/
	if (vkQueueSubmit(DEVICE_MANAGER->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	/*
	d이제 vkQueue Submit 을 사용하여 명령 버퍼를 그래픽 대기열에 제출할 수 있음 이 함수는 작업 부하가 훨씬 클 떄 vkSubmitInfo구조체 배열을 효율성을 위한 인수로 사용함
	마지막 매개변수는 명령 버퍼가 실행ㅇ르 완료할 떄 신호를 보낼 선택적 펜스를 참조함 동기화를 위해 세마포어를 사용하기 때문에 VK_NULL_HANDLE을 전달함

	*/

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex_;
	//이미지를 표시할 스왑 체인과 각 스왑 체인의 이미지 인덱스를 지정 이건 거의 하나임

	presentInfo.pResults = nullptr;
	/* pResults 라는 마지막 선택적 매개변수가 하나있음 이건 프리젠테이션이 성공한 경우 VkResult값의 배열을
	지정하여 모든 개별 스왑체인을 검사할 수 있음 현재 함수의 반환 값을 사용할 수 있기 때문에 단일 스왑체인 만 사용하는경우 필요하지 않음
	*/

	result = vkQueuePresentKHR(DEVICE_MANAGER->getPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	/* vkQueuePresentKHR 함수는 스왑체인에 이미지를 표시하라는 요청을 제출
	다음 장에서 vkAcquireNextImageKHR및 vkQueuePresentKHR모두에 대한 오류 처리를 추가할 것
	왜냐면 실패는 프로그램의 종료로 이어져야 하기때문임 */

	/* 1203 이제 삼각형이 그려짐 drawFrame의 모든 작업은 비동기임 즉 mainLoop에서 루프를 종료해도 그리기 및 프레젠테이션 작업이 계속 진행될 수 있음
	그런일이 일어나는 동안 자원을 정리하는건 문제가 있음

	이 문제를 해결하기 위해 mainLoop를 종료하고 창을 삭제하기 전에 논리 장치가 작업을 마칠 떄 까지 기다려야함
	*/

	//동기화 그림 그리고 wait
	vkQueueWaitIdle(DEVICE_MANAGER->getPresentQueue());//이거도 위의 에러를 똑같이 반환함 

													   /*
													   이렇게 그리는게 효율적이다 이전 프레임이 렌더링되는 동안 게임에서 AI루틴을 실행하는 등 응용프로그램의 상태를 업데이트 할 수있음
													   이렇게 하면 항상 gpu cpu busy를 유지할 수있음
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

	if (indices.graphicsFamily != indices.presentFamily) {//present랑 graphics랑 queue가 다르면 처리가 다름
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
	//depth, stencil, render target 초기화 
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
	랜더 패스의 하위 패스는 이미지 레이아웃 전환을 자동으로 처리함
	이러한 전환은 서브패스 간의 메모리및 실행 종속성을 지정하는 서브 패스 종속성에 의해 제어됨
	지금은 하나의 서브패스 만 있지만이 서브패스 전후의 작업은 암시적 서브패스로 계산됨
	랜더링 패스의 시작과 렌더링 패스의 끝에서 전환을 처리하는 두 가지 기본 종속성이 있지만, 적시에 발생하지 않음
	그것은 전환이 파이프라인의 시작에서 발생한다고 가정하지만 그 시점에서 아직 이미지를 얻지 못했어서 그럼
	이 문제를 처리하는데 두가지 방법이 있음
	이미지를 사용할 수 있을 떄까지 렌더링 패스가 시작되지 않도록 하거나, 특정 단계가지 랜더 패스를 대기시킬 수 있음

	*/

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	/*
	처음 두 필드는 종속성과 종속 서브패스의 색인을 지정 특수값 VK_SUBPASS_EXTERNAL은 srcSubpass또는 dstSubpass에 지정되었는지 여부에 따라 렌더 패스 전후의 암시적 서브 패스를 ㅏㅍㅁ조함
	인덱스 0 은 처음이자 유일한 하나의 인 우리의 서브패스를 참조함
	dstSubpass는 종속성 그래스에서 순환을 방지하기 위해 항상 srcSubpass보다 커야함
	*/
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	/*
	다음 두 필드는 대기할 작업과 이러한 작업이 수행되는 단계를 지정
	스왑체인이 이미지에 접근하기 전에 스왑체인이 이미지 읽기를 끝내기를 기다려야 함
	잊 작업은 색상 첨부 출력 단계자체를 기다ㅕㄹ 수행할 수 있음
	*/
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	/*
	이 작업을 기다려야 하는 작업은 컬러 어테치먼트 단계에 있으며 컬러 어테치먼트를 읽고 쓰는 작업이 필요함
	이러한 설정은 실제로 필요하고 일어나느 변화를 막을것
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

										 //다음 공정 기능 단계를 설명하는 모든 구조를 참조
	pipelineInfo.layout = pipelineLayout;

	//그다은 구조체 포인터가 아닌 vulkan 핸들인 파이프라인 레이아웃이 나옴
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	//마지막으로 랜더링 패스와 이 그래픽 파이프라인이 사용될 하위패스의 인덱스에 대한 참조가 있음
	/* 이 특정 인스턴스 대신 이 파이프라인에서 다른 랜더링 패스를 사용할 수도 있지만 renderPass와 호환 가능해야 한다.
	*/
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	/* 실제로 두개의 매개변수가 더있음 basePipelineHandel, basePipelineIndex임 Vulkan을 사용하면 기존 파이프라인에서 파새앟여 새로운 그래픽 파이프라인을 만들 수 있음
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
	//모든 프레임 버퍼를 포함할 ㅅ ㅜ있도록 컨테이너의 크기를 설정하여 시작!
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
	/* 간단 간단 프레임 버퍼 생성
	render Pass와 프레임 버퍼가 호환되어야 하는지를 지정해야함
	프레임 버퍼는 호환 가능한 렌더링 패스와 함꼐만 사용할 수 있음
	이는 대략 동일한 갯수와 타입의 어테치먼트를 사용해야 한다는 것을 의미
	attachmentCount 및 pAttachments매개 변수는 렌더링 패스 pAttachment 배열의 각 첨부 설명에 바인딩 되어야 하는 VkImageView객체를 지정
	width 및 height 매개 변수는 자체 참조가 가능하며 레이어는 이미지 배열의 레이어 수를 나타냄
	스왑체인 이미지는 단일 이미지 이므로 레이어수는 1

	렌더링을 완*/
}

void Renderer::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = DEVICE_MANAGER->findQueueFamilies(DEVICE_MANAGER->getPhysicalDevice());

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	/* 명령 버퍼는 우리가 검색한 그래픽 및 프리젠테이션 대기열과 같은 장치 대기열 중 하나에 이를 제출하여 실행됨
	각 명령 풀은 단일 유형의 큐에 제출된 명령 버퍼만 할당 할 수 있음
	우리는 그림 그리기 명령을 기록할 것이기 때문에 그래픽대기열 패밀리를 선택해야함

	명령 풀에는 두가지 플레그가 있음
	VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 새로운 커맨드로 커맨드 버퍼가 자주 다시 기록될것을 암시 (메모리 할당 동작을 변경시킬 수 있음
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT 명령 버퍼를 개벼렂ㄱ으로 재기록 할 수 있음 이 플래그를 ㅎ사용하지 않으면
	모두 다시 설정해야 함*/

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
	/* vkDeviceWaitIdle을 호출하기 떄문에 사용중인 리소스에 접근하지 않음
	목표 스왑체인 자체를 다시 만드는것
	이미지 뷰는 스왑체인 이미지를 직접 기반으로 하므로 다시 만들어야함
	렌더 패스는 스왑체인 이미지 형식에 따라 다르기 때문에 다시 작성해야함
	스왑체인 이미지 형식은 창 크기 조정과 같은 작업 중에 변경되는 경우는 거의 없지만 여전히 처리해야함
	뷰포트 및 시저 크기는 그래픽 파이프라인 생성 중 지정되므로 파이프라인도 다시 작성
	뷰포트와 시저 사각형에 동적 상태를 사용하여 이를 피할 수 있음
	마지막으로 프레임 버퍼와 명령 버퍼는 스왑체인 이미지에 직접 의존합

	이러한 객체들을 다시 만들기 전에 기존의 리소스를 정리해야 하는데 이를 하려며 cleanup코드중 일부를 recreateS
	*/
}

Renderer::Renderer() : CSingleTonBase<Renderer>("Renderer")
{

}


Renderer::~Renderer()
{

}
