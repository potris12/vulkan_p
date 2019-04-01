#pragma once
#include "stdafx.h"


struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	/*
	gpu �޸𸮿� ���ε�� vulkan���� �� ������ ������ ���ؽ� ���̴��� �����ϴ� ����� �˷��ִ� ��
	�� ������ �����ϴ� �� �ʿ��� �� ���� ������ ������ ����
	ù��° ����ü�� VkVertexInputBindingDescription�̸� Vertex����ü�� ��� �Լ��� �߰��Ͽ� �ùٸ� �����ͷ� ä��

	*/
	static VkVertexInputBindingDescription getBindingDesctiption() {
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;//�̰� dx11�������� index
		bindingDescription.stride = sizeof(Vertex);//�̰� dx11�� ���̾ƿ� ���� ������ �� ���� �װ�
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//�̰� vertex/instance���� ���� (VK_VERTEX_INPUT_RATE_INCTANCE�� ���� �׸��� ���� ������ ������)
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		//location0�� ���� ���ؽ� ���̴��� input�� �ΰ��� 32��Ʈ sfloat�� ���� location��
		attributeDescriptions[0].binding = 0;//binding �Ű������� Vulkan���� �� ���� �����Ͱ� ���ε��Ǵ� ���� �˷���
		attributeDescriptions[0].location = 0;//location�� ���ý� ���̴����� �Է��� location�� �˷��� 
											  /*
											  format�Ű������� �Ӽ��� ���� ������ ������ ������ ���� ���İ� ������ ȥ���� ������
											  VK_FORMAT_R32_SFLOAT
											  VK_FORMAT_R32G32_SFLOAT
											  VK_FORMAAT_R32G32B32_SFLOAT..
											  SINT, UINT, VK_FORMAT_R64_.. ���� int uint double�� ���� ����
											  */
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//
																  /*
																  offsetof -
																  struct s_a{
																  short b;
																  int c;
																  }
																  s_a a;
																  �� ���� �� b�� ��ġ�� &a�ε� c�� ��ġ ���� �� ����ϴ� ��ũ��
																  offsetof(struct s_a, c)�ΰ�� ��Ű���Ŀ� ���� 2�ƴϸ� 4�� �� ��..
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
