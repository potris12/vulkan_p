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
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	const VkDevice& getDevice() { return device; }
	const VkQueue& getGraphicsQueue() { return graphicsQueue; }
	const VkQueue& getPresentQueue() { return presentQueue; }

	//swap chain
	VkSwapchainKHR& getSwapChain() { return swapChain; }
	VkFormat& getSwapChainImageFormat() { return swapChainImageFormat; }
	VkExtent2D& getSwapChainExtent() { return swapChainExtent; }
	std::vector<VkImageView>& getSwapChainImageViews() { return swapChainImageViews; }
	//tmp public create funcion 
	void createSwapChain();
	void createImageViews();
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
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	//logical device
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	void createLogicalDevice();
	
	//swap chain
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);



public:
	DeviceManager();
	virtual ~DeviceManager();
};

