#include "Globals.h"

// lets pop a window and use Vulkan to clear to a red screen
int main()
{
	GWindow win;
	GEventResponder msgs;
	GVulkanSurface vulkan;
	auto ret = win.Create(0, 0, 800, 600, GWindowStyle::WINDOWEDBORDERED);
	if (ret == GW::GReturn::SUCCESS)
	{
		// TODO: Part 1a
		win.SetWindowName("Dustin Roden - Assignment 2 - Vulkan");
		VkClearValue clrAndDepth[2];
		clrAndDepth[0].color = { {0.4f, 0.4f, 0.4f, 1} };
		clrAndDepth[1].depthStencil = { 1.0f, 0u };
		//msgs.Create([&](const GW::GEvent& e) {
		//	GW::SYSTEM::GWindow::Events q;
		//	if (+e.Read(q) && q == GWindow::Events::RESIZE)
		//		clrAndDepth[0].color.float32[2] += 0.01f; // disable
		//	});
		win.Register(msgs);
#ifndef NDEBUG
		const char* debugLayers[] = {
			"VK_LAYER_KHRONOS_validation", // standard validation layer
			//"VK_LAYER_LUNARG_standard_validation", // add if not on MacOS
			"VK_LAYER_RENDERDOC_Capture" // add this if you have installed RenderDoc
		};
		if (+vulkan.Create(	win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT, 
							sizeof(debugLayers)/sizeof(debugLayers[0]),
							debugLayers, 0, nullptr, 0, nullptr, false))
#else
		if (+vulkan.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT))
#endif
		{
			Renderer::Get().Init(win, vulkan);
			while (+win.ProcessWindowEvents())
			{
				if (+vulkan.StartFrame(2, clrAndDepth))
				{
					Renderer::Get().Render();
					Renderer::Get().Update();
					vulkan.EndFrame(true);
				}
			}
		}
	}
	return 0; // that'S all folks
}
