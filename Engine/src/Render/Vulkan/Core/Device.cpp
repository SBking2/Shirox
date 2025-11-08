#include "PCH.h"
#include "Device.h"
#include<set>
#include <map>
#include<stdexcept>
namespace srx
{
	void Device::Init(const Instance& instance, const Surface& surface)
	{
		FindBestDevice(instance, surface);
		CreateDevice(_physical_device);
	}
	void Device::Destroy()
	{
		vkDestroyDevice(_logical_device, nullptr);
	}

	uint32_t Device::FindMemoryType(uint32_t filter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(_physical_device, &mem_properties);
		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if ((filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
	}

	void Device::FindBestDevice(const Instance& instance, const Surface& surface)
	{
		//获取physical device
		uint32_t device_count;
		vkEnumeratePhysicalDevices(instance.GetInstance(), &device_count, nullptr);
		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(instance.GetInstance(), &device_count, devices.data());

		std::multimap<int, std::pair<DeviceInfo, VkPhysicalDevice>> score_devices_map;
		for (const VkPhysicalDevice& device : devices)
		{
			DeviceInfo info;
			int score = CheckDevice(device, info, surface.GetSurface());
			if (score != 0)
				score_devices_map.insert(std::make_pair(score, std::make_pair(info, device)));
		}

		for (auto it = score_devices_map.rbegin(); it != score_devices_map.rend(); it++)
		{
			if (it->first > 0)
			{
				_physical_device = it->second.second;
				device_info = it->second.first;
				return;
			}
		}

		throw std::runtime_error("failed to find physical device!");
	}

	void Device::CreateDevice(const VkPhysicalDevice& physical_device)
	{
		_physical_device = physical_device;

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		//使用set的话，元素内容不会重复
		std::set<int> queue_indices = { device_info.graphic_queue_index, device_info.present_queue_index };

		//逻辑设备需要开的队列簇
		float queue_priority = 1.0f;		//队列优先级
		for (const auto& index : queue_indices)
		{
			VkDeviceQueueCreateInfo queue_create_info = {};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = index;
			queue_create_info.queueCount = 1;	//暂时只使用带图像计算的队列
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}

		//逻辑设备需要的特性
		VkPhysicalDeviceFeatures features = {};
		features.samplerAnisotropy = VK_TRUE;
		features.sampleRateShading = VK_TRUE;

		std::vector<const char*> validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

		std::vector<const char*> extensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		//创建逻辑设备
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pEnabledFeatures = &features;
		//启用交换链拓展
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
		//同样开启验证层
		create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();

		VkResult result = vkCreateDevice(_physical_device, &create_info, nullptr,
			&_logical_device);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device");
		}

		//获取队列句柄
		vkGetDeviceQueue(_logical_device, device_info.graphic_queue_index, 0, &_graphic_queue);
		vkGetDeviceQueue(_logical_device, device_info.present_queue_index, 0, &_present_queue);
	}

	int Device::CheckDevice(VkPhysicalDevice physical_device, DeviceInfo& device_info, const VkSurfaceKHR& surface)
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
				if (device_info.graphic_queue_index == -1 && properties[i].queueCount > 0
					&& properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					device_info.graphic_queue_index = i;		//记录下具有Graphic功能的队列簇
				}

				if (device_info.present_queue_index == -1)
				{
					VkBool32 is_present_supported = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &is_present_supported);
					if (is_present_supported)
						device_info.present_queue_index = i;
				}

				if (device_info.graphic_queue_index != -1 && device_info.present_queue_index != -1)
					break;
			}

			if (device_info.graphic_queue_index == -1 || device_info.present_queue_index == -1)	//没有这两个功能
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
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &device_info.capabilities);

			uint32_t format_count;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
			device_info.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, device_info.formats.data());

			uint32_t present_mode_count;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
			device_info.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, device_info.present_modes.data());

			if (device_info.formats.empty() || device_info.present_modes.empty())
				return 0;
		}
		return score;
	}
}