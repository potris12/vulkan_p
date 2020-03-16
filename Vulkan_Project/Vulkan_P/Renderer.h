#pragma once
#include "stdafx.h"

#include "Singleton.h"

#include "Mesh.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Shader.h"

#include "UniformBuffer.h"
#include "Texture.h"

#include "GameObject.h"

struct UniformBufferObject
{
	glm::mat4 world;
	glm::mat4 view;
	glm::mat4 proj;

	UniformBufferObject() {
		world = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		proj = glm::mat4(1.0f);
	};
};

class Renderer : public CSingleTonBase<Renderer>
{
public:
	void awake();
	void start() {};
	void update();
	void destroy();

	//render pipeline 함수
	template <typename T>
	std::shared_ptr<UniformBuffer> addUniformBuffer(uint32_t data_num)
	{
		static uint32_t binding_slot = 0;

		auto uniform_buffer = std::make_shared<UniformBufferT<T>>(data_num, binding_slot);
		uniform_buffers_.push_back(uniform_buffer);
		++binding_slot;

		return uniform_buffer;
	}
	std::shared_ptr<Texture> addTexture(const std::string& file_name);

	std::vector<std::shared_ptr<UniformBuffer>> uniform_buffers_;
	std::vector<std::shared_ptr<Texture>> textures_;
	
	//각 객체내에 있어야 할 데이터를 일단 여기에 꺼내 놓음 
#define INSTANCE_COUNT 10
	std::vector<InstanceData> instance_data_;
	std::vector<std::shared_ptr<GameObject>> game_objects_;
 	//render pass
	VkRenderPass& getRenderPass() { return renderPass; }

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
		Renderer* app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));

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
	void createInstanceBuffer();

	void createDescriptorPool();
	void createDescriptorSet();

	void updateUniformBuffer();

	void createTexture();
	//Draw
	void drawFrame();

	VkCommandBuffer& getCurCommandBuffer();

	//helper
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
private:

	//render pass
	void createRenderPass();
	VkRenderPass renderPass{};

	//pipeline
	void createGraphicsPipeline();
	VkPipelineLayout pipelineLayout{};
	VkPipeline graphicsPipeline{};

	//frame buffers
	void createFramebuffers();
	std::vector<VkFramebuffer> swapChainFramebuffers{};

	/* 명령 버퍼를 만들기 전에 명령 플을 만들어야 함
	명령 풀은 버퍼를 저장하는 데 사용되는 메모리를 관리하며 각 명령 버퍼는 자신의 버퍼를 할당 */
	VkCommandPool commandPool{};
	void createCommandPool();

	/* 이제 명령 버퍼를 할당하고 명령 버퍼를 기록할 수 있음 드로잉 명령 중 하나는 올바른 VkFramebuffer를 바인딩 하기 떄문에 실제로 스왑체인의 모든 이미지에 대한 며열ㅇ 버퍼를 다시 기록 해야함
	이를 위해 VkCommandBuffer객체 목록을 클래스 멤버로 만들어야함
	명령 버퍼는 명령 풀이 삭제되면 자동으로 해제되므로 명시적인 정리가 필요하지 않음*/
	std::vector<VkCommandBuffer> commandBuffers;
	void createCommandBuffers();

	//세마포어
	VkSemaphore imageAvailableSemaphore{};
	VkSemaphore renderFinishedSemaphore{};
	void createSemaphores();

	//mesh info
	std::shared_ptr<Mesh> rect_mesh_ = nullptr;

	VkDescriptorSetLayout descriptorSetLayout{};
	VkDescriptorPool descriptorPool{};
	VkDescriptorSet descriptorSet{};

	//helper func
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	

	//depth image
	VkImage depthImage{};
	VkDeviceMemory depthImageMemory{};
	VkImageView depthImageView{};

	void createDepthResources();
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

public:
	Renderer();
	~Renderer();
};


#define RENDERER Renderer::GetInstance()


/*
셰이더 코드의 정점 데이터를 프로그램 코드의 배열로 옮김
벡터와 행렬과 같은 선형 대수학 관련 유형을 제공하는 glm라이브러리를
include하는 것으로 시작 위치와 벡터 지정
*/

/*
버택스 버퍼는 자동으로 메모리를 할당하지 않음 내가 만들어야해
*/

/*
staging buffer
vertex buffer가 아닌 cpu에서 접근 가능한 버퍼. 좀 cpu에 최적화된 버퍼임.
cpu용인 이 버퍼를 이용해서 데이터를 여기에 넣고 버텍스 버퍼에 복사해서 사용함

지금 우리가 가지고 있는 버텍스 버퍼는 제대로 작동하지만 우리가 cpU에서 액세스 할 수 있는 메모리 타입으로 읽을 수 있는 메모리는 그래픽 카드 자체에 대한 최적의 메모리 타입이 아닐 수 있음
- 그래픽 카드에 최적의 메모리 형태가 아니래

가장 최적의 메모리는 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT 플래그를 갖는 메모리로, 일반적으로 전용 그래픽 카드에 적합하며 CPU에 의해 액세스 할 수 없음
이 장에서 우리는 두 개의 버텍스 버퍼를 만들것

정점 어레이로부터 데이터를 업로드 하기 위한 CPU가 액세스 가능한 메모리인 staging buffer이고 다른 하나는 디바이스의 로컬 메모리인 정점 버퍼임
우리는 staging 버퍼로부터 데이터를 실제 정점 버퍼로 이동하는 버퍼 복사 명령을 사용할 수 있음

- Transfer queue 전송 큐
버퍼 복사 명령에는 전송 작업을 지원하는 대기열 패밀리가 필요 VK_QUEUE_TRANSFER_BIT 를 사용하여 표시됨
좋은 소식은 VK_QUEUE_GRAPHICS_BIT 또는 VK_QUEUE_COMPUTE_BIT 기능이 있는 대기열 패밀리가 이미 VK_QUEUE_TRANSFER_BIT작업을 암시적으로 지원한다는 것
이 경우 구현시 queueFlags에 명시적으로 나열 할 필요가 없음
필요가 없지만 만약 너가 넣고 싶으면
QueueFamilyIndices 및 findQueueFamilies를 수정하여 VK_QUEUE_TRANSFER 비트가 있지만 VK_QUEUE_GRAPHICS_BIT가 아닌 대기열 패밀리를 명시적으로 찾음
Modify 전송 큐에 대한 핸들을 요청하기 위한 createLogicalDevice
전송대기열에 제출된 명령 버퍼에 대한 두 번째 명령 풀 만들기
리소스의 sharingMode를 VK_SHARING_MODE_CONCURRENT로 변경하고 그래픽 및 전송 대기열 패밀리를 모두 지정해야함
vkCmdCopyBuffer 와 같은 전송 명령을 그래픽 대기열 대신 전송 대기열에 제출함
*/
