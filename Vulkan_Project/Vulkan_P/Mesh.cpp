#include "stdafx.h"
#include "Mesh.h"
#include "Renderer.h"
#include "shared.h"

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
	나중에 메쉬 정보 파일에 
	1. 정점의 타입이 우선 들어가고 지금같이 Vertex 구조체가 있는게 좋겠다 분리되어있으면 정점이 많아진다면 //캐시미스가// 많이날거 같음
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
