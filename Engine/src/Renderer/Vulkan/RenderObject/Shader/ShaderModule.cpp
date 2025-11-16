#include "PCH.h"
#include "ShaderModule.h"
#include <fstream>
namespace srx
{
	std::shared_ptr<ShaderModule> ShaderModule::Create(const Device& device, const std::string& path)
	{
		std::shared_ptr<ShaderModule> shader = std::make_shared<ShaderModule>();
		shader->Init(device, path);
		return shader;
	}
	void ShaderModule::Init(const Device& device, const std::string& path)
	{
		std::vector<char> source;
		ReadFile(path, source);
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = source.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(source.data());

		if (vkCreateShaderModule(device.GetLogicalDevice(), &create_info, nullptr, &_shader) != VK_SUCCESS)
			throw std::runtime_error("failed to create shader module!");
	}

	void ShaderModule::Destroy(const Device& device)
	{
		vkDestroyShaderModule(device.GetLogicalDevice(), _shader, nullptr);
	}

	/// <summary>
	/// 读取shader
	/// </summary>
	void ShaderModule::ReadFile(const std::string& path, std::vector<char>& source)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);		//ate模式是开始时处于文件尾部
		if (!file.is_open())
		{
			throw std::runtime_error("failed to read file : " + path);
		}

		size_t file_size = (size_t)file.tellg();
		source.resize(file_size);

		//将文件指针指到开头
		file.seekg(0);
		file.read(source.data(), file_size);

		file.close();
	}
}
