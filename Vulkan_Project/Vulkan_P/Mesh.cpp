#include "Mesh.h"
#include "Renderer.h"


void Mesh::awake()
{


	createVertexBuffer();
	createIndexBuffer();

	/*vkCmdBindVertexBuffers
	vkCmdBindVertexBuffers 함수는 이전 장에서 설정 한 것과 같이 바인딩에 정점 버퍼를 바인딩하는 데 사용
	커멘드 버퍼 외의 최초의 2개의 파라미터는, 정점 버퍼를 지정하는 오프셋 및 바이너리의 수를 지정함
	마지막 두 매개 변수는 바인딩 할 정점 버퍼의 배열을 지정하고 정점 데이터를 읽는 바이트 오프셋을 지정함
	또한 vkCmdDraw에 대한 호출을 변경하여 하드 코드 된 숫자 3과 반대로 버퍼의 정점 수를 전달해야함
	*/

}

void Mesh::start()
{
}

void Mesh::update()
{
	
}

void Mesh::destroy()
{

	/*
	버퍼는 프로그램 종료까지 명령 렌더링에 사용할 수 있어야 하며 스왑체인에 의존하지 않으므로
	cleanup함수에서 호출하여 제거
	*/
	vertex_buffer_->destroy();
	index_buffer_->destroy();
}

void Mesh::registeConstantData(VkCommandBuffer & commandBuffer)
{
	auto vertexBuffer = vertex_buffer_->getBuffer();
	auto indexBuffer = index_buffer_->getBuffer();
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0/*offset ?? */, 1/*정점의 수*/, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices_.size()), 1/*인스턴스 수*/, 0, 0, 0);//draw는 어찌됬든 제일 마지막에 호출되야 함 
}

void Mesh::draw()
{
	//vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
	
}

void Mesh::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices_[0]) * indices_.size();
	auto usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	auto propertise = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	index_buffer_ = std::make_shared<Buffer>(bufferSize, usage, propertise);
	index_buffer_->map((void*)indices_.data());
	index_buffer_->unmap();
	index_buffer_->prepareBuffer();

}

void Mesh::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();
	auto usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	auto propertise = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	vertex_buffer_ = std::make_shared<Buffer>(bufferSize, usage, propertise);
	vertex_buffer_->map((void*)vertices_.data());
	vertex_buffer_->unmap();
	vertex_buffer_->prepareBuffer();

}

Mesh::Mesh(std::string mesh_name) : Object(mesh_name)
{
}


Mesh::~Mesh()
{
}
