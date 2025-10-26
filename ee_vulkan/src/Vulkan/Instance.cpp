#include "Instance.h"
#include <iostream>
#include <stdexcept>
#include <map>
namespace ev
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

	void Instance::Init(GLFWwindow* window)
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
		CreateSurface(window);
		CreateDevice();
	}

	void Instance::Destroy()
	{
		_device.Destroy();

		vkDestroySurfaceKHR(_instance, _surface, nullptr);

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
	void Instance::CreateSurface(GLFWwindow* window)
	{
		if (glfwCreateWindowSurface(_instance, window, nullptr, &_surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create surface!");
	}

	void Instance::CreateDevice()
	{
		//获取physical device
		uint32_t device_count;
		vkEnumeratePhysicalDevices(_instance, &device_count, nullptr);
		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(_instance, &device_count, devices.data());

		std::multimap<int, Device> score_devices_map;
		for (const VkPhysicalDevice& device : devices)
		{
			Device logical_device = {};
			logical_device.SetPhysicalDevice(device);
			int score = CheckDevice(device, logical_device);
			if (score != 0)
				score_devices_map.insert(std::make_pair(score, logical_device));
		}

		bool is_find = false;
		for (auto it = score_devices_map.rbegin(); it != score_devices_map.rend(); it++)
		{
			if (it->first > 0)
			{
				_device = it->second;
				is_find = true;
				break;
			}
		}

		if (!is_find)
			throw std::runtime_error("failed to find physical device!");

		_device.Init();
	}

	int Instance::CheckDevice(VkPhysicalDevice physical_device, Device& device)
	{
		int score = 0;

		//===================================计算分数
		{
			VkPhysicalDeviceProperties property;
			vkGetPhysicalDeviceProperties(physical_device, &property);
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(physical_device, &features);
			if (property.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)	//是否独显
				score += 1000;

			score += property.limits.maxImageDimension2D;	//image范围越大的显卡分数越高
			if (!features.geometryShader || !features.samplerAnisotropy)	//查询显卡是否支持几何着色器以及各向异性过滤
				score = 0;

			if (score == 0) return 0;
		}

		//===================================检查显卡的队列簇功能
		{
			uint32_t queue_family_count;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
			std::vector<VkQueueFamilyProperties> properties(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, properties.data());

			for (int i = 0; i < properties.size(); i++)
			{
				if (device.device_info.graphic_queue_index == -1 && properties[i].queueCount > 0
					&& properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					device.device_info.graphic_queue_index = i;		//记录下具有Graphic功能的队列簇
				}

				if (device.device_info.present_queue_index == -1)
				{
					VkBool32 is_present_supported = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, _surface, &is_present_supported);
					if (is_present_supported)
						device.device_info.present_queue_index = i;
				}

				if (device.device_info.graphic_queue_index != -1 && device.device_info.present_queue_index != -1)
					break;
			}

			if (device.device_info.graphic_queue_index == -1 || device.device_info.present_queue_index == -1)	//没有这两个功能
				return 0;
		}

		//===================================检查是否支持swapchian拓展
		{
			uint32_t extension_count;
			vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
			std::vector<VkExtensionProperties> properties(extension_count);
			vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, properties.data());

			bool is_supported_swapchain = false;

			for (const auto& extension : properties)
			{
				if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
				{
					is_supported_swapchain = true;
					break;
				}
			}

			if (!is_supported_swapchain) return 0;
		}

		//===================================检查显卡的swapchain是否与surface是否兼容
		{
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, _surface, &device.device_info.capabilities);

			uint32_t format_count;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _surface, &format_count, nullptr);
			device.device_info.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, _surface, &format_count, device.device_info.formats.data());

			uint32_t present_mode_count;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, _surface, &present_mode_count, nullptr);
			device.device_info.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, _surface, &present_mode_count, device.device_info.present_modes.data());

			if (device.device_info.formats.empty() || device.device_info.present_modes.empty())
				return 0;
		}
		return score;
	}
	
}