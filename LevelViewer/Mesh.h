#pragma once
#include <vector>

#include "h2bParser.h"

#define GATEWARE_ENABLE_GRAPHICS
#include "../gateware/Gateware.h"

#define MAX_SUBMESH_PER_DRAW 1024
struct SHADER_MODEL_DATA {
	GW::MATH::GVECTORF SunDirection, SunColor, EyePos;
	GW::MATH::GMATRIXF ViewMatrix, ProjectionMatrix;
	GW::MATH::GMATRIXF Transforms[MAX_SUBMESH_PER_DRAW];
	H2B::ATTRIBUTES Attributes[MAX_SUBMESH_PER_DRAW];
};

class Mesh {
public:
	char Version[4] = {};
	unsigned VertexCount = 0;
	unsigned IndexCount = 0;
	unsigned MaterialCount = 0;
	unsigned MeshCount = 0;
	std::vector<H2B::VERTEX> Vertices;
	std::vector<unsigned> Indices;
	std::vector<H2B::MATERIAL> Materials;
	std::vector<H2B::BATCH> Batches;
	std::vector<H2B::MESH> Meshes;

	SHADER_MODEL_DATA ShaderData;

	VkBuffer VertexHandle = nullptr;
	VkDeviceMemory VertexData = nullptr;

	VkBuffer IndexHandle = nullptr;
	VkDeviceMemory IndexData = nullptr;

	explicit Mesh(const char* MeshPath);
};
