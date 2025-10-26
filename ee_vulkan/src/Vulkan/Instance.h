#pragma once
#include "Device.h"
#include<string>
#include<vector>
#define GLFW_INCLUED_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace ev
{
	class Instance final
	{
	public:
		void Init(GLFWwindow* window);
		void Destroy();
		inline VkInstance GetInstance() { return _instance; }
		inline VkSurfaceKHR GetSurfaceKHR() { return _surface; }
	private:
		void CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
		void CreateDebugCallback();
		void CreateSurface(GLFWwindow* window);
	private:
		VkInstance _instance;
		VkSurfaceKHR _surface;
		VkDebugUtilsMessengerEXT _debug_messager;	//存储回调函数信息
	};
}