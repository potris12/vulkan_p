#include "Mesh.h"
#include "Renderer.h"


#define INSTANCE_COUNT 10

void Mesh::awake()
{


	createVertexBuffer();
	//createIndexBuffer();

	/*vkCmdBindVertexBuffers
	vkCmdBindVertexBuffers �Լ��� ���� �忡�� ���� �� �Ͱ� ���� ���ε��� ���� ���۸� ���ε��ϴ� �� ���
	Ŀ��� ���� ���� ������ 2���� �Ķ���ʹ�, ���� ���۸� �����ϴ� ������ �� ���̳ʸ��� ���� ������
	������ �� �Ű� ������ ���ε� �� ���� ������ �迭�� �����ϰ� ���� �����͸� �д� ����Ʈ �������� ������
	���� vkCmdDraw�� ���� ȣ���� �����Ͽ� �ϵ� �ڵ� �� ���� 3�� �ݴ�� ������ ���� ���� �����ؾ���
	*/
	createInstanceBuffer();
}

void Mesh::start()
{
}

void Mesh::update()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	
	glm::mat4 world = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// Distribute rocks randomly on two different rings
	for (auto i = 0; i < INSTANCE_COUNT; i++) {

		instanceData[i].world_mtx = glm::translate(glm::mat4(1.0f), glm::vec3((i - INSTANCE_COUNT / 2) * 2, 0.0f, 0.0f)) * world;
	}

	instance_buffer_->mapWithUnmap((void*)instanceData.data());
}

void Mesh::destroy()
{

	/*
	���۴� ���α׷� ������� ��� �������� ����� �� �־�� �ϸ� ����ü�ο� �������� �����Ƿ�
	cleanup�Լ����� ȣ���Ͽ� ����
	*/
	vertex_buffer_->destroy();
	//index_buffer_->destroy();
	instance_buffer_->destroy();
}

void Mesh::createInstanceBuffer()
{
	instanceData.resize(INSTANCE_COUNT);
/*
	std::default_random_engine rndGenerator(benchmark.active ? 0 : (unsigned)time(nullptr));
	std::uniform_real_distribution<float> uniformDist(0.0, 1.0);
	std::uniform_int_distribution<uint32_t> rndTextureIndex(0, textures.rocks.layerCount);
*/
	// Distribute rocks randomly on two different rings
	for (auto i = 0; i < INSTANCE_COUNT; i++) {
		
		instanceData[i].world_mtx = glm::translate(glm::mat4(1.0f), glm::vec3((i - INSTANCE_COUNT / 2)* 2, 0.0f, 0.0f));
	}

	// Staging
	// Instanced data is static, copy to device local memory 
	// This results in better performance

	struct {
		VkDeviceMemory memory;
		VkBuffer buffer;
	} stagingBuffer;

	VkDeviceSize bufferSize = sizeof(instanceData[0]) * instanceData.size();
	auto usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	//VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
	//	| VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
	//	| VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	auto propertise = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	instance_buffer_ = std::make_shared<Buffer>(bufferSize, usage, propertise);
	instance_buffer_->mapWithUnmap((void*)instanceData.data());
	//instance_buffer_->map((void*)instanceData.data());
	//instance_buffer_->unmap();
	//instance_buffer_->prepareBuffer();
}

void Mesh::draw(VkCommandBuffer & commandBuffer)
{
	vertex_buffer_->registeCommandBuffer(commandBuffer, 0, 1, 0);
	instance_buffer_->registeCommandBuffer(commandBuffer, 1, 1, 0);

	vkCmdDraw(commandBuffer, vertices_.size(), INSTANCE_COUNT, 0, 0);
}

void Mesh::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices_[0]) * indices_.size();
	auto usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	auto propertise = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	index_buffer_ = std::make_shared<Buffer>(bufferSize, usage, propertise);
	index_buffer_->prepareBuffer((void*)indices_.data());

}

void Mesh::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();
	auto usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	auto propertise = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	vertex_buffer_ = std::make_shared<Buffer>(bufferSize, usage, propertise);
	vertex_buffer_->prepareBuffer((void*)vertices_.data());

}

Mesh::Mesh(std::string mesh_name) : Object(mesh_name)
{
}


Mesh::~Mesh()
{
}
