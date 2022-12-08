#pragma once
#include <vector>

namespace H2B {
	struct VERTEX;
	struct MATERIAL;
	struct BATCH;
	struct MESH;
}

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

	explicit Mesh(const char* MeshPath);
};
