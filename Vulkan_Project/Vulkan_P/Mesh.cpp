#include "stdafx.h"
#include "Mesh.h"
#include "Renderer.h"
#include "shared.h"

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
	if(vertex_buffer_) vertex_buffer_->destroy();
	if(index_buffer_) index_buffer_->destroy();
}

void Mesh::draw(VkCommandBuffer & commandBuffer)
{
	if(vertex_buffer_) vertex_buffer_->registeCommandBuffer(commandBuffer, 0, 1, 0);
	
	if(index_buffer_) index_buffer_->registeCommandBuffer(commandBuffer, 0);

	if (index_buffer_) {
		vkCmdDrawIndexed(commandBuffer, index_buffer_->getIndexCount(), INSTANCE_COUNT, 0, 0, 0);
	}
	else {
		vkCmdDraw(commandBuffer, vertices_.size(), INSTANCE_COUNT, 0, 0);
	}
	
}

void Mesh::setVertexInputRateVertex(const std::vector<VkFormat>& vertex_formats)
{
	addInputLayout(VK_VERTEX_INPUT_RATE_VERTEX, vertex_formats);
}

void Mesh::addVertexInputRateInstance(const std::vector<VkFormat>& vertex_formats)
{
	addInputLayout(VK_VERTEX_INPUT_RATE_INSTANCE, vertex_formats);
}

void Mesh::addInputLayout(VkVertexInputRate vertex_input_rate, const std::vector<VkFormat>& vertex_formats)
{
	//vertex attribute
	uint32_t totla_size = 0;
	uint32_t location = 0;
	for (const auto& format : vertex_formats)
	{
		vertex_input_attribute_desc_.push_back({ location++, binding_slot_, format, totla_size });

		totla_size += getFormatSize(format);
	}
	vertex_input_bind_desc_.push_back({ binding_slot_, totla_size, vertex_input_rate });


	binding_slot_++;
}

void Mesh::createIndexBuffer()
{
	std::vector<uint16_t> indices{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		4,3,7,4,0,3,
		0,4,5,5,1,0,
		1,5,6,6,2,1,
		3,7,6,6,2,3 
	};

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	index_buffer_ = std::make_shared<IndexBufferT<uint16_t>>(command_pool_, indices);
}

void Mesh::createVertexBuffer()
{
	auto vertex_size = sizeof(vertices_[0]);
	VkDeviceSize bufferSize = vertex_size * vertices_.size();
	
	vertex_buffer_ = std::make_shared<VertexBuffer>(bufferSize);
	vertex_buffer_->prepareBuffer(command_pool_, (void*)vertices_.data());

	/*
	TODO LATER
	���߿� �޽� ���� ���Ͽ� 
	1. ������ Ÿ���� �켱 ���� ���ݰ��� Vertex ����ü�� �ִ°� ���ڴ� �и��Ǿ������� ������ �������ٸ� //ĳ�ù̽���// ���̳��� ����
	*/
	//vertex binding info
	setVertexInputRateVertex({ VK_FORMAT_R32G32B32_SFLOAT , VK_FORMAT_R32G32B32_SFLOAT , VK_FORMAT_R32G32_SFLOAT });
}

Mesh::Mesh(VkCommandPool& command_pool, std::string mesh_name) 
	: Object(mesh_name), command_pool_(command_pool)
{
}


Mesh::~Mesh()
{
}
