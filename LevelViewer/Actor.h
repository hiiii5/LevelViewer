#pragma once

#define GATEWARE_ENABLE_GRAPHICS
#include "../gateware/Gateware.h"

#include "Mesh.h"

class Actor {
public:
    explicit Actor(const char* MeshFile);
    ~Actor();
    
    Mesh* MeshAsset;
    GW::MATH::GMATRIXF Transform{};

    void Render();
};

