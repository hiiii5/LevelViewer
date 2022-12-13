#include "Globals.h"

Actor::Actor(const char* MeshFile) {
    MeshComp = new Mesh(MeshFile);
    Transform = GW::MATH::GIdentityMatrixF;
}

Actor::~Actor() = default;

void Actor::Destroy() const {
	MeshComp->Destroy();
	delete this;
}

void Actor::Render() const {
	Renderer::Get().RenderActor(this, Renderer::Get().InstanceData);
}

