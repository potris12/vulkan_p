#include "stdafx.h"
#include "Shader.h"

VkShaderModule Shader::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(DEVICE_MANAGER->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

Shader::Shader(ShaderType shader_type, std::string file_name, std::string function_name)
	:function_name_(function_name)
{
	VkShaderStageFlagBits shader_stage_flag_bit{};

	//VK_SHADER_STAGE_COMPUTE_BIT ´Â ´Ù¸§ 
	switch (shader_type)
	{
	case ShaderType::VS:
		shader_stage_flag_bit = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case ShaderType::GS:
		shader_stage_flag_bit = VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case ShaderType::DS:
		shader_stage_flag_bit = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		break;
	case ShaderType::HS:
		shader_stage_flag_bit = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		break;
	case ShaderType::PS:
		shader_stage_flag_bit = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		break;
	}

	auto shaderCode = readFile(file_name);
	shader_module_ = createShaderModule(shaderCode);

	
	pipeline_shader_stage_create_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline_shader_stage_create_info_.stage = shader_stage_flag_bit;
	pipeline_shader_stage_create_info_.module = shader_module_;
	pipeline_shader_stage_create_info_.pName = function_name_.c_str();
}

Shader::~Shader()
{
	vkDestroyShaderModule(DEVICE_MANAGER->getDevice(), shader_module_, nullptr);
}
