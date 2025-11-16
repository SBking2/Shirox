#include "PCH.h"
#include "CommandBuffer.h"

namespace srx
{
    std::shared_ptr<CommandBuffer> CommandBuffer::Create()
    {
        std::shared_ptr<CommandBuffer> buffer = std::make_shared<CommandBuffer>();
        return buffer;
    }

    void CommandBuffer::BeginCommand(const Device& device, const CommandPool& pool)
    {
        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = pool.GetCommandPool();
        alloc_info.commandBufferCount = 1;

        vkAllocateCommandBuffers(device.GetLogicalDevice(), &alloc_info, &_command_buffer);

        //开始记录指令
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(_command_buffer, &begin_info);
    }

    void CommandBuffer::EndCommand(const Device& device, const CommandPool& pool)
    {
        vkEndCommandBuffer(_command_buffer);

        //提交指令
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &_command_buffer;

        vkQueueSubmit(device.GetGraphicQueue(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(device.GetGraphicQueue());

        vkFreeCommandBuffers(device.GetLogicalDevice(), pool.GetCommandPool(), 1, &_command_buffer);
    }
}