#include "APlayer.h"

#include <PxRigidActor.h>
#include <characterkinematic/PxController.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/system/Window.h>

#include "APhysxBox.h"
#include "../Scene.h"
#include "../GameStatics.h"

APlayer::APlayer(
        const glm::vec3 &position, float yaw,
        float mouseSpeed,
        float flySpeed
) : m_physicsScene(GameStatics::GetPhysicsScene()),
    m_window(GameStatics::GetWindow()),
    m_scene(GameStatics::GetScene()),
    m_controller(m_physicsScene->CreateController({position.x, position.y, position.z}, 0.4f, 1.8f)),
    m_mouseSpeed(mouseSpeed),
    m_flySpeed(flySpeed) {
    GetTransform().SetPosition(position).RotateY(yaw);
}

APlayer::~APlayer() {
    PX_RELEASE(m_controller)
}

void APlayer::Update(const float deltaTime) {
    Turn();
    Move(deltaTime);
    CreateBox();
    DestroyBox();
}

void APlayer::Draw(Renderer &renderer) {
    renderer.DrawPointLight(
            GetTransform().GetPosition(),
            {1.0f, 1.0f, 1.0f},
            64.0f
    );
}

glm::vec3 APlayer::GetInputDirection() {
    Transform &transform = GetTransform();

    return transform.GetHorizontalRightVector() * m_window->GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
           glm::vec3{0.0f, 1.0f, 0.0f} * m_window->GetKeyAxis(GLFW_KEY_E, GLFW_KEY_Q) +
           transform.GetHorizontalForwardVector() * m_window->GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
}

void APlayer::Turn() {
    const glm::vec2 &deltaMousePos = m_window->GetMouseDeltaPosition();
    GetTransform()
            .RotateX(m_mouseSpeed * -deltaMousePos.y)
            .RotateY(m_mouseSpeed * -deltaMousePos.x)
            .ClampPitch();
}

void APlayer::Move(float deltaTime) {
    const glm::vec3 inputDirection = GetInputDirection();
    const glm::vec3 displacement = inputDirection * m_flySpeed * deltaTime;

    m_controller->move(
            {displacement.x, displacement.y, displacement.z},
            0.01f,
            deltaTime,
            physx::PxControllerFilters()
    );

    const auto &position = m_controller->getPosition();
    GetTransform().SetPosition({position.x, position.y, position.z});
}

void APlayer::CreateBox() {
    Transform &transform = GetTransform();
    const glm::vec3 &position = transform.GetPosition();
    const glm::vec3 &forward = transform.GetForwardVector();

    const bool currLmb = m_window->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    if (currLmb && !m_prevLmb) {
        m_scene->CreateActor<APhysxBox>(
                m_physicsScene,
                position + forward,
                glm::vec3{0.5f, 0.5f, 0.5f},
                forward * 20.0f
        );
    }
    m_prevLmb = currLmb;
}

void APlayer::DestroyBox() {
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
