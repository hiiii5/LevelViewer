#pragma once
#include "Globals.h"

class Mesh;

class MeshParser
{
public:
	/**
	 * \brief Loads a mesh from disk, streaming the data into the target mesh.
	 * \param MeshPath Path locally to the mesh on disk.
	 * \param TargetMesh Target mesh to stream data in to, must not be null.
	 * \return True if successful. False if the mesh failed to load, or if targetMesh was null.
	 */
	static bool Load(const char* MeshPath, Mesh* TargetMesh) {
		const auto parser = new H2B::Parser;

		// Attempt to parse the mesh from binary data, at the destination path.
		const auto success = parser->Parse(MeshPath);

		if(!success || !TargetMesh) {
			return false;
		}

		// The operation was successful so prepare the mesh object.
		memcpy(TargetMesh->Version, parser->version, sizeof(char) * 4);
		TargetMesh->VertexCount = parser->vertexCount;
		TargetMesh->IndexCount = parser->indexCount;
		TargetMesh->MaterialCount = parser->materialCount;
		TargetMesh->MeshCount = parser->meshCount;
		TargetMesh->Vertices = parser->vertices;
		TargetMesh->Indices = parser->indices;
		TargetMesh->Materials = parser->materials;
		TargetMesh->Batches = parser->batches;
		TargetMesh->Meshes = parser->meshes;

		return true;
	}
};

