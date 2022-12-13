#pragma once

#include "Globals.h"

// Creation, Rendering & Cleanup
class Renderer {
public:
	static Renderer& Get() {
		static Renderer inst;	// Guaranteed to be destroyed.
								// Instantiated on first use.
		return inst;
	}

private:
	// proxy handles
	GWindow win;
	GVulkanSurface vlk;
	GEventReceiver shutdown;
	
	std::vector<VkBuffer> StorageHandle{};
	std::vector<VkDeviceMemory> StorageData{};
	// pipeline settings for drawing (also required)
	VkPipeline Pipeline = nullptr;
	VkPipelineLayout PipelineLayout = nullptr;

	VkDescriptorSetLayout DescriptorLayout = nullptr;

	VkDescriptorPool DescriptorPool = nullptr;

	std::vector<VkDescriptorSet> DescriptorSet{};

	std::map<std::string, Shader> Shaders{};

	std::chrono::time_point<std::chrono::steady_clock> Start, Stop;

	// Default constructor for use with singleton pattern.
	Renderer(){}

	// Used by the renderer to start the default graphics pipeline.
	void InitPipeline(unsigned width, unsigned height, unsigned maxFrames);

	// Cleanup is called internally at a specific moment instead of destruction.
	void CleanUp();

public:
	// Delete copy constructor for use with singleton pattern.
	Renderer(const Renderer& Obj) = delete; // Don't implement.
	void operator=(Renderer const&) = delete; // Don't implement.

	void UpdateStorageBuffers(VkPhysicalDevice physicalDevice, unsigned& maxFrames, bool create);

	// Starts the renderer
	void Init(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GVulkanSurface _vlk);

	// Updates the renderer.
	void Render();

	void Update();

	// Used to get the current command buffers, instead of using this internally it is expected that the buffer is provided to other methods.
	void GetCurrentBuffers(unsigned& CurrentBuffer, VkCommandBuffer& CommandBuffer) const;

	// Binds a mesh to its data in vulkan.
	void CompileMesh(Mesh* Target);

	// Binds a shader to its data in vulkan.
	void CompileShader(Shader& ShaderRef, const VkDevice& Device);

	// Writes an actors mesh component to the buffers for drawing.
	void RenderActor(const Actor* ActorToRender, const SHADER_MODEL_DATA& CurrentData);

	// Get'S the worlds shader data TODO: Un-const this.
	SHADER_MODEL_DATA GetWorldShaderData() const;

	// Used to destroy a mesh'S data in the vulkan pipeline.
	void DeallocateMesh(const Mesh* MeshToDestroy) const;

	// Used to destroy a shader'S data in the vulkan pipeline.
	void DeallocateShader(const Shader& ShaderToDestroy) const;

	GW::MATH::GMATRIXF GetDefaultProjectionMatrix() const;

	void GetWindowResolution(unsigned int& Width, unsigned int& Height) const;

	GW::INPUT::GInput Input;
	GW::INPUT::GController Controller;
	FloatingCameraController Player{};

	float DeltaTime;

	Level CurrentLevel;

	// what we need at a minimum to draw a triangle
	VkDevice Device = nullptr;
	VkPhysicalDevice PhysicalDevice = nullptr;

	SHADER_MODEL_DATA InstanceData{};
};
