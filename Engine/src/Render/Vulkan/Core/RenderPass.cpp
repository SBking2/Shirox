#include "RenderPass.h"
#include "Core/Utils/Utils.h"
#include <array>
#include <stdexcept>
namespace ev
{
	void RenderPass::Init(const Device& device, const Swapchain& swapchain)
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = swapchain.swapchain_info.surface_format.format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;	//每次渲染前一帧清楚帧缓冲
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;	//渲染的内容会被存储起来

		//对模板缓冲不关心（暂时）
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		//表示不关心渲染前的图像布局
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;		//图像被用作在交换链中呈现

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;	//attachment description的索引
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depth_attachment = {};
		depth_attachment.format = Utils::FindSupportedFormat(device
			, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }
			, VK_IMAGE_TILING_OPTIMAL
			, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_attachment_ref = {};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;

		std::array<VkAttachmentDescription, 2> attachments = {
			color_attachment, depth_attachment
		};

		//配置子流程依赖
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	//等待颜色附着输出的阶段
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderpass_create_info = {};
		renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderpass_create_info.pAttachments = attachments.data();
		renderpass_create_info.subpassCount = 1;
		renderpass_create_info.pSubpasses = &subpass;
		renderpass_create_info.dependencyCount = 1;
		renderpass_create_info.pDependencies = &dependency;

		if (vkCreateRenderPass(device.GetLogicalDevice(), &renderpass_create_info, nullptr, &_render_pass) != VK_SUCCESS)
			throw std::runtime_error("failed to create render pass!");
	}
	void RenderPass::Destroy(const Device& device)
	{
		vkDestroyRenderPass(device.GetLogicalDevice(), _render_pass, nullptr);
	}
}