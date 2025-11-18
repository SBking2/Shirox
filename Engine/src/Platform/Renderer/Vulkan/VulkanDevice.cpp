#include "PCH.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
namespace srx
{
	///////////////////////////////////////////////////////////////////////////////
	////						Physical Device								   ////
	///////////////////////////////////////////////////////////////////////////////
	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		VulkanContext* context = (VulkanContext*)RendererContext::GetContext().get();

		///////////////////////////////////////////////////	显卡查询
		uint32_t device_count;
		vkEnumeratePhysicalDevices(context->GetVkInstance(), &device_count, nullptr);
		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(context->GetVkInstance(), &device_count, devices.data());

		SRX_ASSERT(devices.size() > 0, "获取不到物理设备!");

		VkPhysicalDevice selected_device = nullptr;
		for (auto device : devices)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)		//如果当前显卡是独显
			{
				selected_device = device;
				break;
			}
		}

		if (selected_device == nullptr)
		{
			SRX_LOG_INFO("找不到独显！");
			selected_device = devices.front();
		}

		_PhysicalDevice = selected_device;

		///////////////////////////////////////////////////	查找队列簇
		uint32_t queue_family_count;
		vkGetPhysicalDeviceQueueFamilyProperties(_PhysicalDevice, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_familys(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(_PhysicalDevice, &queue_family_count, queue_familys.data());
		uint32_t required_queue = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT;
		_QueueFamilyIndices = GetQueueFamilyIndices(queue_familys, required_queue);

		///////////////////////////////////////////////////	查找支持的Extension
		uint32_t supported_extension_count;
		vkEnumerateDeviceExtensionProperties(_PhysicalDevice, nullptr, &supported_extension_count, nullptr);
		std::vector<VkExtensionProperties> extension_proerties(supported_extension_count);
		vkEnumerateDeviceExtensionProperties(_PhysicalDevice, nullptr, &supported_extension_count, extension_proerties.data());
		
		_ExtensionPropertires.clear();
		for (int i = 0; i < supported_extension_count; i++)
			_ExtensionPropertires.emplace(extension_proerties[i].extensionName);

		///////////////////////////////////////////////////	查找支持的Layer
		uint32_t supported_layer_count;
		vkEnumerateDeviceLayerProperties(_PhysicalDevice, &supported_layer_count, nullptr);
		std::vector<VkLayerProperties> layers_properties(supported_layer_count);
		vkEnumerateDeviceLayerProperties(_PhysicalDevice, &supported_layer_count, layers_properties.data());

		_LayerProerties.clear();
		for (int i = 0; i < supported_layer_count; i++)
			_LayerProerties.emplace(layers_properties[i].layerName);
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{

	}

	bool VulkanPhysicalDevice::IsSupportedExtension(const std::string& extension_name)
	{
		if (_ExtensionPropertires.find(extension_name) != _ExtensionPropertires.end())
			return true;
		return false;
	}

	bool VulkanPhysicalDevice::IsSupportedLayer(const std::string& layer_name)
	{
		if (_LayerProerties.find(layer_name) != _LayerProerties.end())
			return true;
		return false;
	}

	VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices(const std::vector<VkQueueFamilyProperties>& queue_family, uint32_t require_queue)
	{
		QueueFamilyIndices indices;

		// Dedicated queue for compute
		// Try to find a queue family index that supports compute but not graphics
		if (require_queue & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < queue_family.size(); i++)
			{
				auto& queueFamilyProperties = queue_family[i];
				if ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					indices.compute = i;
					break;
				}
			}
		}

		// Dedicated queue for transfer
		// Try to find a queue family index that supports transfer but not graphics and compute
		if (require_queue & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < queue_family.size(); i++)
			{
				auto& queueFamilyProperties = queue_family[i];
				if ((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					indices.transfer = i;
					break;
				}
			}
		}

		// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
		for (uint32_t i = 0; i < queue_family.size(); i++)
		{
			if ((require_queue & VK_QUEUE_TRANSFER_BIT) && indices.transfer == -1)
			{
				if (queue_family[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
					indices.transfer = i;
			}

			if ((require_queue & VK_QUEUE_COMPUTE_BIT) && indices.compute == -1)
			{
				if (queue_family[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
					indices.compute = i;
			}

			if (require_queue & VK_QUEUE_GRAPHICS_BIT)
			{
				if (queue_family[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.graphic = i;
			}
		}

		SRX_ASSERT((indices.compute != -1 && indices.graphic != -1 && indices.transfer != -1), "找不到适合的队列簇!");

		return indices;
	}

	///////////////////////////////////////////////////////////////////////////////
	////								 Device								   ////
	///////////////////////////////////////////////////////////////////////////////

	VulkanDevice::VulkanDevice(Ref<VulkanPhysicalDevice> physical_device
		, VkPhysicalDeviceFeatures enable_features) : _EnableFeatures(enable_features)
	{
		///////////////////////////////////////////////////	配置Queue Create Info
		auto queue_family_index = physical_device->GetQueueFamilyIndices();
		std::set<uint32_t> queue_indices =
		{
			queue_family_index.compute
			, queue_family_index.graphic
			, queue_family_index.transfer
		};
		float queue_priority = 0.0f;		//队列优先级
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_indices.size());
		{
			int i = 0;
			for (auto index : queue_indices)
			{
				queue_create_infos[i] = {};
				queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queue_create_infos[i].queueFamilyIndex = index;
				queue_create_infos[i].queueCount = 1;	//暂时只使用带图像计算的队列
				queue_create_infos[i].pQueuePriorities = &queue_priority;
				i++;
			}
		}
		///////////////////////////////////////////////////	配置需要的Layer
		std::vector<const char*> required_layers;
		std::string validation_layer_name = "VK_LAYER_KHRONOS_validation";
		SRX_ASSERT(physical_device->IsSupportedLayer(validation_layer_name), "当前显卡不支持验证层!");
		required_layers.push_back(validation_layer_name.c_str());

		///////////////////////////////////////////////////	配置需要的Extension
		std::vector<const char*> required_extensions;
		SRX_ASSERT(physical_device->IsSupportedExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "当前显卡不支持Swapchian拓展!");
		required_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		//创建逻辑设备
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pEnabledFeatures = &_EnableFeatures;
		//启用拓展
		create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
		create_info.ppEnabledExtensionNames = required_extensions.data();
		//开启层
		create_info.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
		create_info.ppEnabledLayerNames = required_layers.data();

		VkResult result = vkCreateDevice(physical_device->GetVkPhysicalDevice(), &create_info, nullptr, &_Device);

		SRX_ASSERT(result == VK_SUCCESS, "创建逻辑设备失败!");

		vkGetDeviceQueue(_Device, queue_family_index.graphic, 0, &_GraphicQueue);
		vkGetDeviceQueue(_Device, queue_family_index.compute, 0, &_ComputeQueue);
		vkGetDeviceQueue(_Device, queue_family_index.transfer, 0, &_TransferQueue);
	}

	VulkanDevice::~VulkanDevice()
	{

	}

	void VulkanDevice::Destroy()
	{
		vkDestroyDevice(_Device, nullptr);
	}
}