#pragma once

#include "Globals.h"

class Actor {
public:
    explicit Actor(const char* MeshFile);
    ~Actor();

    void Destroy() const;

    std::string Name;
    Mesh* MeshComp{};
    GW::MATH::GMATRIXF Transform{};

    void Render() const;
};

