#include "PCH.h"
#include "VulkanContext.h"

namespace srx
{
	////验证层的DebugCallback
	//static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	//	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	//	VkDebugUtilsMessageTypeFlagsEXT message_type,
	//	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	//	void* pUserData
	//)
	//{
	//	Log::LogError(pCallbackData->pMessage);
	//	return VK_FALSE;
	//}

	//void VulkanContext::Init(const Window* window)
	//{
	//	_window = window;

	//	//Root层
	//	CreateInstance();
	//	CreateDebugCallback();
	//	
	//	CreateSurface(window);

	//	FindBestDevice();

	//	//一切对象的起源 : Device
	//	CreateLogicalDevice();
	//	vkGetDeviceQueue(_device, device_info.graphic_queue_index, 0, &_graphic_queue);
	//	vkGetDeviceQueue(_device, device_info.present_queue_index, 0, &_present_queue);

	//	//第一层
	//	CreateCommandPool();
	//	CreateDescriptorPool();
	//}

	//void VulkanContext::Clear()
	//{
	//	vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);
	//	vkDestroyCommandPool(_device, _command_pool, nullptr);
	//	vkDestroyDevice(_device, nullptr);
	//	vkDestroySurfaceKHR(_instance, _surface, nullptr);

	//	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
	//	if (func != nullptr)
	//		func(_instance, _debug_messager, nullptr);
	//	else
	//		Log::LogError("Failed to Destroy debug callback!");

	//	vkDestroyInstance(_instance, nullptr);
	//}

	//void VulkanContext::CreateInstance()
	//{
	//	uint32_t glfw_extension_count;
	//	const char** gl_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	//	std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };	//需要开启的层

	//	std::vector<const char*> required_extensions;	//需要开启的拓展
	//	for (int i = 0; i < glfw_extension_count; i++)
	//		required_extensions.push_back(gl_extensions[i]);
	//	required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);	//消息回调需要的拓展

	//	VkApplicationInfo app_info = {};
	//	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	//	app_info.pApplicationName = "Hello_Triangle";
	//	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	//	app_info.pEngineName = "No_Engine";
	//	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	//	app_info.apiVersion = VK_API_VERSION_1_0;

	//	VkInstanceCreateInfo create_info = {};
	//	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	//	create_info.pApplicationInfo = &app_info;
	//	//打开glfw需要的拓展
	//	create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
	//	create_info.ppEnabledExtensionNames = required_extensions.data();
	//	//开启校验层
	//	create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
	//	create_info.ppEnabledLayerNames = validation_layers.data();

	//	VkResult result = vkCreateInstance(&create_info, nullptr, &_instance);
	//	if (result != VK_SUCCESS)
	//		Log::LogError("Failed to create vulkan instance!");
	//}

	////创建验证层回调
	//void VulkanContext::CreateDebugCallback()
	//{
	//	//设置vulkan的消息回调
	//	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	//	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	//	//指定回调会处理的消息级别
	//	create_info.messageSeverity =
	//		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	//		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	//		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	//	//指定回调会处理的消息类型
	//	create_info.messageType =
	//		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	//		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	//		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	//	create_info.pfnUserCallback = DebugCallback;
	//	create_info.pUserData = nullptr;

	//	//寻找函数地址
	//	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");

	//	if (func == nullptr || func(_instance, &create_info, nullptr, &_debug_messager) != VK_SUCCESS)
	//		Log::LogError("Failed to setup debug callback!");
	//}

	//void VulkanContext::CreateSurface(const Window* window)
	//{
	//	if (glfwCreateWindowSurface(_instance, window->GetWindow(), nullptr, &_surface) != VK_SUCCESS)
	//		Log::LogError("Failed to create surface!");
	//}

	//void VulkanContext::CreateLogicalDevice()
	//{
	//	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	//	//使用set的话，元素内容不会重复
	//	std::set<int> queue_indices = { device_info.graphic_queue_index, device_info.present_queue_index };

	//	//逻辑设备需要开的队列簇
	//	float queue_priority = 1.0f;		//队列优先级
	//	for (const auto& index : queue_indices)
	//	{
	//		VkDeviceQueueCreateInfo queue_create_info = {};
	//		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	//		queue_create_info.queueFamilyIndex = index;
	//		queue_create_info.queueCount = 1;	//暂时只使用带图像计算的队列
	//		queue_create_info.pQueuePriorities = &queue_priority;
	//		queue_create_infos.push_back(queue_create_info);
	//	}

	//	//逻辑设备需要的特性
	//	VkPhysicalDeviceFeatures features = {};
	//	features.samplerAnisotropy = VK_TRUE;
	//	features.sampleRateShading = VK_TRUE;

	//	std::vector<const char*> validation_layers = {
	//		"VK_LAYER_KHRONOS_validation"
	//	};

	//	std::vector<const char*> extensions =
	//	{
	//		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	//	};

	//	//创建逻辑设备
	//	VkDeviceCreateInfo create_info = {};
	//	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	//	create_info.pQueueCreateInfos = queue_create_infos.data();
	//	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	//	create_info.pEnabledFeatures = &features;
	//	//启用交换链拓展
	//	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	//	create_info.ppEnabledExtensionNames = extensions.data();
	//	//同样开启验证层
	//	create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
	//	create_info.ppEnabledLayerNames = validation_layers.data();

	//	VkResult result = vkCreateDevice(_physical_device, &create_info, nullptr,
	//		&_device);

	//	if (result != VK_SUCCESS)
	//		Log::LogError("Failed to create logical device!");
	//}

	//void VulkanContext::FindBestDevice()
	//{
	//	//获取physical device
	//	uint32_t device_count;
	//	vkEnumeratePhysicalDevices(_instance, &device_count, nullptr);
	//	std::vector<VkPhysicalDevice> devices(device_count);
	//	vkEnumeratePhysicalDevices(_instance, &device_count, devices.data());

	//	std::multimap<int, std::pair<DeviceInfo, VkPhysicalDevice>> score_devices_map;
	//	for (const VkPhysicalDevice& device : devices)
	//	{
	//		DeviceInfo info;
	//		int score = CheckDevice(info);
	//		if (score != 0)
	//			score_devices_map.insert(std::make_pair(score, std::make_pair(info, device)));
	//	}

	//	for (auto it = score_devices_map.rbegin(); it != score_devices_map.rend(); it++)
	//	{
	//		if (it->first > 0)
	//		{
	//			_physical_device = it->second.second;
	//			device_info = it->second.first;
	//			return;
	//		}
	//	}

	//	Log::LogError("Failed to find physical device!");
	//}

	//void VulkanContext::CreateCommandPool()
	//{
	//	VkCommandPoolCreateInfo command_pool_create_info = {};
	//	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//	command_pool_create_info.queueFamilyIndex = device_info.graphic_queue_index;
	//	command_pool_create_info.flags = 0;

	//	if (vkCreateCommandPool(_device, &command_pool_create_info, nullptr, &_command_pool)
	//		!= VK_SUCCESS)
	//		Log::LogError("Failed to create command pool!");
	//}

	//void VulkanContext::CreateDescriptorPool()
	//{
	//	//表示DescriptorPool能生成两种类型的DescriptorSet
	//	std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
	//	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//	pool_sizes[0].descriptorCount = 1;

	//	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//	pool_sizes[1].descriptorCount = 1;

	//	VkDescriptorPoolCreateInfo create_info = {};
	//	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//	create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	//	create_info.pPoolSizes = pool_sizes.data();

	//	create_info.maxSets = 1;

	//	if (vkCreateDescriptorPool(_device, &create_info, nullptr, &_descriptor_pool) != VK_SUCCESS)
	//		Log::LogError("failed to create descriptor pool!");
	//}

	//int VulkanContext::CheckDevice(DeviceInfo& device_info)
	//{
	//	int score = 0;

	//	//===================================计算分数
	//	{
	//		VkPhysicalDeviceProperties property;
	//		vkGetPhysicalDeviceProperties(_physical_device, &property);
	//		VkPhysicalDeviceFeatures features;
	//		vkGetPhysicalDeviceFeatures(_physical_device, &features);
	//		if (property.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)	//是否独显
	//			score += 1000;

	//		score += property.limits.maxImageDimension2D;	//image范围越大的显卡分数越高
	//		if (!features.geometryShader || !features.samplerAnisotropy)	//查询显卡是否支持几何着色器以及各向异性过滤
	//			score = 0;

	//		if (score == 0) return 0;
	//	}

	//	//===================================检查显卡的队列簇功能
	//	{
	//		uint32_t queue_family_count;
	//		vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queue_family_count, nullptr);
	//		std::vector<VkQueueFamilyProperties> properties(queue_family_count);
	//		vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queue_family_count, properties.data());

	//		for (int i = 0; i < properties.size(); i++)
	//		{
	//			if (device_info.graphic_queue_index == -1 && properties[i].queueCount > 0
	//				&& properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
	//			{
	//				device_info.graphic_queue_index = i;		//记录下具有Graphic功能的队列簇
	//			}

	//			if (device_info.present_queue_index == -1)
	//			{
	//				VkBool32 is_present_supported = false;
	//				vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, i, _surface, &is_present_supported);
	//				if (is_present_supported)
	//					device_info.present_queue_index = i;
	//			}

	//			if (device_info.graphic_queue_index != -1 && device_info.present_queue_index != -1)
	//				break;
	//		}

	//		if (device_info.graphic_queue_index == -1 || device_info.present_queue_index == -1)	//没有这两个功能
	//			return 0;
	//	}

	//	//===================================检查是否支持swapchian拓展
	//	{
	//		uint32_t extension_count;
	//		vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &extension_count, nullptr);
	//		std::vector<VkExtensionProperties> properties(extension_count);
	//		vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &extension_count, properties.data());

	//		bool is_supported_swapchain = false;

	//		for (const auto& extension : properties)
	//		{
	//			if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
	//			{
	//				is_supported_swapchain = true;
	//				break;
	//			}
	//		}

	//		if (!is_supported_swapchain) return 0;
	//	}

	//	//===================================检查显卡的swapchain是否与surface是否兼容
	//	{
	//		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface, &device_info.capabilities);

	//		uint32_t format_count;
	//		vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface, &format_count, nullptr);
	//		device_info.formats.resize(format_count);
	//		vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface, &format_count, device_info.formats.data());

	//		uint32_t present_mode_count;
	//		vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, _surface, &present_mode_count, nullptr);
	//		device_info.present_modes.resize(present_mode_count);
	//		vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, _surface, &present_mode_count, device_info.present_modes.data());

	//验证层的DebugCallback
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		SRX_LOG_ERROR(pCallbackData->pMessage);
		return VK_FALSE;
	}

	VulkanContext::VulkanContext()
	{
		
	}

	VulkanContext::~VulkanContext()
	{

	}

	void VulkanContext::Init()
	{
		CreateInstance();
		CreateDebugCallback();
		_PhysicalDevice = CreateRef<VulkanPhysicalDevice>();
		VkPhysicalDeviceFeatures features = {};
		_Device = CreateRef<VulkanDevice>(_PhysicalDevice, features);
	}

	void VulkanContext::Destroy()
	{
		_Device->Destroy();

		//卸载验证层
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Instance, "vkDestroyDebugUtilsMessengerEXT");
		SRX_ASSERT(func, "卸载Instance验证层失败!")
		func(_Instance, _DebugMessager, nullptr);
		
		vkDestroyInstance(_Instance, nullptr);
	}

	void VulkanContext::CreateInstance()
	{
		uint32_t glfw_extension_count;
		const char** gl_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };	//需要开启的层

		std::vector<const char*> required_extensions;	//需要开启的拓展
		for (int i = 0; i < glfw_extension_count; i++)
			required_extensions.push_back(gl_extensions[i]);
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);	//消息回调需要的拓展

		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "Shirox";
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "Shirox";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;
		//打开glfw需要的拓展
		create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
		create_info.ppEnabledExtensionNames = required_extensions.data();
		//开启校验层
		create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();

		VkResult result = vkCreateInstance(&create_info, nullptr, &_Instance);
		SRX_ASSERT(result == VK_SUCCESS, "创建Instance失败！");
	}

	void VulkanContext::CreateDebugCallback()
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
		create_info.pfnUserCallback = DebugCallback;
		create_info.pUserData = nullptr;

		//寻找函数地址
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Instance, "vkCreateDebugUtilsMessengerEXT");
		bool result = func == nullptr || func(_Instance, &create_info, nullptr, &_DebugMessager) != VK_SUCCESS;
		SRX_ASSERT(result == VK_SUCCESS, "创建Instance验证层失败!");
	}

//		if (device_info.formats.empty() || device_info.present_modes.empty())
	//			return 0;
	//	}
	//	return score;
	//}
}