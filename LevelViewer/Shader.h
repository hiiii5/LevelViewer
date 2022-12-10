#pragma once

#define GATEWARE_ENABLE_GRAPHICS
#include "../gateware/Gateware.h"

struct Shader {
	VkShaderModule VertShader, PixShader;
};

