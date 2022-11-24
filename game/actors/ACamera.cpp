//
// Created by andyroiiid on 11/24/2022.
//

#include "ACamera.h"

#include <haru/system/Window.h>

ACamera::ACamera(Window *window, float fov, float near, float far)
        : m_window(window), m_fov(fov), m_near(near), m_far(far) {
}

void ACamera::Update(float deltaTime) {
    if (!m_targetActor) return;

    if (m_targetActor->IsPendingDestroy()) {
        m_targetActor = nullptr;
    }

    GetTransform() = m_targetActor->GetTransform();
}

void ACamera::Draw(Renderer &renderer) {
    const glm::ivec2 screenSize = m_window->GetSize();

    renderer.SetCameraMatrices(
            GetTransform().GetInverseMatrix(),
            glm::perspective(
                    glm::radians(m_fov),
                    static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y),
                    m_near,
                    m_far
            )
    );
}
