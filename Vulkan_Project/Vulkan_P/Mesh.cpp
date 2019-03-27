#include "Mesh.h"
#include "Renderer.h"


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
	���۴� ���α׷� ������� ��� �������� ����� �� �־�� �ϸ� ����ü�ο� �������� �����Ƿ�
	cleanup�Լ����� ȣ���Ͽ� ����
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
	vkCmdBindVertexBuffers(commandBuffer, 0/*offset ?? */, 1/*������ ��*/, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices_.size()), 1/*�ν��Ͻ� ��*/, 0, 0, 0);//draw�� ������ ���� �������� ȣ��Ǿ� �� 
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
