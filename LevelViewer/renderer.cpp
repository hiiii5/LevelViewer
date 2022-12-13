#include "Globals.h"

void Renderer::UpdateStorageBuffers(VkPhysicalDevice physicalDevice, unsigned& maxFrames, bool create) {
	vlk.GetSwapchainImageCount(maxFrames);

	if(StorageHandle.size() < maxFrames) {
		StorageHandle.resize(maxFrames);
	}
	if(StorageData.size() < maxFrames) {
		StorageData.resize(maxFrames);
	}

	// For every frame, make a matching buffer to hold initial data for the shaders.
	for (unsigned int i = 0; i < maxFrames; i++) {
		if(create) {
			GvkHelper::create_buffer(physicalDevice, Device, sizeof(SHADER_MODEL_DATA),
								 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StorageHandle[i], &StorageData[i]);
		}
		GvkHelper::write_to_buffer(Device, StorageData[i], &InstanceData, sizeof(SHADER_MODEL_DATA));
	}
}

void Renderer::Init(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GVulkanSurface _vlk) {
	win = _win;
	vlk = _vlk;
	unsigned int width, height;
	win.GetClientWidth(width);
	win.GetClientHeight(height);

	Input.Create(win);
	Controller.Create();

	InstanceData.Transforms[0] = GW::MATH::GIdentityMatrixF;

	InstanceData.ViewMatrix = GW::MATH::GIdentityMatrixF;
	InstanceData.EyePos = { 0.75f, 0.25f, -3.5f };
	GW::MATH::GVECTORF up = { 0.0f, 1.0f, 0.0f };

	GW::MATH::GMatrix::TranslateGlobalF(InstanceData.ViewMatrix, InstanceData.EyePos, InstanceData.ViewMatrix);

	GW::MATH::GVECTORF lookAtPos{ 0.0f, 0.0f, 0.0f };

	GW::MATH::GMatrix::LookAtLHF(InstanceData.EyePos, lookAtPos, up, InstanceData.ViewMatrix);

	float aspectRatio;
	vlk.GetAspectRatio(aspectRatio);

	InstanceData.ProjectionMatrix = GetDefaultProjectionMatrix();
	InstanceData.SunColor = {0.9f, 0.9f, 1.0f, 1.0f};
	InstanceData.SunDirection = {-1, -1, 2};
	GW::MATH::GVector::NormalizeF(InstanceData.SunDirection, InstanceData.SunDirection);

	/***************** GEOMETRY INTIALIZATION ******************/
	// Grab the device & physical device so we can allocate some stuff
	vlk.GetPhysicalDevice((void**)&PhysicalDevice);
	vlk.GetDevice((void**)&Device);

	CurrentLevel = Level();
	CurrentLevel.Load("../GameLevel.txt");

	unsigned maxFrames;
	UpdateStorageBuffers(PhysicalDevice, maxFrames, true);

	/***************** SHADER INTIALIZATION ******************/
	auto S = Shader{ "../vs.hlsl", "../ps.hlsl" };
	CompileShader(S, Device);
	Shaders.insert({"DefaultShader", S});

	/***************** PIPELINE INTIALIZATION ******************/
	InitPipeline(width, height, maxFrames);

	/***************** CLEANUP / SHUTDOWN ******************/
	// GVulkanSurface will inform us when to release any allocated resources
	shutdown.Create(vlk, [&]() {
		if (+shutdown.Find(GW::GRAPHICS::GVulkanSurface::Events::RELEASE_RESOURCES, true)) {
			CleanUp(); // unlike D3D we must be careful about destroy timing
		}
	});
}

void Renderer::InitPipeline(unsigned width, unsigned height, unsigned maxFrames) {
	// Create Pipeline & Layout (Thanks Tiny!)
	VkRenderPass renderPass;
	vlk.GetRenderPass((void**)&renderPass);
	VkPipelineShaderStageCreateInfo stage_create_info[2] = {};

	// Create Stage Info for Vertex Shader
	stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stage_create_info[0].module = Shaders["DefaultShader"].VertShader;
	stage_create_info[0].pName = "main";

	// Create Stage Info for Fragment Shader
	stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stage_create_info[1].module = Shaders["DefaultShader"].PixShader;
	stage_create_info[1].pName = "main";

	// Assembly State
	VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
	assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assembly_create_info.primitiveRestartEnable = false;

	// Vertex Input State
	VkVertexInputBindingDescription vertex_binding_description = {};
	vertex_binding_description.binding = 0;
	vertex_binding_description.stride = sizeof(H2B::VERTEX);
	vertex_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputAttributeDescription vertex_attribute_description[3] = {
		{0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0}, //uv, normal, etc....
		{1, 0, VK_FORMAT_R32G32B32_SFLOAT, 12},
		{2, 0, VK_FORMAT_R32G32B32_SFLOAT, 24}
	};

	VkPipelineVertexInputStateCreateInfo input_vertex_info = {};
	input_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_vertex_info.vertexBindingDescriptionCount = 1;
	input_vertex_info.pVertexBindingDescriptions = &vertex_binding_description;
	input_vertex_info.vertexAttributeDescriptionCount = 3;
	input_vertex_info.pVertexAttributeDescriptions = vertex_attribute_description;

	// Viewport State (we still need to set this up even though we will overwrite the values)
	VkViewport viewport = {
		0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1
	};

	VkRect2D scissor = { {0, 0}, {width, height} };
	VkPipelineViewportStateCreateInfo viewport_create_info = {};
	viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_create_info.viewportCount = 1;
	viewport_create_info.pViewports = &viewport;
	viewport_create_info.scissorCount = 1;
	viewport_create_info.pScissors = &scissor;

	// Rasterizer State
	VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
	rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_create_info.lineWidth = 1.0f;
	rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterization_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_create_info.depthClampEnable = VK_FALSE;
	rasterization_create_info.depthBiasEnable = VK_FALSE;
	rasterization_create_info.depthBiasClamp = 0.0f;
	rasterization_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_create_info.depthBiasSlopeFactor = 0.0f;

	// Multisampling State
	VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
	multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_create_info.sampleShadingEnable = VK_FALSE;
	multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_create_info.minSampleShading = 1.0f;
	multisample_create_info.pSampleMask = VK_NULL_HANDLE;
	multisample_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_create_info.alphaToOneEnable = VK_FALSE;

	// Depth-Stencil State
	VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
	depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_create_info.depthTestEnable = VK_TRUE;
	depth_stencil_create_info.depthWriteEnable = VK_TRUE;
	depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_create_info.minDepthBounds = 0.0f;
	depth_stencil_create_info.maxDepthBounds = 1.0f;
	depth_stencil_create_info.stencilTestEnable = VK_FALSE;

	// Color Blending Attachment & State
	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
	color_blend_attachment_state.colorWriteMask = 0xF;
	color_blend_attachment_state.blendEnable = VK_FALSE;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
	color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
	color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
	color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_create_info.logicOpEnable = VK_FALSE;
	color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
	color_blend_create_info.attachmentCount = 1;
	color_blend_create_info.pAttachments = &color_blend_attachment_state;
	color_blend_create_info.blendConstants[0] = 0.0f;
	color_blend_create_info.blendConstants[1] = 0.0f;
	color_blend_create_info.blendConstants[2] = 0.0f;
	color_blend_create_info.blendConstants[3] = 0.0f;

	// Dynamic State 
	VkDynamicState dynamic_state[2] = {
		// By setting these we do not need to re-create the pipeline on Resize
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamic_create_info = {};
	dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_create_info.dynamicStateCount = 2;
	dynamic_create_info.pDynamicStates = dynamic_state;

	VkDescriptorSetLayoutBinding descriptorLayoutBinding{};
	descriptorLayoutBinding.binding = 0;
	descriptorLayoutBinding.descriptorCount = 1;
	descriptorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
	descriptorCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorCreateInfo.flags = 0;
	descriptorCreateInfo.bindingCount = 1;
	descriptorCreateInfo.pBindings = &descriptorLayoutBinding;
	descriptorCreateInfo.pNext = nullptr;
	vkCreateDescriptorSetLayout(Device, &descriptorCreateInfo, nullptr, &DescriptorLayout);

	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	VkDescriptorPoolSize poolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxFrames };
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = &poolSize;
	poolCreateInfo.maxSets = maxFrames;
	poolCreateInfo.flags = 0;
	poolCreateInfo.pNext = nullptr;
	vkCreateDescriptorPool(Device, &poolCreateInfo, nullptr, &DescriptorPool);

	VkDescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = &DescriptorLayout;
	allocateInfo.descriptorPool = DescriptorPool;
	allocateInfo.pNext = nullptr;
	DescriptorSet.resize(maxFrames);
	for (unsigned int i = 0; i < maxFrames; i++) {
		vkAllocateDescriptorSets(Device, &allocateInfo, &DescriptorSet[i]);
	}

	VkWriteDescriptorSet shaderWriteDescriptorSet{};
	shaderWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	shaderWriteDescriptorSet.descriptorCount = 1;
	shaderWriteDescriptorSet.dstArrayElement = 0;
	shaderWriteDescriptorSet.dstBinding = 0;
	shaderWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	for (unsigned int i = 0; i < maxFrames; i++) {
		shaderWriteDescriptorSet.dstSet = DescriptorSet[i];
		VkDescriptorBufferInfo dbinfo = { StorageHandle[i], 0, VK_WHOLE_SIZE };
		shaderWriteDescriptorSet.pBufferInfo = &dbinfo;
		vkUpdateDescriptorSets(Device, 1, &shaderWriteDescriptorSet, 0, nullptr);
	}

	// Descriptor pipeline layout
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &DescriptorLayout;

	VkPushConstantRange data = { VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(unsigned int) };
	pipeline_layout_create_info.pushConstantRangeCount = 1;
	pipeline_layout_create_info.pPushConstantRanges = &data;
	vkCreatePipelineLayout(Device, &pipeline_layout_create_info,
		nullptr, &PipelineLayout);

	// Pipeline State... (FINALLY) 
	VkGraphicsPipelineCreateInfo pipeline_create_info = {};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.stageCount = 2;
	pipeline_create_info.pStages = stage_create_info;
	pipeline_create_info.pInputAssemblyState = &assembly_create_info;
	pipeline_create_info.pVertexInputState = &input_vertex_info;
	pipeline_create_info.pViewportState = &viewport_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_create_info;
	pipeline_create_info.pMultisampleState = &multisample_create_info;
	pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
	pipeline_create_info.pColorBlendState = &color_blend_create_info;
	pipeline_create_info.pDynamicState = &dynamic_create_info;
	pipeline_create_info.layout = PipelineLayout;
	pipeline_create_info.renderPass = renderPass;
	pipeline_create_info.subpass = 0;
	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1,
		&pipeline_create_info, nullptr, &Pipeline);
}

void Renderer::CompileMesh(Mesh* Target) {
	// Transfer triangle data to the vertex buffer. (staging would be prefered here)
	GvkHelper::create_buffer(PhysicalDevice, Device, sizeof(H2B::VERTEX) * Target->VertexCount,
							 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Target->VertexHandle, &Target->VertexData);
	GvkHelper::write_to_buffer(Device, Target->VertexData, &Target->Vertices[0], sizeof(H2B::VERTEX) * Target->VertexCount);

	GvkHelper::create_buffer(PhysicalDevice, Device, sizeof(unsigned) * Target->IndexCount,
							 VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Target->IndexHandle, &Target->IndexData);
	GvkHelper::write_to_buffer(Device, Target->IndexData, &Target->Indices[0], sizeof(unsigned) * Target->IndexCount);
}

void Renderer::CompileShader(Shader& ShaderRef, const VkDevice& Device) {
	// Intialize runtime shader compiler HLSL -> SPIRV
	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	shaderc_compile_options_t options = shaderc_compile_options_initialize();
	shaderc_compile_options_set_source_language(options, shaderc_source_language_hlsl);
	shaderc_compile_options_set_invert_y(options, false); // TODO: Part 2i
#ifndef NDEBUG
	shaderc_compile_options_set_generate_debug_info(options);
#endif
	// Create Vertex Shader
	std::string vsShader = ShaderParser::ShaderAsString(ShaderRef.VsFile.c_str());
	shaderc_compilation_result_t result = shaderc_compile_into_spv( // compile
		compiler, vsShader.c_str(), strlen(vsShader.c_str()),
		shaderc_vertex_shader, "main.vert", "main", options);
	if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
		std::cout << "Vertex Shader Errors: " << shaderc_result_get_error_message(result) << std::endl;
	GvkHelper::create_shader_module(Device, shaderc_result_get_length(result), // load into Vulkan
									(char*)shaderc_result_get_bytes(result), &ShaderRef.VertShader);
	shaderc_result_release(result); // done
	// Create Pixel Shader
	std::string psShader = ShaderParser::ShaderAsString(ShaderRef.PsFile.c_str());
	result = shaderc_compile_into_spv( // compile
		compiler, psShader.c_str(), strlen(psShader.c_str()),
		shaderc_fragment_shader, "main.frag", "main", options);
	if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
		std::cout << "Pixel Shader Errors: " << shaderc_result_get_error_message(result) << std::endl;
	GvkHelper::create_shader_module(Device, shaderc_result_get_length(result), // load into Vulkan
									(char*)shaderc_result_get_bytes(result), &ShaderRef.PixShader);
	shaderc_result_release(result); // done
	// Free runtime shader compiler resources
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
}

void Renderer::RenderActor(const Actor* ActorToRender, const SHADER_MODEL_DATA& CurrentData) {
	// now we can draw
	VkDeviceSize offsets[] = { 0 };
	VkCommandBuffer CommandBuffer;
	unsigned int currentBuffer;
	GetCurrentBuffers(currentBuffer, CommandBuffer);
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &ActorToRender->MeshComp->VertexHandle, offsets);
	vkCmdBindIndexBuffer(CommandBuffer, ActorToRender->MeshComp->IndexHandle, 0, VK_INDEX_TYPE_UINT32);

	for (unsigned int i = 0; i < ActorToRender->MeshComp->MeshCount; ++i) {
		const auto indexCount = ActorToRender->MeshComp->Batches[i].indexCount;
		const auto offset = ActorToRender->MeshComp->Batches[i].indexOffset;
		auto matId = ActorToRender->MeshComp->Meshes[i].materialIndex;
		GvkHelper::write_to_buffer(Device, StorageData[i], &CurrentData, sizeof(SHADER_MODEL_DATA));

		vkCmdPushConstants(CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(matId), &matId);

		vkCmdDrawIndexed(CommandBuffer, indexCount, 1, offset, 0, 0);
	}
}

void Renderer::GetCurrentBuffers(unsigned& CurrentBuffer, VkCommandBuffer& CommandBuffer) const {
	vlk.GetSwapchainCurrentImage(CurrentBuffer);
	vlk.GetCommandBuffer(CurrentBuffer, reinterpret_cast<void**>(&CommandBuffer));
}

SHADER_MODEL_DATA Renderer::GetWorldShaderData() const {
	return InstanceData;
}

void Renderer::Render() {
	Start = std::chrono::high_resolution_clock::now();

	unsigned currentBuffer;
	VkCommandBuffer commandBuffer;
	GetCurrentBuffers(currentBuffer, commandBuffer);

	// what is the current client area dimensions?
	unsigned int width, height;
	win.GetClientWidth(width);
	win.GetClientHeight(height);

	// setup the pipeline'S dynamic settings
	VkViewport viewport = {
		0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1
	};
	VkRect2D scissor = {{0, 0}, {width, height}};

	// Cut the viewport rect for viewing.
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// Bind to the current pipeline.
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);

	// Bind to that pipelines information.
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout,
							0, 1, &DescriptorSet[currentBuffer], 0, nullptr);

	// Render the current level.
	CurrentLevel.Render(InstanceData);

	Stop = std::chrono::high_resolution_clock::now();

	using ms = std::chrono::duration<float, std::milli>;
	DeltaTime = std::chrono::duration_cast<ms>(Stop - Start).count();
}

void Renderer::Update() {
	unsigned maxFrames;
	UpdateStorageBuffers(PhysicalDevice, maxFrames, false);
	Player.Move();
}

void Renderer::DeallocateMesh(const Mesh* MeshToDestroy) const {
	if(!MeshToDestroy) return;

	if(MeshToDestroy->IndexHandle != nullptr) 
		vkDestroyBuffer(Device, MeshToDestroy->IndexHandle, nullptr);
	if (MeshToDestroy->IndexData != nullptr)
		vkFreeMemory(Device, MeshToDestroy->IndexData, nullptr);
	if (MeshToDestroy->VertexHandle != nullptr)
		vkDestroyBuffer(Device, MeshToDestroy->VertexHandle, nullptr);
	if (MeshToDestroy->VertexData != nullptr)
		vkFreeMemory(Device, MeshToDestroy->VertexData, nullptr);
}

void Renderer::DeallocateShader(const Shader& ShaderToDestroy) const {
	vkDestroyShaderModule(Device, ShaderToDestroy.VertShader, nullptr);
	vkDestroyShaderModule(Device, ShaderToDestroy.PixShader, nullptr);
}

void Renderer::CleanUp() {
	// wait till everything has completed
	vkDeviceWaitIdle(Device);
	// Release allocated buffers, shaders & pipeline

	CurrentLevel.Destroy();

	for (const auto& pair : Shaders) {
		DeallocateShader(pair.second);
	}
	Shaders.clear();

	for (const auto buffer : StorageHandle) {
		vkDestroyBuffer(Device, buffer, nullptr);
	}

	for (const auto handle : StorageData) {
		vkFreeMemory(Device, handle, nullptr);
	}
		
	vkDestroyDescriptorSetLayout(Device, DescriptorLayout, nullptr);

	vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
	vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
	vkDestroyPipeline(Device, Pipeline, nullptr);
}

GW::MATH::GMATRIXF Renderer::GetDefaultProjectionMatrix() const {
	float aspectRatio;
	vlk.GetAspectRatio(aspectRatio);
	GW::MATH::GMATRIXF ret = GW::MATH::GIdentityMatrixF;
	GW::MATH::GMatrix::ProjectionVulkanLHF(65.0f * (3.14159f / 180.0f), aspectRatio, 0.1f, 100.0f, ret);
	return ret;
}

void Renderer::GetWindowResolution(unsigned& Width, unsigned& Height) const {
	win.GetClientWidth(Width);
	win.GetClientHeight(Height);
}

