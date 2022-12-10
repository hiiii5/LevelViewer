#include "Mesh.h"

#include "MeshParser.h"

Mesh::Mesh(const char* MeshPath) {
	 MeshParser::Load(MeshPath, this);
}
