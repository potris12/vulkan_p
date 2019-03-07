#include "stdafx.h"
#include "Buffer.h"

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


//���ؽ� ���� �߰�
#include <glm/glm.hpp>
#include <array>

const int WIDTH = 800;
const int HEIGHT = 600;
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
struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	/*
	gpu �޸𸮿� ���ε�� vulkan���� �� ������ ������ ���ؽ� ���̴��� �����ϴ� ����� �˷��ִ� ��
	�� ������ �����ϴ� �� �ʿ��� �� ���� ������ ������ ����
	ù��° ����ü�� VkVertexInputBindingDescription�̸� Vertex����ü�� ��� �Լ��� �߰��Ͽ� �ùٸ� �����ͷ� ä��

	*/
	static VkVertexInputBindingDescription getBindingDesctiption() {
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;//�̰� dx11�������� index
		bindingDescription.stride = sizeof(Vertex);//�̰� dx11�� ���̾ƿ� ���� ������ �� ���� �װ�
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//�̰� vertex/instance���� ���� (VK_VERTEX_INPUT_RATE_INCTANCE�� ���� �׸��� ���� ������ ������)
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		//location0�� ���� ���ؽ� ���̴��� input�� �ΰ��� 32��Ʈ sfloat�� ���� location��
		attributeDescriptions[0].binding = 0;//binding �Ű������� Vulkan���� �� ���� �����Ͱ� ���ε��Ǵ� ���� �˷���
		attributeDescriptions[0].location = 0;//location�� ���ý� ���̴����� �Է��� location�� �˷��� 
											  /*
											  format�Ű������� �Ӽ��� ���� ������ ������ ������ ���� ���İ� ������ ȥ���� ������
											  VK_FORMAT_R32_SFLOAT
											  VK_FORMAT_R32G32_SFLOAT
											  VK_FORMAAT_R32G32B32_SFLOAT..
											  SINT, UINT, VK_FORMAT_R64_.. ���� int uint double�� ���� ����
											  */
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;//
																  /*
																  offsetof -
																  struct s_a{
																  short b;
																  int c;
																  }
																  s_a a;
																  �� ���� �� b�� ��ġ�� &a�ε� c�� ��ġ ���� �� ����ϴ� ��ũ��
																  offsetof(struct s_a, c)�ΰ�� ��Ű���Ŀ� ���� 2�ƴϸ� 4�� �� ��..
																  */
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

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

class Framework : public std::enable_shared_from_this<Framework>
{
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	const VkDevice& getDevice() { return device; }
	const VkCommandPool& getCommandPool() { return commandPool; }
	const VkQueue& getGraphicsQueue() { return graphicsQueue; }
	const VkPhysicalDevice& getPhysicalDevice() { return physicalDevice;}
	VkPhysicalDeviceMemoryProperties& getPhysicalDeviceMemoryProperties() { return memProperties; }
private:

	VkPhysicalDeviceMemoryProperties memProperties;

	std::shared_ptr<Buffer> vertex_buffer_ = nullptr;
	std::shared_ptr<Buffer> index_buffer_ = nullptr;

	GLFWwindow * window;//s

	VkInstance instance;
	VkDebugUtilsMessengerEXT callback;
	VkSurfaceKHR surface;//s

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;//s

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;

	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	/* ��� ���۸� ����� ���� ��� ���� ������ ��
	��� Ǯ�� ���۸� �����ϴ� �� ���Ǵ� �޸𸮸� �����ϸ� �� ��� ���۴� �ڽ��� ���۸� �Ҵ� */
	VkCommandPool commandPool;

	/* ���� ��� ���۸� �Ҵ��ϰ� ��� ���۸� ����� �� ���� ����� ��� �� �ϳ��� �ùٸ� VkFramebuffer�� ���ε� �ϱ� ������ ������ ����ü���� ��� �̹����� ���� �翭�� ���۸� �ٽ� ��� �ؾ���
	�̸� ���� VkCommandBuffer��ü ����� Ŭ���� ����� ��������
	��� ���۴� ��� Ǯ�� �����Ǹ� �ڵ����� �����ǹǷ� ������� ������ �ʿ����� ����*/
	std::vector<VkCommandBuffer> commandBuffers;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;


	/*
	���ý� ����
	*/
	

	void initWindow();

	/* glfw�� �ݹ��Լ��� �����̵� �μ��θ� �Լ� �����͸� �޾Ƶ��̹Ƿ� ����Լ��� ���� ����� ���� ����
	0�� �ƴ� �� üũ - 0äũ ���ϸ� â�� �ּ�ȭ �Ǿ� ����ü�� �ۼ��� ������ �� ���� */
	static void onWindowResized(GLFWwindow* window, int width, int height) {
		if (width == 0 || height == 0) return;
		Framework* app = reinterpret_cast<Framework*>(glfwGetWindowUserPointer(window));

		app->recreateSwapChain();
	}

	VkExtent2D chooseSwapExtent();
	void initVulkan();

	void cleanupSwapChain();
	void recreateSwapChain();
	void mainLoop();

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
	void drawFrame();

	
	void cleanup();

	void createInstance();

	void setupDebugCallback();

	void createSurface();

	void pickPhysicalDevice();

	void createLogicalDevice();

	void createSwapChain();

	void createImageViews();

	void createRenderPass();

	void createGraphicsPipeline();

	void createFramebuffers();

	void createCommandPool();

	void createVertexBuffer();

	void createIndexBuffer();
	
	void createCommandBuffers();

	void createSemaphores();

	VkShaderModule createShaderModule(const std::vector<char>& code);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	bool isDeviceSuitable(VkPhysicalDevice device);

	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	std::vector<const char*> getRequiredExtensions();

	bool checkValidationLayerSupport();

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

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}


public:
	std::shared_ptr<Framework> create()
	{
		return std::make_shared<Framework>();
	}

	Framework() = default;
};
