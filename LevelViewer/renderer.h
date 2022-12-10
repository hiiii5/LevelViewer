#pragma once

#include "Actor.h"
#include "h2bParser.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderParser.h"

#include "shaderc/shaderc.h" // needed for compiling shaders at runtime
#ifdef _WIN32 // must use MT platform DLL libraries on windows
#pragma comment(lib, "shaderc_combined.lib")
#endif

// Creation, Rendering & Cleanup
class Renderer {
	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GVulkanSurface vlk;
	GW::CORE::GEventReceiver shutdown;

	// what we need at a minimum to draw a triangle
	VkDevice device = nullptr;
	
	std::vector<VkBuffer> storageHandle{};
	std::vector<VkDeviceMemory> storageData{};
	// pipeline settings for drawing (also required)
	VkPipeline pipeline = nullptr;
	VkPipelineLayout pipelineLayout = nullptr;

	VkDescriptorSetLayout descriptorLayout = nullptr;

	VkDescriptorPool descriptorPool = nullptr;

	std::vector<VkDescriptorSet> descriptorSet{};

	SHADER_MODEL_DATA instanceData{};

	Actor* A;
	Shader S;

	static Renderer* InstPtr;

	// Default constructor for use with singleton pattern.
	Renderer(){}

public:
	// Delete copy constructor for use with singleton pattern.
	Renderer(const Renderer& Obj) = delete;

	static Renderer* Get() {
		if(InstPtr == nullptr) {
			InstPtr = new Renderer();
		}

		return InstPtr;
	}
	
	void InitRenderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GVulkanSurface _vlk);

	void InitPipeline(unsigned width, unsigned height, unsigned maxFrames);

	static void InitMesh(const VkDevice Device, const VkPhysicalDevice PhysicalDevice, Mesh& Target);

	static void CompileShader(const VkDevice& Device, const char* VsFile, const char* PsFile, Shader& ShaderRef);

	void RenderActor(const VkCommandBuffer CommandBuffer, const Actor* ActorToRender, const SHADER_MODEL_DATA& InstanceData);

	void GetCurrentBuffers(unsigned& CurrentBuffer, VkCommandBuffer& CommandBuffer) const;

	SHADER_MODEL_DATA GetWorldShaderData() const;

	void Render();

	void DeallocateMesh(const Mesh* MeshToDestroy) const;

	void DeallocateShader(const Shader& ShaderToDestroy) const;

private:
	void CleanUp();
};
