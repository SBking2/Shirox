#include "PCH.h"
#include "Image.h"
#include <stdexcept>
namespace srx
{
    std::shared_ptr<Image> Image::Create(const Device& device, uint32_t width, uint32_t height
		, VkFormat format, VkImageTiling tiling
		, ImageUsage usage, ImageProperty properties)
    {
        auto img = std::make_shared<Image>();
		img->Init(device, width, height, format, tiling, usage, properties);
        return img;
    }

    void Image::Destroy(const Device& device)
    {
		vkDestroyImage(device.GetLogicalDevice(), _img, nullptr);
		vkFreeMemory(device.GetLogicalDevice(), _memory, nullptr);
    }

	void Image::InputData(const Device& device, uint32_t width, uint32_t height, const Buffer& buffer, const CommandPool& pool)
	{
		auto com_buffer = CommandBuffer::Create();
		com_buffer->BeginCommand(device, pool);

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(com_buffer->GetBuffer(), buffer.GetBuffer(), _img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		com_buffer->EndCommand(device, pool);
	}

    void Image::Init(const Device& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling
		, ImageUsage usage, ImageProperty properties)
    {
		//开始创建VkImage
		VkImageCreateInfo img_create_info = {};
		img_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		img_create_info.imageType = VK_IMAGE_TYPE_2D;
		img_create_info.extent.width = width;
		img_create_info.extent.height = height;
		img_create_info.extent.depth = 1;
		img_create_info.mipLevels = 1;
		img_create_info.arrayLayers = 1;
		img_create_info.format = format;
		img_create_info.tiling = tiling;	//纹素的排列方式
		img_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	//第一次变换后就丢弃？
		img_create_info.usage = usage;
		img_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		img_create_info.samples = VK_SAMPLE_COUNT_1_BIT;

		if (vkCreateImage(device.GetLogicalDevice(), &img_create_info, nullptr, &_img) != VK_SUCCESS)
			throw std::runtime_error("failed to create image!");

		VkMemoryRequirements mem_requirement;
		vkGetImageMemoryRequirements(device.GetLogicalDevice(), _img, &mem_requirement);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirement.size;
		alloc_info.memoryTypeIndex = device.FindMemoryType(mem_requirement.memoryTypeBits, properties);

		if (vkAllocateMemory(device.GetLogicalDevice(), &alloc_info, nullptr, &_memory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate img memory!");

		vkBindImageMemory(device.GetLogicalDevice(), _img, _memory, 0);
    }

}