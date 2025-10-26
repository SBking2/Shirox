#include "Device.h"
#include<set>
#include<stdexcept>
namespace ev
{
	void Device::Init()
	{
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
	void Device::Destroy()
	{
		vkDestroyDevice(_logical_device, nullptr);
	}
}