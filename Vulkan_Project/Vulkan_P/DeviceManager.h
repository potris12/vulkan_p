#pragma once
#include "stdafx.h"
#include "Singleton.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


class DeviceManager : public CSingleTonBase<DeviceManager>
{
public:
	void awake();
	void start() {};
	void update() {};
	void destroy();

	GLFWwindow* getWindow() { return window; }
	VkInstance& getVKInstance() { return instance; }
	VkSurfaceKHR& getSurface() { return surface; }

	VkPhysicalDevice& getPhysicalDevice() { return physicalDevice; }
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	const VkDevice& getDevice() { return device; }
	const VkQueue& getGraphicsQueue() { return graphicsQueue; }
	const VkQueue& getPresentQueue() { return presentQueue; }

	const int getWIDTH() { return WIDTH; }
	const int getHEIGHT() { return HEIGHT;}

	/*
	그래픽 카드는 할당 할 수 있는 여러 유형의 메모리를 제공할 ㅅ ㅜ있음
	각 유형의 메모리는 허용되는 작업 및 성능 특성에 따라 다름
	버퍼의 요구 사항과 자체 애플리케이션 요구사항을 결합하여 사용할 올바른 유형의 메모리를 찾아야함
	*/
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

private:

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


	//window
	const int WIDTH = 800;
	const int HEIGHT = 600;
	GLFWwindow* window;
	void initWindow();

	//instance
	VkInstance instance;
	void createInstance();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	//validation layout
	VkDebugUtilsMessengerEXT callback;
	void setupDebugCallback();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	//create surface
	VkSurfaceKHR surface;
	void createSurface();
	
	//physical device
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	//logical device
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	void createLogicalDevice();

	//메모리 요구사항?
	VkPhysicalDeviceMemoryProperties memProperties;


public:
	DeviceManager();
	virtual ~DeviceManager();
};

#define DEVICE_MANAGER DeviceManager::GetInstance()