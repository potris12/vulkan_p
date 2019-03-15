#pragma once
#include "stdafx.h"
#include "Singleton.h"

class Renderer : public CSingleTonBase<Renderer>
{
public:
	void awake();
	void start() {};
	void update() {};
	void destroy();


	void createSwapChain();
	void createImageViews();


	//swap chain
	VkSwapchainKHR& getSwapChain() { return swapChain; }
	VkFormat& getSwapChainImageFormat() { return swapChainImageFormat; }
	VkExtent2D& getSwapChainExtent() { return swapChainExtent; }
	std::vector<VkImageView>& getSwapChainImageViews() { return swapChainImageViews; }

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);

private:
	//swap chain
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

public:
	Renderer();
	~Renderer();
};


#define RENDERER Renderer::GetInstance()