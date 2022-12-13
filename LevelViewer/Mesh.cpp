#include "Globals.h"

Mesh::Mesh(const char* MeshPath) {
	 if(!MeshParser::Load(MeshPath, this)) {
		std::cout << "Error loading mesh " << MeshPath << std::endl;
		 //Renderer::Get().CompileMesh(this);
	 }
}

Mesh::~Mesh() = default;

void Mesh::Destroy() const {
	Renderer::Get().DeallocateMesh(this);
	delete this;
}
