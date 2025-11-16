#include "PCH.h"
#include "Buffer.h"
#include <stdexcept>
namespace srx
{
	std::shared_ptr<Buffer> Buffer::Create(const Device& device
		, BufferUsage usage, BufferProperty properties, BufferSize size)
	{
		std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>();
		buffer->Init(device, usage, properties, size);
		return buffer;
	}

	void Buffer::Destroy(const Device& device)
	{
		vkDestroyBuffer(device.GetLogicalDevice(), _buffer, nullptr);
		vkFreeMemory(device.GetLogicalDevice(), _memory, nullptr);
	}

	void Buffer::InputData(const Device& device, const BufferSize& size, const void* data)
	{
		void* mmap_data;
		vkMapMemory(device.GetLogicalDevice(), _memory, 0, size, 0, &mmap_data);		//memory的内存映射到CPU上某一块
		memcpy(mmap_data, data, size);		//把要传的数据传入到mmap_data中，从而映射到memory中
		vkUnmapMemory(device.GetLogicalDevice(), _memory);
	}

	void Buffer::InputData(const Device& device, const BufferSize& size, const Buffer& buffer, const CommandPool& pool)
	{
		auto command_buffer = CommandBuffer::Create();
		command_buffer->BeginCommand(device, pool);
		
		VkBufferCopy copy_region = {};
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;
		vkCmdCopyBuffer(command_buffer->GetBuffer(), buffer.GetBuffer(), _buffer, 1, &copy_region);

		command_buffer->EndCommand(device, pool);
	}

	void Buffer::Init(const Device& device
		, BufferUsage usage, BufferProperty properties, BufferSize size)
	{
		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = size;
		buffer_create_info.usage = usage;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device.GetLogicalDevice(), &buffer_create_info, nullptr, &_buffer) != VK_SUCCESS)
			throw std::runtime_error("failed to create buffer!");

		//内存
		VkMemoryRequirements mem_requirment;
		vkGetBufferMemoryRequirements(device.GetLogicalDevice(), _buffer, &mem_requirment);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirment.size;
		alloc_info.memoryTypeIndex = device.FindMemoryType(mem_requirment.memoryTypeBits, properties);

		if (vkAllocateMemory(device.GetLogicalDevice(), &alloc_info, nullptr, &_memory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate memory!");

		vkBindBufferMemory(device.GetLogicalDevice(), _buffer, _memory, 0);
	}
}