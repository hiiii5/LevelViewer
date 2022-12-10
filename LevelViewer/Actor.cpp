#include "Actor.h"

#include "renderer.h"

Actor::Actor(const char* MeshFile) {
    MeshAsset = new Mesh(MeshFile);
    Transform = GW::MATH::GIdentityMatrixF;
}

Actor::~Actor() {
	delete MeshAsset;
}

void Actor::Render() {
	auto rendererInst = Renderer::Get();
	unsigned currentBuffer;
	VkCommandBuffer commandBuffer;
	rendererInst->GetCurrentBuffers(currentBuffer, commandBuffer);
	rendererInst->RenderActor(commandBuffer, this, rendererInst->GetWorldShaderData());
}

