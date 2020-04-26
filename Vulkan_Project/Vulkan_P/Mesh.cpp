#include "stdafx.h"
#include "Mesh.h"
#include "Renderer.h"
#include "shared.h"

void Mesh::awake()
{
	//createVertexBuffer();
	//createIndexBuffer();

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

	//instancing buffers
	for (auto instancing_buffer : instancing_buffers_) {
		instancing_buffer->destroy();
	}
}

void Mesh::draw(VkCommandBuffer & commandBuffer)
{
	if(vertex_buffer_) vertex_buffer_->registeCommandBuffer(commandBuffer, 0, 1, 0);//binding slot�̶� binding count�� buffer�� �˾Ƽ� �˵��� ���� �� offset�� �ʿ���� ��
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
