//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include <memory>

#include "Actor.h"

class ALevelGeom : public Actor {
public:
    DEFINE_ACTOR_CLASS(ALevelGeom)

    explicit ALevelGeom(const std::string &levelName);

    ~ALevelGeom() override;

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    MeshBase m_mesh;
};
