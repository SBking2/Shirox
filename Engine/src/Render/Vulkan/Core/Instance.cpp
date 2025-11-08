#include "PCH.h"
#include "Instance.h"
#include <iostream>
#include <stdexcept>
#include <map>
namespace srx
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		std::cerr << "validation layer : " << pCallbackData->pMessage << std::endl;
		std::cerr << std::endl;
		return VK_FALSE;
	}

	void Instance::Init()
	{
		#pragma region 准备好创建VkInstance需要的extension和layer
		//glfw需要的拓展
		uint32_t glfw_extension_count = 0;
		const char** gl_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };	//需要开启的层

		std::vector<const char*> required_extensions;	//需要开启的拓展
		for (int i = 0; i < glfw_extension_count; i++)
		{
			required_extensions.push_back(gl_extensions[i]);
		}
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);	//消息回调需要的拓展
		#pragma endregion

		CreateInstance(required_extensions, validation_layers);
		CreateDebugCallback();
	}

	void Instance::Destroy()
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(_instance, _debug_messager, nullptr);
		else
			throw std::runtime_error("failed to upset debug!");

		vkDestroyInstance(_instance, nullptr);
	}

	void Instance::CreateInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
	{
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "Hello_Triangle";
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "No_Engine";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;
		//打开glfw需要的拓展
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
		//开启校验层
		create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		create_info.ppEnabledLayerNames = layers.data();

		VkResult result = vkCreateInstance(&create_info, nullptr, &_instance);
		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to create vulkan instance");
	}
	void Instance::CreateDebugCallback()
	{
		//设置vulkan的消息回调
		VkDebugUtilsMessengerCreateInfoEXT create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		//指定回调会处理的消息级别
		create_info.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		//指定回调会处理的消息类型
		create_info.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debug_callback;
		create_info.pUserData = nullptr;

		//寻找函数地址
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");

		if (func == nullptr || func(_instance, &create_info, nullptr, &_debug_messager) != VK_SUCCESS)
			throw std::runtime_error("failed to set up debug callback!");
	}
}