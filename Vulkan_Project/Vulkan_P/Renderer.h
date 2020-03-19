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

#include "RenderContainer.h"

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

	//render pipeline �Լ�
	
	//�� ��ü���� �־�� �� �����͸� �ϴ� ���⿡ ���� ���� 
#define INSTANCE_COUNT 10
	std::vector<std::shared_ptr<GameObject>> game_objects_;
 	//render pass
	VkRenderPass& getRenderPass() { return renderPass; }

	//frame buffer
	std::vector<VkFramebuffer>& getSwapChainFramebuffers() { return swapChainFramebuffers; }
	//command pool
	const VkCommandPool& getCommandPool() { return commandPool; }

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

	void updateUniformBuffer();

	//Draw
	void drawFrame();

	//helper
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
private:

	//render container 
	std::shared_ptr<RenderContainer> render_container_ = nullptr;

	//render pass
	void createRenderPass();
	VkRenderPass renderPass{};

	//frame buffers
	void createFramebuffers();
	std::vector<VkFramebuffer> swapChainFramebuffers{};

	/* ��� ���۸� ����� ���� ��� ���� ������ ��
	��� Ǯ�� ���۸� �����ϴ� �� ���Ǵ� �޸𸮸� �����ϸ� �� ��� ���۴� �ڽ��� ���۸� �Ҵ� */
	VkCommandPool commandPool{};
	void createCommandPool();

	/* ���� ��� ���۸� �Ҵ��ϰ� ��� ���۸� ����� �� ���� ����� ��� �� �ϳ��� �ùٸ� VkFramebuffer�� ���ε� �ϱ� ������ ������ ����ü���� ��� �̹����� ���� �翭�� ���۸� �ٽ� ��� �ؾ���
	�̸� ���� VkCommandBuffer��ü ����� Ŭ���� ����� ��������
	��� ���۴� ��� Ǯ�� �����Ǹ� �ڵ����� �����ǹǷ� ������� ������ �ʿ����� ����*/

	//��������
	VkSemaphore imageAvailableSemaphore{};
	VkSemaphore renderFinishedSemaphore{};
	void createSemaphores();

	//mesh info
	//std::shared_ptr<Mesh> rect_mesh_ = nullptr;

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
