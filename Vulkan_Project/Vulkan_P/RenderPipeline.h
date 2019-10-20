#pragma once

#include "Object.h"

#include "Mesh.h"
#include "Buffer.h"
#include "Vertex.h"

#include "UniformBuffer.h"
#include "Texture.h"

/*
render container�� ���� �༮
�ϴ� object�� �����ϱ� ������ �ʿ��� �༮�鸸 ��� ������ ���� 
�̳༮�� ��ü �������� ���� �� ���� 
���߿� Ȥ�� ��Ƽ ����Ÿ���� ���� ������ �� �� �ְ� �Ǵ� ��쿡�� ����� �� �ֵ��� ����

 - �ϴ� renderer�� command pool�� �����ϵ��� �� command pool�� �ϳ���! [[����]]
*/

class RenderPipeline
{
public:
	void awake();
	void start() {};
	void update();
	void destroy();


	//render pipeline �Լ�
	std::shared_ptr<UniformBuffer> addUniformBuffer(VkDeviceSize buffer_size, VkDeviceSize buffer_offset);
	void removeUniformBuffer(uint32_t binding_slot);

	std::shared_ptr<Texture> addTexture(const std::string& file_name);
	void removeTexture(uint32_t binding_slot);

private:
	std::vector<std::shared_ptr<UniformBuffer>> uniform_buffers_;
	std::vector<std::shared_ptr<Texture>> textures_;

public:
	RenderPipeline();
	~RenderPipeline();
};

