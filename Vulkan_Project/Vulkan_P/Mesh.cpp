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
	vertex_buffer_->destroy();
	index_buffer_->destroy();
}

void Mesh::draw(VkCommandBuffer & commandBuffer)
{
	vertex_buffer_->registeCommandBuffer(commandBuffer, 0, 1, 0);
	
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
