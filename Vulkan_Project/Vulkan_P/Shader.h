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
	//void destroy();//rendercontainer든 pipeline 제작 후 호출되어야함
	//솔직히 파이프라인 제작 후에는 삭제해도 무관함 .. 
	//파이프라인에서 shader를 set할 때와 제작할 때 잠깐 이용했다가 마는거


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

