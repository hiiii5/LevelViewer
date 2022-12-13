#pragma once

#include "Globals.h"

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

	VkBuffer VertexHandle = nullptr;
	VkDeviceMemory VertexData = nullptr;

	VkBuffer IndexHandle = nullptr;
	VkDeviceMemory IndexData = nullptr;

	explicit Mesh(const char* MeshPath);
	~Mesh();
	void Destroy() const;
};
