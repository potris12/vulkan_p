#pragma once

#include "Mesh.h"
#include "Buffer.h"
#include "Vertex.h"

#include "GADComponent.h"

struct UniformBufferObject
{
	glm::mat4 world;
	glm::mat4 view;
	glm::mat4 proj;
};

struct UniformBufferObjectInstance
{
	glm::mat4 world;
};

namespace GADEngine {

	class TestRenderComponent : public GAD::GADComponent<TestRenderComponent>
	{

	public:
		void awake();
		void start() {};
		void update();
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
			TestRenderComponent* app = reinterpret_cast<TestRenderComponent*>(glfwGetWindowUserPointer(window));

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

		//uniform buffer
		void createDescriptorSetLayout();
		void createUniformBuffer();
		void createDescriptorPool();
		void createDescriptorSet();

		void updateUniformBuffer();

		//loading an image 
		void createTextureImage();
		//image view and sampler
		void createTextureImageView();
		//sampler
		void createTextureSampler();

		//Draw
		void drawFrame();

		VkCommandBuffer& getCurCommandBuffer() { return commandBuffers[imageIndex_]; }
	private:
		//cur swap chain image index
		uint32_t imageIndex_ = 0;

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

		/* ���� ��� ���۸� �Ҵ��ϰ� ��� ���۸� ����� �� ���� ����� ��� �� �ϳ��� �ùٸ� VkFramebuffer�� ���ε� �ϱ� ������ ������ ����ü���� ��� �̹����� ���� �翭�� ���۸� �ٽ� ��� �ؾ���
		�̸� ���� VkCommandBuffer��ü ����� Ŭ���� ����� ��������
		��� ���۴� ��� Ǯ�� �����Ǹ� �ڵ����� �����ǹǷ� ������� ������ �ʿ����� ����*/
		std::vector<VkCommandBuffer> commandBuffers;
		void createCommandBuffers();

		//��������
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;
		void createSemaphores();

		//mesh info
		std::shared_ptr<Mesh> rect_mesh_ = nullptr;

		//uniform buffer
		VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;
		VkBuffer instancingBuffer;
		VkDeviceMemory instancingBufferMemory;


		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;

		//load image
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;

		static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		//helper func
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void transitionImageLayout(VkImage image, VkFormat, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		//std::shared_ptr<Buffer> uniform_buffer_ = nullptr;
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		VkImageView textureImageView;
		VkSampler textureSampler;


		//depth image
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		void createDepthResources();
		VkFormat findDepthFormat();
		bool hasStencilComponent(VkFormat format);

	public:
		TestRenderComponent();
		~TestRenderComponent();
	};
}


