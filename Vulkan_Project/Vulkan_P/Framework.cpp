#include "Framework.h"


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}


void Framework::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, Framework::onWindowResized);
}

VkExtent2D Framework::chooseSwapExtent()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	VkExtent2D actualExtent = { width, height };

	return actualExtent;
}

void Framework::initVulkan()
{
	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
	createSemaphores();
}

void Framework::cleanupSwapChain()
{
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void Framework::recreateSwapChain()
{
	vkDeviceWaitIdle(device);

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

void Framework::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}
	/*�޸� ����
	��ȿ�� �˻� ���̾ Ȱ��ȭ �� ���¿��� ���� ���α׷��� �����ϰ� �������α׷��� �����ϰ� �������α׷��� �޸� ��뷮�� ����͸��ϸ�
	�������α׷��� ������ �����ϴ°��� �� �� ���� �� ������ ��ȿ�� �˻� ���̾� ������ �������α׷��� gpu�� ��������� ����ȭ�Ǳ⸦ ����ϱ� ������
	��������� �ʼ��� �ƴ����� �� �� �������� �����ϴ� ���� ���ɿ� ū ������ ��ġ���� ����
	���� ������ �׸��⸦ �����ϱ� ���� ������ ���̼��� �����⸦ ��������� ��ٸ����ν� �̸� �ذ� �� ������ */

	vkDeviceWaitIdle(device);
	/* vkQueueWaitIdle�� ����Ͽ� Ư�� ��� ��⿭�� �۾��� �Ϸ�� ������ ��ٸ� ���� �Ӥ���
	�� ��ɵ��� ����ȭ�� �����ϴ� �ſ� �������� ������� ���� �� ���� */
}

void Framework::drawFrame()
{
	//if (chooseSwapExtent().height == 0 || chooseSwapExtent().width) return;

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
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
	presentInfo.pImageIndices = &imageIndex;
	//�̹����� ǥ���� ���� ü�ΰ� �� ���� ü���� �̹��� �ε����� ���� �̰� ���� �ϳ���

	presentInfo.pResults = nullptr;
	/* pResults ��� ������ ������ �Ű������� �ϳ����� �̰� ���������̼��� ������ ��� VkResult���� �迭��
	�����Ͽ� ��� ���� ����ü���� �˻��� �� ���� ���� �Լ��� ��ȯ ���� ����� �� �ֱ� ������ ���� ����ü�� �� ����ϴ°�� �ʿ����� ����
	*/

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
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
	vkQueueWaitIdle(presentQueue);//�̰ŵ� ���� ������ �Ȱ��� ��ȯ�� 

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

void Framework::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Framework::cleanup()
{
	cleanupSwapChain();

	/*
	���۴� ���α׷� ������� ��� �������� ����� �� �־�� �ϸ� ����ü�ο� �������� �����Ƿ�
	cleanup�Լ����� ȣ���Ͽ� ����
	*/
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, callback, nullptr);
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}

void Framework::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

void Framework::setupDebugCallback()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
}

void Framework::createSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void Framework::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}
void Framework::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void Framework::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
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

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Framework::createImageViews()
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

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void Framework::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
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

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void Framework::createGraphicsPipeline()
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
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
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
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
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
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

}

void Framework::createFramebuffers()
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

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
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

void Framework::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0;
	/* ��� ���۴� �츮�� �˻��� �׷��� �� ���������̼� ��⿭�� ���� ��ġ ��⿭ �� �ϳ��� �̸� �����Ͽ� �����
	�� ��� Ǯ�� ���� ������ ť�� ����� ��� ���۸� �Ҵ� �� �� ����
	�츮�� �׸� �׸��� ����� ����� ���̱� ������ �׷��ȴ�⿭ �йи��� �����ؾ���

	��� Ǯ���� �ΰ��� �÷��װ� ����
	VK_COMMAND_POOL_CREATE_TRANSIENT_BIT ���ο� Ŀ�ǵ�� Ŀ�ǵ� ���۰� ���� �ٽ� ��ϵɰ��� �Ͻ� (�޸� �Ҵ� ������ �����ų �� ����
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ��� ���۸� ������������ ���� �� �� ���� �� �÷��׸� ��������� ������
	��� �ٽ� �����ؾ� ��*/

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void Framework::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	/*
	ó�� �� �Ű� ������ �������� �ٷ� �� �� ���� �� ��° �Ű������� �޸� ���� ���� ��������
	�� �޸𸮴� ���� ���ۿ� ���� Ư���� �Ҵ�Ǳ� ������ �������� �ܼ��� 0��
	�������� 0�� �ƴѰ�� memRequirements.alignment�� ���� �� �־����
	���� c++�� ���� �޸� �Ҵ�� �޸𸮴� ��� �������� �����Ǿ�� ��, ���� ��ü�� ���ε� �� �޸𸮴� ���۰� �� �̻� ������ ���� ��� �����ؾ� �ϹǷ� ���۰� ������ �� �����ؾ���
	*/

	/*
	filling the vertex buffer
	���� ���ؽ� �����͸� ���ۿ� ������ ����
	�̴� vkMapMemory�� ����� ���� �޸𸮸� CPU�� �׼��� ���� �ϵ��� �޸𸮿� �����Ͽ� ����
	*/
	void* data;
	/*
	�� �Լ��� �����°� ũ��� ���ǵ� ������ �޸� ���ҽ��� ������ �׼��� �� �� �ְ���
	���⼭ �����°� ũ��� ���� 0�� bufferInfo.size�� Ư���� VK_WHOLE_SIZE�� �����Ͽ� ��� �޸𸮸� ���� �� ���� ����
	�� ���� �Ű������� �÷��׸� �����ϴµ� ����� �������� ���� API���� ���� ����� �� �ִ� �Ű������� ����
	������ ���� ���ε� �޸𸮿� ���� �������� ����� ����
	*/
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);

	/*
	���� vkUnmapMemory �� ����Ͽ� ���ؽ� �����͸� ���� �� �޸𸮿� memcpy�ϰ� �ٽ� ���� ������ �� ����
	�������� ����̹��� ���� ��� ĳ�� ������ ���� �޸𸮿� �����͸� ��� �������� ���� �� ����
	���ۿ� ���Ⱑ ���ε� �޸𸮿� ���� ǥ�õ��� ���� ���� ���� �� ������ �ذ��� �� �ִ� �ΰ��� ���
	- VK_MEMORY_PROPERTY_HOST_COHERENT_BIT�� �Բ� ǥ�õǴ� ȣ��Ʈ �ϰ��� �޸� ���� ����ض�
	- ���ε� �޸𸮿� ���� �Ŀ� vkFlushMappedMemoryRanges�� ȣ���ϰ� ���� �� �޸𸮿��� �б��� vkInvalidateMappedMemoryRanges�� ȣ���ض�

	�츮�� ���ε� �޸𸮰� �Ҵ� �� �޸��� ����� �һ� ��ġ�ϴ��� Ȯ���ϴ� ù ��° ����� ã�ư���
	����� �÷��� ���� ������ �ణ ������ �� ����
	*/
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	/*
	���� �����͸� �����ϰ� �����ϱ� ���� stagingBufferMemory�� �Բ� ���ο� stagingBuffer�� ����ϰ� ����
	�� �忡���� �� ���� ���ο� ���� ��� �÷��׸� ����� ��
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT  �޸� ���� ���ۿ��� ���۸� �ҽ��� ����� �� ����
	VK_BUFFER_USAGE_TRANSFER_DST_BIT  ���۴� �޸� ���� �۾����� ������� ����� �� ����

	���� vertexBuffer�� ��ġ�� ���� �޸� �������� �Ҵ��
	�̴� �Ϲ������� vkMapMemory�� ����� �� ������ �ǹ���
	�׷��� stagingBuffer���� vertexBuffer�� �����͸� ������ �� ����
	stagingBuffer�� ���� ���� �ҽ� �÷��׿� vertexBUffer�� ���� ���� ��� �÷��׸� ���� ���� ��� �÷��׿� �Բ� �����ۿ� �̸� ������ �������� ǥ���ؾ���

	*/
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Framework::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
	/*
	���̴� �ΰ��� ����
	bufferSize �� �ε��� ������ ũ���� uint16_t �Ǵ� uint32_t�� ũ�⿡ �ش��ϴ� �ε��� ���� ����
	indexBuffer������ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT��� USAGE_INDEX_BUFFER_BIT�� �����
	*/
}

void Framework::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//�� ���� ����ϰ� ���� �۾��� ������ ��ĥ �� ���� �Լ����� ���ƿ��⸦ ��ٸ� �� 
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;//Optional
	copyRegion.dstOffset = 0;//Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	/*
	draw�� �޸� ��ٸ��� �ʾƵ� �� ��� ���� ���؝۷�����
	�� ������ �Ϸ� �� ������ ����� �� �ִ� �ΰ��� ����� ����
	��Ÿ���� ����Ͽ� vkWaitForFences�� ����ϰų� vkQueueWaitIdle�� ����Ͽ� ���� ��⿭�� ���� ���°� �� ������ ��ٸ� �� ����
	���� ��ġ�� ����ϸ� ���ÿ� ���� �� ������ �����ϰ� �� ���� �ϳ� �� �����ϴ� ��� �Ϸ�� ��� ������ ��ٸ� �� ����
	�װ��� �����ڿ��� �� ���� ��ȸ�� ����ȭ ��ų �� ����
	*/
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);//���� �Ϸ� �� ����
}

uint32_t Framework::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	/*
	VkPhysicalDeviceMemoryProperties ����ü���� �� ���� �迭 memoryTypes�� memoryHeaps�� ����
	�޸� ���� VRAM�� ������ ��� RAM�� ���� VRAM �� ���� ������ ���� ������ �޸� ���ҽ��Դϴ�. �� ������ ���� ������ �ޤǹа� �ֽ�
	���� �츮�� �޸� ������ ���ؼ��� ������ �������� �װ��� ��ġ�� ���� �ƴ϶� ���ɿ� ������ ��ĥ �� �ִٰ� ����� �� ����
	*/
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	/*
	typeFilter �Ű� ������ ������ �޸� ������ ��Ʈ �ʵ带 �����ϴ� �� ���˴ϴ�. �� �ܼ��� �ݺ��Ͽ� �ش� ��Ʈ�� 1�� �����Ǿ� �ִ��� Ȯ���Ͽ� ������ �޸� ������ �ε����� ã�� �� ����
	���� �����͸� �ش� �޸𸮿� �� �� �־����
	memoryTypes �迭�� �� �޸� ������ �� �� �Ӽ��� �����ϴ� VkMemoryType ����ü�� ������
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT�� ǥ�õ�����
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT �Ӽ��� ����ؾ� ��
	���� �� �Ӽ��� ������ Ȯ���ϱ� ���� ������ ������ �� ����
	*/
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	/*
	�� �̻��� �Ӽ��� ���� �� �����Ƿ� ��Ʈ AND�� ����� ���� 0�� �ƴ� ���ϴ� �Ӽ� ��Ʈ �ʵ�� ������ Ȯ���ؾ� ��
	�ʿ�� �ϴ� ��� ������Ƽ�� ������ ���ۿ� ������ �޸� ���� �����ϸ�, �� �ε����� �����־� �׷��� ���� ��� ����
	*/
	throw std::runtime_error("failed to find suitable memory type!");

}

void Framework::createCommandBuffers()
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

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (auto i = 0; i < commandBuffers.size(); ++i) {
		//��� ���� ��� ����
		/* ������� vkCmdBeginRenderPass�� ���� �н��� �����Ͽ� ���� �����н��� VKRenderPassBeginInfo����ü�� �Ϻ� �Ű������� ����Ͽ� ����
		*/
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
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

		/*vkCmdBindVertexBuffers
		vkCmdBindVertexBuffers �Լ��� ���� �忡�� ���� �� �Ͱ� ���� ���ε��� ���� ���۸� ���ε��ϴ� �� ���
		Ŀ��� ���� ���� ������ 2���� �Ķ���ʹ�, ���� ���۸� �����ϴ� ������ �� ���̳ʸ��� ���� ������
		������ �� �Ű� ������ ���ε� �� ���� ������ �迭�� �����ϰ� ���� �����͸� �д� ����Ʈ �������� ������
		���� vkCmdDraw�� ���� ȣ���� �����Ͽ� �ϵ� �ڵ� �� ���� 3�� �ݴ�� ������ ���� ���� �����ؾ���
		*/
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0/*offset ?? */, 1/*������ ��*/, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1/*�ν��Ͻ� ��*/, 0, 0, 0);
		/*
		VkCommandBuffer                             commandBuffer,
		uint32_t                                    indexCount,
		uint32_t                                    instanceCount,
		uint32_t                                    firstIndex,
		int32_t                                     vertexOffset,
		uint32_t                                    firstInstance
		*/
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void Framework::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphores!");
	}
}

VkShaderModule Framework::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

VkSurfaceFormatKHR Framework::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

VkPresentModeKHR Framework::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
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

VkExtent2D Framework::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { WIDTH, HEIGHT };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

SwapChainSupportDetails Framework::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool Framework::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Framework::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices Framework::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

std::vector<const char*> Framework::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool Framework::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

