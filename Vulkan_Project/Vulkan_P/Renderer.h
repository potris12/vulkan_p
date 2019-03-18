#pragma once
#include "stdafx.h"
#include "Singleton.h"

#include "Buffer.h"
#include "Vertex.h"

////�̰� ���� ������ ��ǲ���̾ƿ��� 
//const std::vector<Vertex> vertices = {
//	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
//	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
//	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
//};

/*
�ε��� ������
*/
const std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0,1,2,2,3,0
};

/*
���ý� �Է��� ó���ϴ� ����� �����ϴ� �� ��° ������ VkVertexInputAttributeDescription��
��
*/


class Renderer : public CSingleTonBase<Renderer>
{
public:
	void awake();
	void start() {};
	void update() {};
	void destroy();


	//swap chain
	VkSwapchainKHR& getSwapChain() { return swapChain; }
	VkFormat& getSwapChainImageFormat() { return swapChainImageFormat; }
	VkExtent2D& getSwapChainExtent() { return swapChainExtent; }
	std::vector<VkImageView>& getSwapChainImageViews() { return swapChainImageViews; }

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);

	//render pass
	VkRenderPass& getRenderPass() { return renderPass; }

	/* shader module */
	VkShaderModule createShaderModule(const std::vector<char>& code);
	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	//graphics pipeline
	VkPipeline getGraphicsPipeline() { return graphicsPipeline; }
	//frame buffer
	std::vector<VkFramebuffer>& getSwapChainFramebuffers() { return swapChainFramebuffers; }
	//command pool
	const VkCommandPool& getCommandPool() { return commandPool; }
	//command buffers
	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; };

	//resize swap chain
	void cleanupSwapChain();
	void recreateSwapChain();
	/* glfw�� �ݹ��Լ��� �����̵� �μ��θ� �Լ� �����͸� �޾Ƶ��̹Ƿ� ����Լ��� ���� ����� ���� ����
	0�� �ƴ� �� üũ - 0äũ ���ϸ� â�� �ּ�ȭ �Ǿ� ����ü�� �ۼ��� ������ �� ���� */
	static void onWindowResized(GLFWwindow* window, int width, int height) {
		if (width == 0 || height == 0) return;
		Renderer* app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));

		app->recreateSwapChain();
	}


	/*
	drawFrame�Լ���
	1. ����ü�ο��� �̹����� ������
	2. �ش� �̹����� �ִ� ��� ���۸� ������ ������ ����ġ��Ʈ�� ����
	3. ���������̼��� ���� �̹����� ���� ü������ ��ȯ
	�̷��� �� �̺�Ʈ�� ���� �Լ� ȣ���� ����Ͽ� �����ϵ��� ���������� �񵿱������� �����
	�Լ� ȣ���� �۾��� ������ �Ϸ�Ǳ� ���� ��ȯ�Ǹ� ���� ������ ���ǵ��� ����
	�װ��� ������ �۾��� ���� �۾��� �����ϱ� ������ ��¿ �� ���� ���̱⵵ ��
	����ü�� �̺�Ʈ�� ����ȭ �ϴ� ������� �潺�� ������� ����
	�̵��� �ϳ��� ���� ��ȣ�� ������ ������ �����ϸ� �潺�� ������� unsignaled���¿��� signaled���·� �� ������ ����ϴ� �� ����� �� �ִ� �� ��ü��
	�������� �潺�� ���´� vkWaitForFences�� ���� ȣ���� ����Ͽ� ���α׷����� �׼��� �� �� ������ ��������� ����� �� ���ٴ� ��
	���潺�� �ַ� ���� ���α׷� ��ü�� ������ �۾��� ����ȭ�ϴ� �ݸ� ��������� ��� ��⿭ ������ �Ǵ� �۾� ��⿭ ���� �۾��� ����ȭ�ϴ� �� ����
	�츮�� ������� ���� �����ϰ� ����� �׸��� ��� �� �����������̼��� ��⿭ �۾��� ����ȭ �Ұ�

	��������
	- �̹����� �����ͼ� ������ �� �غ� �Ǿ����� �˸��� ���� �ϳ��� ������� �ʿ��ϸ� �������� �Ϸ�ǰ� ���������̼��� �߻��� �� ������ �˸��� �Ǵٸ� ��ȣ�� �ʿ�
	�̷��� �������� ��ü�� ������ �ΰ��� Ŭ���� ����� ����

	*/
	//Draw
	void drawFrame();

private:
	//swap chain
	void createSwapChain();
	void createImageViews();
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	//render pass
	void createRenderPass();
	VkRenderPass renderPass;

	//pipeline
	void createGraphicsPipeline();
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	//frame buffers
	void createFramebuffers();
	std::vector<VkFramebuffer> swapChainFramebuffers;

	/* ��� ���۸� ����� ���� ��� ���� ������ ��
	��� Ǯ�� ���۸� �����ϴ� �� ���Ǵ� �޸𸮸� �����ϸ� �� ��� ���۴� �ڽ��� ���۸� �Ҵ� */
	VkCommandPool commandPool;
	void createCommandPool();


	std::shared_ptr<Buffer> vertex_buffer_ = nullptr;
	std::shared_ptr<Buffer> index_buffer_ = nullptr;
	void createVertexBuffer();
	void createIndexBuffer();

	/* ���� ��� ���۸� �Ҵ��ϰ� ��� ���۸� ����� �� ���� ����� ��� �� �ϳ��� �ùٸ� VkFramebuffer�� ���ε� �ϱ� ������ ������ ����ü���� ��� �̹����� ���� �翭�� ���۸� �ٽ� ��� �ؾ���
	�̸� ���� VkCommandBuffer��ü ����� Ŭ���� ����� ��������
	��� ���۴� ��� Ǯ�� �����Ǹ� �ڵ����� �����ǹǷ� ������� ������ �ʿ����� ����*/
	std::vector<VkCommandBuffer> commandBuffers;
	void createCommandBuffers();

	//��������
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	void createSemaphores();

public:
	Renderer();
	~Renderer();
};


#define RENDERER Renderer::GetInstance()


/*
���̴� �ڵ��� ���� �����͸� ���α׷� �ڵ��� �迭�� �ű�
���Ϳ� ��İ� ���� ���� ����� ���� ������ �����ϴ� glm���̺귯����
include�ϴ� ������ ���� ��ġ�� ���� ����
*/

/*
���ý� ���۴� �ڵ����� �޸𸮸� �Ҵ����� ���� ���� ��������
*/

/*
staging buffer
vertex buffer�� �ƴ� cpu���� ���� ������ ����. �� cpu�� ����ȭ�� ������.
cpu���� �� ���۸� �̿��ؼ� �����͸� ���⿡ �ְ� ���ؽ� ���ۿ� �����ؼ� �����

���� �츮�� ������ �ִ� ���ؽ� ���۴� ����� �۵������� �츮�� cpU���� �׼��� �� �� �ִ� �޸� Ÿ������ ���� �� �ִ� �޸𸮴� �׷��� ī�� ��ü�� ���� ������ �޸� Ÿ���� �ƴ� �� ����
- �׷��� ī�忡 ������ �޸� ���°� �ƴϷ�

���� ������ �޸𸮴� VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT �÷��׸� ���� �޸𸮷�, �Ϲ������� ���� �׷��� ī�忡 �����ϸ� CPU�� ���� �׼��� �� �� ����
�� �忡�� �츮�� �� ���� ���ؽ� ���۸� �����

���� ��̷κ��� �����͸� ���ε� �ϱ� ���� CPU�� �׼��� ������ �޸��� staging buffer�̰� �ٸ� �ϳ��� ����̽��� ���� �޸��� ���� ������
�츮�� staging ���۷κ��� �����͸� ���� ���� ���۷� �̵��ϴ� ���� ���� ����� ����� �� ����

- Transfer queue ���� ť
���� ���� ��ɿ��� ���� �۾��� �����ϴ� ��⿭ �йи��� �ʿ� VK_QUEUE_TRANSFER_BIT �� ����Ͽ� ǥ�õ�
���� �ҽ��� VK_QUEUE_GRAPHICS_BIT �Ǵ� VK_QUEUE_COMPUTE_BIT ����� �ִ� ��⿭ �йи��� �̹� VK_QUEUE_TRANSFER_BIT�۾��� �Ͻ������� �����Ѵٴ� ��
�� ��� ������ queueFlags�� ��������� ���� �� �ʿ䰡 ����
�ʿ䰡 ������ ���� �ʰ� �ְ� ������
QueueFamilyIndices �� findQueueFamilies�� �����Ͽ� VK_QUEUE_TRANSFER ��Ʈ�� ������ VK_QUEUE_GRAPHICS_BIT�� �ƴ� ��⿭ �йи��� ��������� ã��
Modify ���� ť�� ���� �ڵ��� ��û�ϱ� ���� createLogicalDevice
���۴�⿭�� ����� ��� ���ۿ� ���� �� ��° ��� Ǯ �����
���ҽ��� sharingMode�� VK_SHARING_MODE_CONCURRENT�� �����ϰ� �׷��� �� ���� ��⿭ �йи��� ��� �����ؾ���
vkCmdCopyBuffer �� ���� ���� ����� �׷��� ��⿭ ��� ���� ��⿭�� ������
*/
