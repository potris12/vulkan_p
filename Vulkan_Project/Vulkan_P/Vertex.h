#pragma once
#include "stdafx.h"


struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	/*
	gpu 메모리에 업로드된 vulkan에게 이 데이터 형식을 버텍스 쉐이더에 전달하는 방법을 알려주는 것
	이 정보를 전달하는 데 필요한 두 가지 유형의 구조가 있음
	첫번째 구조체는 VkVertexInputBindingDescription이며 Vertex구조체에 멤버 함수를 추가하여 올바른 데이터로 채웁

	*/
	static VkVertexInputBindingDescription getBindingDesctiption() {
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;//이거 dx11레지스터 index
		bindingDescription.stride = sizeof(Vertex);//이거 dx11의 레이아웃 단위 지정할 떄 쓰는 그거
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//이거 vertex/instance구분 인자 (VK_VERTEX_INPUT_RATE_INCTANCE는 정점 그리고 정점 데이터 유지함)
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		//location0을 가진 버텍스 쉐이더의 input은 두개의 32비트 sfloat를 갖는 location임
		attributeDescriptions[0].binding = 0;//binding 매개변수는 Vulkan에게 각 정점 데이터가 바인딩되는 것을 알려줌
		attributeDescriptions[0].location = 0;//location은 버택스 쉐이더에서 입력의 location를 알려줌 
											  /*
											  format매개변수는 속성에 대한 데이터 유형을 설명함 색상 형식과 동일해 혼동이 가능함
											  VK_FORMAT_R32_SFLOAT
											  VK_FORMAT_R32G32_SFLOAT
											  VK_FORMAAT_R32G32B32_SFLOAT..
											  SINT, UINT, VK_FORMAT_R64_.. 각각 int uint double도 있음 ㅇㅇ
											  */
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//
																  /*
																  offsetof -
																  struct s_a{
																  short b;
																  int c;
																  }
																  s_a a;
																  가 있을 때 b의 위치는 &a인데 c의 위치 구할 때 사용하는 매크로
																  offsetof(struct s_a, c)인경우 아키텍쳐에 따라 2아니면 4가 됨 올..
																  */
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);


		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
		return attributeDescriptions;
	}
};
