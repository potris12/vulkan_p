#pragma once
#include "stdafx.h"


// Per-instance data block
struct InstanceData {
	glm::mat4 world_mtx;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
};


/*
gpu �޸𸮿� ���ε�� vulkan���� �� ������ ������ ���ؽ� ���̴��� �����ϴ� ����� �˷��ִ� ��
	�� ������ �����ϴ� �� �ʿ��� �� ���� ������ ������ ����
	ù��° ����ü�� VkVertexInputBindingDescription�̸� Vertex����ü�� ��� �Լ��� �߰��Ͽ� �ùٸ� �����ͷ� ä��


	static std::array<VkVertexInputBindingDescription, 2> getBindingDesctiption() {
		std::array<VkVertexInputBindingDescription, 2> bindingDescriptions = {};

		bindingDescriptions[0].binding = 0;//�̰� dx11�������� index
		bindingDescriptions[0].stride = sizeof(Vertex);//�̰� dx11�� ���̾ƿ� ���� ������ �� ���� �װ�
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//�̰� vertex/instance���� ���� (VK_VERTEX_INPUT_RATE_INCTANCE�� ���� �׸��� ���� ������ ������)


		bindingDescriptions[1].binding = 1;//�̰� dx11�������� index
		bindingDescriptions[1].stride = sizeof(InstanceData);//�̰� dx11�� ���̾ƿ� ���� ������ �� ���� �װ�
		bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;//�̰� vertex/instance���� ���� (VK_VERTEX_INPUT_RATE_INCTANCE�� ���� �׸��� ���� ������ ������)


		return bindingDescriptions;
	}

	static std::array<VkVertexInputAttributeDescription, 7> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions = {};

		//location0�� ���� ���ؽ� ���̴��� input�� �ΰ��� 32��Ʈ sfloat�� ���� location��
		attributeDescriptions[0].binding = 0;//binding �Ű������� Vulkan���� �� ���� �����Ͱ� ���ε��Ǵ� ���� �˷���
		attributeDescriptions[0].location = 0;//location�� ���ý� ���̴����� �Է��� location�� �˷���
											  
											  format�Ű������� �Ӽ��� ���� ������ ������ ������ ���� ���İ� ������ ȥ���� ������
											  VK_FORMAT_R32_SFLOAT
											  VK_FORMAT_R32G32_SFLOAT
											  VK_FORMAAT_R32G32B32_SFLOAT..
											  SINT, UINT, VK_FORMAT_R64_.. ���� int uint double�� ���� ����
											  
attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//
														  
														  offsetof -
														  struct s_a{
														  short b;
														  int c;
														  }
														  s_a a;
														  �� ���� �� b�� ��ġ�� &a�ε� c�� ��ġ ���� �� ����ϴ� ��ũ��
														  offsetof(struct s_a, c)�ΰ�� ��Ű���Ŀ� ���� 2�ƴϸ� 4�� �� ��..
														  
attributeDescriptions[0].offset = offsetof(Vertex, pos);

attributeDescriptions[1].binding = 0;
attributeDescriptions[1].location = 1;
attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
attributeDescriptions[1].offset = offsetof(Vertex, color);


attributeDescriptions[2].binding = 0;
attributeDescriptions[2].location = 2;
attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
attributeDescriptions[2].offset = offsetof(Vertex, texCoord);


attributeDescriptions[3].binding = 1;//binding �Ű������� Vulkan���� �� ���� �����Ͱ� ���ε��Ǵ� ���� �˷���
attributeDescriptions[3].location = 3;
attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
attributeDescriptions[3].offset = 0;

attributeDescriptions[4].binding = 1;
attributeDescriptions[4].location = 4;
attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
attributeDescriptions[4].offset = sizeof(float) * 4;


attributeDescriptions[5].binding = 1;
attributeDescriptions[5].location = 5;
attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
attributeDescriptions[5].offset = sizeof(float) * 8;


attributeDescriptions[6].binding = 1;
attributeDescriptions[6].location = 6;
attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
attributeDescriptions[6].offset = sizeof(float) * 12;
return attributeDescriptions;
	}
*/