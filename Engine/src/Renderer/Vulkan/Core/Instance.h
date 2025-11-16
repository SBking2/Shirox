#pragma once
#include<string>
#include<vector>
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace srx
{
	class Instance final
	{
	public:
		void Init();
		void Destroy();
		inline const VkInstance& GetInstance() const { return _instance; }
	private:
		void CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
		void CreateDebugCallback();
	private:
		VkInstance _instance;
		VkDebugUtilsMessengerEXT _debug_messager;	//存储回调函数信息
	};
}