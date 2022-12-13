#pragma once

// Simple basecode showing how to create a window and attatch a vulkansurface
#define GATEWARE_ENABLE_CORE // All libraries need this
#define GATEWARE_ENABLE_SYSTEM // Graphics libs require system level libraries
#define GATEWARE_ENABLE_GRAPHICS // Enables all Graphics Libraries
#define GATEWARE_ENABLE_MATH
#define GATEWARE_ENABLE_INPUT
// Ignore some GRAPHICS libraries we aren't going to use
#define GATEWARE_DISABLE_GDIRECTX11SURFACE // we have another template for this
#define GATEWARE_DISABLE_GDIRECTX12SURFACE // we have another template for this
#define GATEWARE_DISABLE_GRASTERSURFACE // we have another template for this
#define GATEWARE_DISABLE_GOPENGLSURFACE // we have another template for this
// With what we want & what we don't defined we can include the API
#include "../gateware/Gateware.h"

// open some namespaces to compact the code a bit
using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;

#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "shaderc/shaderc.h" // needed for compiling shaders at runtime
#ifdef _WIN32 // must use MT platform DLL libraries on windows
#pragma comment(lib, "shaderc_combined.lib")
#endif

// Order matters here.
#include "h2bParser.h"

#define MAX_SUBMESH_PER_DRAW 1024
struct SHADER_MODEL_DATA {
	GW::MATH::GVECTORF SunDirection, SunColor, EyePos;
	GW::MATH::GMATRIXF ViewMatrix, ProjectionMatrix;
	GW::MATH::GMATRIXF Transforms[MAX_SUBMESH_PER_DRAW];
	H2B::ATTRIBUTES Attributes[MAX_SUBMESH_PER_DRAW];
};

enum AssetType {
	MeshAsset,
	LightAsset,
	UnknownAsset = -1
};

#include "Light.h"
#include "Mesh.h"
#include "Camera.h"
#include "Actor.h"
#include "Shader.h"

#include "MeshParser.h"
#include "ShaderParser.h"
#include "LevelParser.h"

#include "FloatingCameraController.h"

#include "Level.h"

#include "renderer.h"
