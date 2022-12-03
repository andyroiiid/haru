//
// Created by andyroiiid on 12/3/2022.
//

#include "AFuncTest.h"

void AFuncTest::Draw(Renderer &renderer) {
    AFuncBrush::Draw(renderer);
    renderer.DrawPointLight(GetTransform().GetPosition(), {1.0f, 0.0f, 0.0f}, 5.0f);
}

void AFuncTest::Use(APlayer *player, const physx::PxRaycastHit &hit) {
    Destroy();
}
