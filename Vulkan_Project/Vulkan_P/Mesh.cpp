#include "Mesh.h"
#include "Renderer.h"


#define INSTANCE_COUNT 10

void Mesh::awake()
{


	createVertexBuffer();
	//createIndexBuffer();

	/*vkCmdBindVertexBuffers
	vkCmdBindVertexBuffers 함수는 이전 장에서 설정 한 것과 같이 바인딩에 정점 버퍼를 바인딩하는 데 사용
	커멘드 버퍼 외의 최초의 2개의 파라미터는, 정점 버퍼를 지정하는 오프셋 및 바이너리의 수를 지정함
	마지막 두 매개 변수는 바인딩 할 정점 버퍼의 배열을 지정하고 정점 데이터를 읽는 바이트 오프셋을 지정함
	또한 vkCmdDraw에 대한 호출을 변경하여 하드 코드 된 숫자 3과 반대로 버퍼의 정점 수를 전달해야함
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
	버퍼는 프로그램 종료까지 명령 렌더링에 사용할 수 있어야 하며 스왑체인에 의존하지 않으므로
	cleanup함수에서 호출하여 제거
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
