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
		/* glfw의 콜백함수임 불행이도 인수로만 함수 포인터를 받아들이므로 멤버함수를 직접 사용할 수는 없음
		0이 아닌 거 체크 - 0채크 안하면 창이 최소화 되어 스왑체인 작성이 실패할 수 있음 */
		static void onWindowResized(GLFWwindow* window, int width, int height) {
			if (width == 0 || height == 0) return;
			TestRenderComponent* app = reinterpret_cast<TestRenderComponent*>(glfwGetWindowUserPointer(window));

			app->recreateSwapChain();
		}


		/*
		drawFrame함수는
		1. 스왑체인에서 이미지를 가져옴
		2. 해당 이미지가 있는 명령 버퍼를 프레임 버퍼의 어테치먼트로 실행
		3. 프레젠테이션을 위해 이미지를 스왑 체인으로 반환
		이러한 각 이벤트는 단일 함수 호출을 사용하여 동작하도록 설정되지만 비동기적으로 실행됨
		함수 호출은 작업이 실제로 완료되기 전에 반환되며 실행 순서도 정의되지 않음
		그것은 각각의 작업이 이전 작업에 의존하기 떄문에 어쩔 수 없는 일이기도 함
		스왑체인 이벤트를 동기화 하는 방법에는 펜스와 세마포어가 있음
		이들은 하나의 연산 신호를 가지고 연산을 조정하며 펜스나 세마포어가 unsignaled상태에서 signaled상태로 갈 떄까지 대기하는 데 사용할 수 있는 두 객체임
		차이점은 펜스의 상태는 vkWaitForFences와 같은 호출을 사용하여 프로그램에서 액세스 할 수 있으며 세마포어는 사용할 수 없다는 점
		ㅣ펜스는 주로 응용 프로그램 자체를 렌더링 작업과 동기화하는 반면 세마포어는 명령 대기열 내에서 또는 작업 대기열 간에 작업을 동기화하는 데 사용됨
		우리는 세마포어를 가장 적합하게 만드는 그리기 명령 및 프리제ㄴ테이션의 대기열 작업을 동기화 할것

		세마포어
		- 이미지를 가져와서 렌더링 할 준비가 되었음을 알리기 위해 하나의 세마포어가 필요하며 렌더링이 완료되고 프리젠테이션이 발생할 수 있음을 알리는 또다른 신호가 필요
		이러한 세마포어 객체를 저장할 두개의 클래스 멤버를 만듬

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

		/* 명령 버퍼를 만들기 전에 명령 플을 만들어야 함
		명령 풀은 버퍼를 저장하는 데 사용되는 메모리를 관리하며 각 명령 버퍼는 자신의 버퍼를 할당 */
		VkCommandPool commandPool;
		void createCommandPool();

		/* 이제 명령 버퍼를 할당하고 명령 버퍼를 기록할 수 있음 드로잉 명령 중 하나는 올바른 VkFramebuffer를 바인딩 하기 떄문에 실제로 스왑체인의 모든 이미지에 대한 며열ㅇ 버퍼를 다시 기록 해야함
		이를 위해 VkCommandBuffer객체 목록을 클래스 멤버로 만들어야함
		명령 버퍼는 명령 풀이 삭제되면 자동으로 해제되므로 명시적인 정리가 필요하지 않음*/
		std::vector<VkCommandBuffer> commandBuffers;
		void createCommandBuffers();

		//세마포어
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


