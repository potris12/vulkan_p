#include "Mesh.h"
#include "Renderer.h"


#define INSTANCE_COUNT 10

void Mesh::awake()
{


	createVertexBuffer();
	createIndexBuffer();

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

	instancing_buffer_->prepareBuffer( command_pool_, (void*)instanceData.data());
}

void Mesh::destroy()
{
	vertex_buffer_->destroy();
	index_buffer_->destroy();
	instancing_buffer_->destroy();//�̰� ��� render container ? render pipe line�� �����ؾ��� 
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

	VkDeviceSize bufferSize = sizeof(instanceData[0]) * instanceData.size();
	
	instancing_buffer_ = std::make_shared<InstancingBuffer>(bufferSize);
	instancing_buffer_->prepareBuffer(command_pool_, (void*)instanceData.data());
}

void Mesh::draw(VkCommandBuffer & commandBuffer)
{
	vertex_buffer_->registeCommandBuffer(commandBuffer, 0, 1, 0);
	instancing_buffer_->registeCommandBuffer(commandBuffer, 1, 1, 0);
	index_buffer_->registeCommandBuffer(commandBuffer, 0);

	if (index_buffer_) {
		vkCmdDrawIndexed(commandBuffer, indices_.size(), INSTANCE_COUNT, 0, 0, 0);
	}
	else {
		vkCmdDraw(commandBuffer, vertices_.size(), INSTANCE_COUNT, 0, 0);
	}
	
}

void Mesh::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices_[0]) * indices_.size();

	index_buffer_ = std::make_shared<IndexBuffer>(bufferSize, VK_INDEX_TYPE_UINT16);
	index_buffer_->prepareBuffer(command_pool_, (void*)indices_.data());
}

void Mesh::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();
	
	vertex_buffer_ = std::make_shared<VertexBuffer>(bufferSize);
	vertex_buffer_->prepareBuffer(command_pool_, (void*)vertices_.data());

}

Mesh::Mesh(VkCommandPool& command_pool, std::string mesh_name) 
	: Object(mesh_name), command_pool_(command_pool)
{
}


Mesh::~Mesh()
{
}
