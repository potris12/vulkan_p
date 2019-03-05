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

void Framework::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}
	/*메모리 누수
	유효성 검사 레이어가 활성화 된 상태에서 응용 프로그램을 실행하고 응용프로그램을 실행하고 응용프로그램의 메모리 사용량을 모니터링하면
	응용프로그램이 느리게 증가하는것을 할 수 있음 그 이유는 유효성 검사 레이어 구현이 응용프로그램이 gpu와 명시적으로 동기화되기를 기대하기 때문임
	기술적으로 필수는 아니지만 한 번 프레임을 수행하는 것이 성능에 큰 영향을 미치지는 않음
	다음 프레임 그리기를 시작하기 전에 프레젠 테이션이 끝나기를 명시적으로 기다림으로써 이를 해결 할 수있음 */

	vkDeviceWaitIdle(device);
	/* vkQueueWaitIdle을 사용하여 특정 명령 대기열의 작업이 완료될 때까지 기다릴 수동 ㅣㅆ음
	이 기능들을 동기화를 수행하는 매우 기초적인 방법으로 사용될 수 있음 */
}

void Framework::drawFrame()
{
	//if (chooseSwapExtent().height == 0 || chooseSwapExtent().width) return;

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
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
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
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
	presentInfo.pImageIndices = &imageIndex;
	//이미지를 표시할 스왑 체인과 각 스왑 체인의 이미지 인덱스를 지정 이건 거의 하나임

	presentInfo.pResults = nullptr;
	/* pResults 라는 마지막 선택적 매개변수가 하나있음 이건 프리젠테이션이 성공한 경우 VkResult값의 배열을
	지정하여 모든 개별 스왑체인을 검사할 수 있음 현재 함수의 반환 값을 사용할 수 있기 때문에 단일 스왑체인 만 사용하는경우 필요하지 않음
	*/

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
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
	vkQueueWaitIdle(presentQueue);//이거도 위의 에러를 똑같이 반환함 

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
	버퍼는 프로그램 종료까지 명령 렌더링에 사용할 수 있어야 하며 스왑체인에 의존하지 않으므로
	cleanup함수에서 호출하여 제거
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
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

}

void Framework::createFramebuffers()
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

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
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

void Framework::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0;
	/* 명령 버퍼는 우리가 검색한 그래픽 및 프리젠테이션 대기열과 같은 장치 대기열 중 하나에 이를 제출하여 실행됨
	각 명령 풀은 단일 유형의 큐에 제출된 명령 버퍼만 할당 할 수 있음
	우리는 그림 그리기 명령을 기록할 것이기 때문에 그래픽대기열 패밀리를 선택해야함

	명령 풀에는 두가지 플레그가 있음
	VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 새로운 커맨드로 커맨드 버퍼가 자주 다시 기록될것을 암시 (메모리 할당 동작을 변경시킬 수 있음
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT 명령 버퍼를 개벼렂ㄱ으로 재기록 할 수 있음 이 플래그를 ㅎ사용하지 않으면
	모두 다시 설정해야 함*/

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
	처음 세 매개 변수는 무엇인지 바로 알 수 있음 네 번째 매개변수는 메모리 영역 내의 오프셋임
	이 메모리는 정점 버퍼에 대해 특별히 할당되기 떄문에 오프셋은 단순히 0임
	오프셋이 0이 아닌경우 memRequirements.alignment로 나눌 수 있어야해
	물론 c++의 동적 메모리 할당과 메모리는 어느 시점에서 해제되어야 함, 버퍼 객체에 바인드 된 메모리는 버퍼가 더 이상 사용되지 않을 경우 해제해야 하므로 버퍼가 삭제된 후 해제해야해
	*/

	/*
	filling the vertex buffer
	이제 버텍스 데이터를 버퍼에 복사할 차례
	이는 vkMapMemory를 사용해 버퍼 메모리를 CPU가 액세스 가능 하도록 메모리에 매핑하여 수행
	*/
	void* data;
	/*
	이 함수는 오프셋과 크기로 정의된 지정된 메모리 리소스의 영역에 액세스 할 수 있게함
	여기서 오프셋과 크기는 각각 0과 bufferInfo.size임 특수값 VK_WHOLE_SIZE를 지정하여 모든 메모리를 매핑 할 수도 있음
	두 번쨰 매개변수는 플래그를 지정하는데 사용할 수있지만 지금 API에는 아직 사용할 수 있는 매개변수가 없음
	마지막 값은 매핑된 메모리에 대한 포인터의 출력을 지정
	*/
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);

	/*
	이제 vkUnmapMemory 를 사용하여 버텍스 데이터를 매핑 된 메모리에 memcpy하고 다시 매핑 해제할 수 있음
	불행히도 드라이버는 예를 들어 캐싱 때문에 버퍼 메모리에 데이터를 즉시 복사하지 않을 수 있음
	버퍼에 쓰기가 매핑된 메모리에 아직 표시되지 않을 수도 있음 이 문제를 해결할 수 있는 두가지 방법
	- VK_MEMORY_PROPERTY_HOST_COHERENT_BIT와 함께 표시되는 호스트 일관된 메모리 힙을 사용해라
	- 매핑된 메모리에 쓰기 후에 vkFlushMappedMemoryRanges를 호출하고 매핑 된 메모리에서 읽기전 vkInvalidateMappedMemoryRanges를 호출해라

	우리는 매핑된 메모리가 할당 된 메모리의 내용과 할상 일치하는지 확인하는 첫 번째 방법을 찾아갔음
	명시적 플러싱 보다 성능이 약간 떨어질 수 있음
	*/
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	/*
	정점 데이터를 매핑하고 복사하기 위해 stagingBufferMemory와 함께 새로운 stagingBuffer를 사용하고 있음
	이 장에서는 두 개의 새로운 버퍼 사용 플래그를 사용할 것
	VK_BUFFER_USAGE_TRANSFER_SRC_BIT  메모리 전송 동작에서 버퍼를 소스로 사용할 수 있음
	VK_BUFFER_USAGE_TRANSFER_DST_BIT  버퍼는 메모리 전송 작업에서 대상으로 사용할 수 있음

	이제 vertexBuffer는 장치의 로컬 메모리 유형으로 할당됨
	이는 일반적으로 vkMapMemory를 사용할 수 없음을 의미함
	그러나 stagingBuffer에서 vertexBuffer로 데이터를 복사할 수 있음
	stagingBuffer에 대한 전송 소스 플래그와 vertexBUffer에 대한 전송 대상 플래그를 정점 버퍼 사용 플래그와 함께 지저앟여 이를 수행할 예정임을 표시해야함

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
	차이는 두가지 뿐임
	bufferSize 는 인덱스 유형의 크기인 uint16_t 또는 uint32_t의 크기에 해당하는 인덱스 수와 같음
	indexBuffer사용법은 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT대신 USAGE_INDEX_BUFFER_BIT를 쓰면됨
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
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//한 번만 사용하고 복사 작업이 실행을 마칠 떄 까지 함수에서 돌아오기를 기다릴 것 
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
	draw와 달리 기다리지 않아도 됨 즉시 전송 실해앟려고함
	이 전송이 완료 될 때까지 대기할 수 있는 두가지 방법이 있음
	울타리를 사용하여 vkWaitForFences로 대기하거나 vkQueueWaitIdle을 사용하여 전송 대기열이 유휴 상태가 될 떄까지 기다릴 수 있음
	차단 장치를 사용하면 동시에 여러 번 전송을 예약하고 한 번에 하나 씩 실행하는 대신 완료된 모든 전송을 기다릴 수 있음
	그것은 운전자에게 더 많은 기회를 최적화 시킬 수 있음
	*/
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);//전송 완료 후 정리
}

uint32_t Framework::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	/*
	VkPhysicalDeviceMemoryProperties 구조체에는 두 개의 배열 memoryTypes및 memoryHeaps가 있음
	메모리 힙은 VRAM이 부족한 경우 RAM의 전용 VRAM 및 스왑 공간과 같은 별개의 메모리 리소스입니다. 이 힙에는 여러 유형의 메ㅗ밀가 있습
	지금 우리는 메모리 윻ㅇ에 대해서만 관심을 가질것임 그것이 위치한 힙이 아니라 성능에 영향을 미칠 수 있다고 상상할 수 있음
	*/
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	/*
	typeFilter 매개 변수는 적절한 메모리 유형의 비트 필드를 지정하는 데 사용됩니다. 즉 단순히 반복하여 해당 비트가 1로 설정되어 있는지 확인하여 적절한 메모리 유형의 인덱스를 찾을 수 있음
	정점 데이터를 해당 메모리에 쓸 수 있어야해
	memoryTypes 배열은 각 메모리 유형의 힙 및 속성을 지정하는 VkMemoryType 구조체로 구성됨
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT로 표시되지만
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 속성도 사용해야 함
	이제 이 속성의 지원을 확인하기 위해 루프를 수정할 수 있음
	*/
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	/*
	둘 이상의 속성을 가질 수 있으므로 비트 AND의 결과가 단지 0이 아닌 원하는 속성 비트 필드와 같은지 확인해야 함
	필요로 하는 모든 프로퍼티를 가지는 버퍼에 적절한 메모리 형이 존재하면, 그 인덱스를 돌려주어 그렇지 않은 경우 오류
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

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (auto i = 0; i < commandBuffers.size(); ++i) {
		//명령 버퍼 기록 시작
		/* 드로잉은 vkCmdBeginRenderPass로 렌더 패스를 시작하여 시작 랜더패스는 VKRenderPassBeginInfo구조체의 일부 매개변수를 사용하여 구성
		*/
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
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

		/*vkCmdBindVertexBuffers
		vkCmdBindVertexBuffers 함수는 이전 장에서 설정 한 것과 같이 바인딩에 정점 버퍼를 바인딩하는 데 사용
		커멘드 버퍼 외의 최초의 2개의 파라미터는, 정점 버퍼를 지정하는 오프셋 및 바이너리의 수를 지정함
		마지막 두 매개 변수는 바인딩 할 정점 버퍼의 배열을 지정하고 정점 데이터를 읽는 바이트 오프셋을 지정함
		또한 vkCmdDraw에 대한 호출을 변경하여 하드 코드 된 숫자 3과 반대로 버퍼의 정점 수를 전달해야함
		*/
		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0/*offset ?? */, 1/*정점의 수*/, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1/*인스턴스 수*/, 0, 0, 0);
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

