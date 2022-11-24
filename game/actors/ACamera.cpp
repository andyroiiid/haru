//
// Created by andyroiiid on 11/24/2022.
//

#include "ACamera.h"

ACamera::ACamera(float fov, float near, float far)
        : m_fov(fov), m_near(near), m_far(far) {
}

void ACamera::Update(float deltaTime) {
    if (!m_targetActor) return;

    if (m_targetActor->IsPendingDestroy()) {
        m_targetActor = nullptr;
    }

    GetTransform() = m_targetActor->GetTransform();
}

void ACamera::Draw(Renderer &renderer) {
    const glm::vec2 screenSize = renderer.GetSize();

    renderer.SetCameraMatrices(
            GetTransform().GetInverseMatrix(),
            glm::perspective(
                    glm::radians(m_fov),
                    screenSize.x / screenSize.y,
                    m_near,
                    m_far
            )
    );
}
