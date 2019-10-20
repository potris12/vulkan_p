#pragma once

#include "Object.h"

#include "Mesh.h"
#include "Buffer.h"
#include "Vertex.h"

#include "UniformBuffer.h"
#include "Texture.h"

/*
render container와 같은 녀석
일단 object가 없으니까 랜더에 필요한 녀석들만 모아 놓도록 하자 
이녀석은 객체 종류마다 가질 수 있음 
나중에 혹시 멀티 랜더타겟을 가진 랜더를 할 수 있게 되는 경우에도 사용할 수 있도록 하자

 - 일단 renderer의 command pool을 공유하도록 함 command pool은 하나다! [[아직]]
*/

class RenderPipeline
{
public:
	void awake();
	void start() {};
	void update();
	void destroy();


	//render pipeline 함수
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

