#pragma once
#include "stdafx.h"
#include "Object.h"
#include "Buffer.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "InstancingBuffer.h"


class Mesh : public Object
{
public:
	void awake() override;
	void start() override;
	void update() override;
	void destroy() override;

	void draw(VkCommandBuffer& commandBuffer);
	/* instancing buffer data */
	std::shared_ptr<InstancingBuffer> addInstancingBuffer(VkDeviceSize buffer_size);
	std::vector<std::shared_ptr<InstancingBuffer>>& getInstancingBufferData() { return instancing_buffers_;  }
private:
	//std::vector<Vertex> vertices_;
	std::vector< VkVertexInputBindingDescription> vertex_input_bind_desc_;//vertex binding info 
	void setVertexInputRateVertex(const std::vector<VkFormat>& vertex_formats);
	void addVertexInputRateInstance(const std::vector<VkFormat>& vertex_formats);
	void addInputLayout(VkVertexInputRate vertex_input_rate, const std::vector<VkFormat>& vertex_formats);

	uint32_t binding_slot_ = 0;

	std::vector< VkVertexInputAttributeDescription> vertex_input_attribute_desc_;//vertex info

	std::shared_ptr<VertexBuffer> vertex_buffer_ = nullptr;
	std::shared_ptr<IndexBuffer> index_buffer_ = nullptr;
	std::vector<std::shared_ptr<InstancingBuffer>> instancing_buffers_;

	VkCommandPool& command_pool_;

	void createIndexBuffer();
	void createVertexBuffer();
public:
	Mesh(VkCommandPool& command_pool, std::string mesh_name);
	~Mesh();
};

/*const std::vector<uint16_t> indices_ = {
	0,1,2,2,3,0
};*/

/*
버택스 입력을 처리하는 방법을 설명하는 두 번째 구조는 VkVertexInputAttributeDescription임
함
*/



////이게 정점 데이터 인풋레이아웃임 
//const std::vector<Vertex> vertices = {
//	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
//	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
//	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
//};

/*
인덱스 데이터
*/
/*const std::vector<Vertex> vertices_ = {
	{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f} },
{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } }
};*/
/*
{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
{ { 0.5f, -0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
{ { 0.5f, 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
{ { -0.5f, 0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

{ { -0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
{ { 0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
{ { 0.5f, 0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
{ { -0.5f, 0.5f, -0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } }*/