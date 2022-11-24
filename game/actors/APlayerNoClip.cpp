#include "APlayerNoClip.h"

#include <PxRigidActor.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/system/Window.h>

#include "APhysxBox.h"
#include "../Scene.h"
#include "../GameStatics.h"

APlayerNoClip::APlayerNoClip(
        const glm::vec3 &position, float yaw,
        float mouseSpeed,
        float flySpeed
) : m_physicsScene(GameStatics::GetPhysicsScene()),
    m_window(GameStatics::GetWindow()),
    m_scene(GameStatics::GetScene()),
    m_mouseSpeed(mouseSpeed),
    m_flySpeed(flySpeed) {
    GetTransform().SetPosition(position).RotateY(yaw);
}

void APlayerNoClip::Update(const float deltaTime) {
    Turn();
    Move(deltaTime);
    CreateBox();
    DestroyBox();
}

void APlayerNoClip::Draw(Renderer &renderer) {
    renderer.DrawPointLight(
            GetTransform().GetPosition(),
            {1.0f, 1.0f, 1.0f},
            64.0f
    );
}

glm::vec3 APlayerNoClip::GetInputDirection() {
    Transform &transform = GetTransform();

    return transform.GetHorizontalRightVector() * m_window->GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
           glm::vec3{0.0f, 1.0f, 0.0f} * m_window->GetKeyAxis(GLFW_KEY_E, GLFW_KEY_Q) +
           transform.GetHorizontalForwardVector() * m_window->GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
}

void APlayerNoClip::Turn() {
    const glm::vec2 &deltaMousePos = m_window->GetMouseDeltaPosition();
    GetTransform()
            .RotateX(m_mouseSpeed * -deltaMousePos.y)
            .RotateY(m_mouseSpeed * -deltaMousePos.x)
            .ClampPitch();
}

void APlayerNoClip::Move(float deltaTime) {
    const glm::vec3 inputDirection = GetInputDirection();
    if (inputDirection.x != 0.0f || inputDirection.y != 0.0f || inputDirection.z != 0.0f) {
        float flySpeed = m_flySpeed;
        if (m_window->IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
            flySpeed *= 2.0f;
        }
        GetTransform().Translate(normalize(inputDirection) * (flySpeed * deltaTime));
    }
}

void APlayerNoClip::CreateBox() {
    Transform &transform = GetTransform();

    const bool currLmb = m_window->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    if (currLmb && !m_prevLmb) {
        m_scene->CreateActor<APhysxBox>(
                m_physicsScene,
                transform.GetPosition(),
                glm::vec3{0.5f, 0.5f, 0.5f},
                transform.GetForwardVector() * 20.0f
        );
    }
    m_prevLmb = currLmb;
}

void APlayerNoClip::DestroyBox() {
    Transform &transform = GetTransform();
    const glm::vec3 &position = transform.GetPosition();
    const glm::vec3 &forward = transform.GetForwardVector();

    const bool currRmb = m_window->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT);
    if (currRmb && !m_prevRmb) {
        const auto hit = m_physicsScene->Raycast(
                {position.x, position.y, position.z},
                {forward.x, forward.y, forward.z},
                10.0f
        );
        if (hit.hasBlock) {
            const auto actor = static_cast<Actor *>(hit.block.actor->userData);
            if (actor->IsClass<APhysxBox>()) {
                actor->Destroy();
            }
        }
    }
    m_prevRmb = currRmb;
}
