#pragma once

#include "DeviceManager.h"

enum class ShaderType
{
	VS,GS,DS,HS,PS
};

class Shader
{
public:
	//void create();
	VkPipelineShaderStageCreateInfo& getDesc() { return pipeline_shader_stage_create_info_; }
	//void destroy();//rendercontainer�� pipeline ���� �� ȣ��Ǿ����
	//������ ���������� ���� �Ŀ��� �����ص� ������ .. 
	//���������ο��� shader�� set�� ���� ������ �� ��� �̿��ߴٰ� ���°�


private:
	std::string function_name_{};
	VkShaderModule shader_module_{};
	VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info_{};

	VkShaderModule createShaderModule(const std::vector<char>& code);
	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
public:
	Shader(ShaderType shader_type, std::string file_name, std::string function_name);
	~Shader();
};

