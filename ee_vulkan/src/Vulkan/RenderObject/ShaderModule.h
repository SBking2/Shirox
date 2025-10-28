#pragma once
#include "Vulkan/Core/Device.h"
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <memory>
namespace ev
{
	class ShaderModule final
	{
	public:
		static std::shared_ptr<ShaderModule> Create(const Device& device, const std::string& path);
		void Destroy(const Device& device);
		inline const VkShaderModule& GetShader() const { return _shader; }
	private:
		void Init(const Device& device, const std::string& path);
		void ReadFile(const std::string& path, std::vector<char>& source);
	private:
		VkShaderModule _shader;
	};
}