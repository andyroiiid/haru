//
// Created by andyroiiid on 12/3/2022.
//

#pragma once

#include "actors/AFuncBrush.h"

class AFuncTest : public AFuncBrush {
public:
    DEFINE_ACTOR_CLASS(AFuncTest)

    explicit AFuncTest(const EntityBrushes& brushes) : AFuncBrush(brushes) {}

    void Draw(Renderer &renderer) override;

    void Use(APlayer *player, const physx::PxRaycastHit &hit) override;
};
