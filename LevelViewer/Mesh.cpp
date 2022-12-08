#include "Mesh.h"

#include "h2bParser.h"
#include "MeshParser.h"

Mesh::Mesh(const char* MeshPath) {
	 MeshParser::Load(MeshPath, this);
}
