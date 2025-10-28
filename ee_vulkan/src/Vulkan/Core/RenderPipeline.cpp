#include "RenderPipeline.h"
#include "../RenderObject/ShaderModule.h"
#include "Renderer/Mesh/SkinnedMesh.h"
#include <stdexcept>
#include <array>
namespace ev
{
	void RenderPipeline::Init(const Device& device, const RenderPass& render_pass, const Swapchain& swapchain, VkDescriptorSetLayout& descriptor_layout)
	{
		std::shared_ptr<ShaderModule> vertex_shader = ShaderModule::Create(device, "assets/shaders/vert.spv");
		std::shared_ptr<ShaderModule> fragment_shader = ShaderModule::Create(device, "assets/shaders/frag.spv");

		//指定shader在哪个着色器阶段使用
		VkPipelineShaderStageCreateInfo vert_shader_stage_create_info = {};
		vert_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_shader_stage_create_info.module = vertex_shader->GetShader();
		vert_shader_stage_create_info.pName = "main";

		VkPipelineShaderStageCreateInfo frag_shader_stage_create_info = {};
		frag_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_shader_stage_create_info.module = fragment_shader->GetShader();
		frag_shader_stage_create_info.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] =
		{
			vert_shader_stage_create_info,
			frag_shader_stage_create_info
		};

		//顶点绑定描述
		VkVertexInputBindingDescription bind_descriptions = {};
		bind_descriptions.binding = 0;
		bind_descriptions.stride = sizeof(SkinnedMesh::Vertex);
		bind_descriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		//顶点属性描述
		std::array<VkVertexInputAttributeDescription, 5> attribute_descriptions{};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = 0;

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(SkinnedMesh::Vertex, color);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[2].offset = offsetof(SkinnedMesh::Vertex, tex_coord);

		attribute_descriptions[3].binding = 0;
		attribute_descriptions[3].location = 3;
		attribute_descriptions[3].format = VK_FORMAT_R32G32B32A32_SINT;
		attribute_descriptions[3].offset = offsetof(SkinnedMesh::Vertex, bone_ids);

		attribute_descriptions[4].binding = 0;
		attribute_descriptions[4].location = 4;
		attribute_descriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute_descriptions[4].offset = offsetof(SkinnedMesh::Vertex, weights);

		//1.顶点输入
		//指定传给顶点着色器地顶点数据的格式
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
		vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
		vertex_input_state_create_info.pVertexBindingDescriptions = &bind_descriptions;
		vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions.data();

		//2.输入装配
		//定义了哪几种类型的图元
		//是否启用几何图元重启
		VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
		assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assembly_create_info.primitiveRestartEnable = VK_FALSE;

		//3.视口和裁剪

		//设置视口
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchain.swapchain_info.extent.width;
		viewport.height = (float)swapchain.swapchain_info.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		//设置裁剪
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain.swapchain_info.extent;

		VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
		viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state_create_info.viewportCount = 1;
		viewport_state_create_info.pViewports = &viewport;
		viewport_state_create_info.scissorCount = 1;
		viewport_state_create_info.pScissors = &scissor;

		//4.光栅化
		VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
		rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_create_info.depthClampEnable = VK_FALSE;		//表明丢弃远近平面外的东西，并不截断为远近平面
		rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;		//如果是True，则禁止一切片段输出到帧缓冲
		rasterization_create_info.lineWidth = 1.0f;		//如果是True，则禁止一切片段输出到帧缓冲
		rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;		//背面剔除
		rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;		//指定顺时针的顶点顺序为正面

		rasterization_create_info.depthBiasEnable = VK_FALSE;	//是否将片段所处线段的斜率？放到深度值上？
		rasterization_create_info.depthBiasConstantFactor = 0.0f;
		rasterization_create_info.depthBiasClamp = 0.0f;
		rasterization_create_info.depthBiasSlopeFactor = 0.0f;

		//5.多重采样
		VkPipelineMultisampleStateCreateInfo multi_sample_create_info = {};
		multi_sample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multi_sample_create_info.sampleShadingEnable = VK_TRUE;	//禁用多重采样
		multi_sample_create_info.minSampleShading = 0.2f;
		multi_sample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	//采样一次？

		//6.深度和模板测试
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		//7.颜色混合
		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_FALSE;		//暂时禁用颜色混合

		VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
		color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_create_info.logicOpEnable = VK_FALSE;
		color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
		color_blend_create_info.attachmentCount = 1;
		color_blend_create_info.pAttachments = &color_blend_attachment;

		//8.动态状态
		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,		//视口变换
			VK_DYNAMIC_STATE_LINE_WIDTH		//线宽
		};

		VkPipelineDynamicStateCreateInfo dynamic_create_info = {};
		dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_create_info.dynamicStateCount = 0;	//sizeof(dynamicStates) / sizeof(VkDynamicState);	//暂时不开启
		dynamic_create_info.pDynamicStates = dynamicStates;

		//9.管线布局(uniform)
		VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
		pipeline_layout_create_info.sType =
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pSetLayouts = &descriptor_layout;
		pipeline_layout_create_info.pushConstantRangeCount = 0; // Optional
		pipeline_layout_create_info.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(device.GetLogicalDevice(), &pipeline_layout_create_info, nullptr,
			&_pipeline_layout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout!");
		//创建渲染管线
		VkGraphicsPipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_info.stageCount = 2;	//两个着色器阶段
		pipeline_create_info.pStages = shaderStages;

		pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
		pipeline_create_info.pInputAssemblyState = &assembly_create_info;
		pipeline_create_info.pViewportState = &viewport_state_create_info;
		pipeline_create_info.pRasterizationState = &rasterization_create_info;
		pipeline_create_info.pMultisampleState = &multi_sample_create_info;
		pipeline_create_info.pDepthStencilState = &depthStencil;
		pipeline_create_info.pColorBlendState = &color_blend_create_info;
		pipeline_create_info.pDynamicState = &dynamic_create_info;

		pipeline_create_info.layout = _pipeline_layout;
		pipeline_create_info.renderPass = render_pass.GetRenderPass();
		pipeline_create_info.subpass = 0;	//子流程的索引

		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipeline_create_info.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(device.GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info
			, nullptr, &_pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphic pipeline!");
		}

		//创建了管线之后可以销毁shader module了
		vertex_shader->Destroy(device);
		fragment_shader->Destroy(device);
	}

	void RenderPipeline::Destroy(const Device& device)
	{
		vkDestroyPipelineLayout(device.GetLogicalDevice(), _pipeline_layout, nullptr);
		vkDestroyPipeline(device.GetLogicalDevice(), _pipeline, nullptr);
	}

}