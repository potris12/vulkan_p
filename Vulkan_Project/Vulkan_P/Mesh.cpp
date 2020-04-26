#include "stdafx.h"
#include "Mesh.h"
#include "Renderer.h"
#include "shared.h"

void Mesh::awake()
{
	//createVertexBuffer();
	//createIndexBuffer();

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

	//instancing buffers
	for (auto instancing_buffer : instancing_buffers_) {
		instancing_buffer->destroy();
	}
}

void Mesh::draw(VkCommandBuffer & commandBuffer)
{
	if(vertex_buffer_) vertex_buffer_->registeCommandBuffer(commandBuffer, 0, 1, 0);//binding slot이랑 binding count는 buffer가 알아서 알도록 하자 뭐 offset도 필요없음 ㅇ
	for (auto instancing_buffer : instancing_buffers_)
	{
		instancing_buffer->registeCommandBuffer(commandBuffer);
	}
	
	if(index_buffer_) index_buffer_->registeCommandBuffer(commandBuffer, 0);

	if (index_buffer_) {
		vkCmdDrawIndexed(commandBuffer, index_buffer_->getDataCount(), INSTANCE_COUNT, 0, 0, 0);
	}
	else {
		vkCmdDraw(commandBuffer, vertex_buffer_->getDataCount(), INSTANCE_COUNT, 0, 0);
	}
	
}

std::vector<std::shared_ptr<InstancingBuffer>>& Mesh::addBufferDataStart()
{
	for (auto instancing_buffer : instancing_buffers_)
	{
		instancing_buffer->addBufferDataStart();
	}
	return instancing_buffers_;
}

void Mesh::addBufferDataEnd()
{
	for (auto instancing_buffer : instancing_buffers_)
	{
		instancing_buffer->addBufferDataEnd();
	}
}

void Mesh::setVertexInputState(VkPipelineVertexInputStateCreateInfo& vertex_input_state)
{
	vertex_input_state.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_bind_desc_.size());
	vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attribute_desc_.size());
	vertex_input_state.pVertexBindingDescriptions = vertex_input_bind_desc_.data();
	vertex_input_state.pVertexAttributeDescriptions = vertex_input_attribute_desc_.data();
	vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
}

void Mesh::setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo& input_assembly_state)
{
	input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//topology
	input_assembly_state.primitiveRestartEnable = VK_FALSE;//restart able
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
	for (const auto& format : vertex_formats)
	{
		vertex_input_attribute_desc_.push_back({ binding_location_++, binding_slot_, format, totla_size });

		totla_size += getFormatSize(format);
	}
	vertex_input_bind_desc_.push_back({ binding_slot_, totla_size, vertex_input_rate });


	binding_slot_++;
}

Mesh::Mesh(VkCommandPool& command_pool, std::string mesh_name)
	: Object(mesh_name), command_pool_(command_pool)
{

}


Mesh::~Mesh()
{
}
