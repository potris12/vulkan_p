#pragma once
#include "stdafx.h"
#include "Object.h"
#include "Buffer.h"
#include "Vertex.h"

class Mesh : public Object
{
public:
	void awake() override;
	void start() override;
	void update() override;
	void destroy() override;

	void registeConstantData(VkCommandBuffer& commandBuffer);
	void draw();
private:
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

	const float fx = 0.5f;
	const float fy = 0.5f;
	const float fz = 0.5f;
	
	const std::vector<Vertex> vertices_ = {
		{{-fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}},
		{{+fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f}},
		{{-fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}},
		{{+fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f}},
		{{-fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}},
		{{+fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f}},
		{{-fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}},
		{{+fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f}},
		{{-fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{-fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}},
		{{-fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{-fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{-fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f}},
		{{+fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, +fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}},
		{{+fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{+fx, +fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}},
		{{+fx, -fy, +fz}, { 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f}},
		{{+fx, -fy, -fz}, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f}}
	};

	/*const std::vector<uint16_t> indices_ = {
		0,1,2,2,3,0
	};*/

	const std::vector<uint16_t> indices_ = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		4,3,7,4,0,3,
		0,4,5,5,1,0,
		1,5,6,6,2,1,
		3,7,6,6,2,3
	};
	/*
	버택스 입력을 처리하는 방법을 설명하는 두 번째 구조는 VkVertexInputAttributeDescription임
	함
	*/

	//std::vector<Vertex> vertices_;
	std::shared_ptr<Buffer> vertex_buffer_ = nullptr;
	//std::vector<uint16_t> indices_;
	std::shared_ptr<Buffer> index_buffer_ = nullptr;

	void createIndexBuffer();
	void createVertexBuffer();

public:
	Mesh(std::string mesh_name);
	~Mesh();
};

