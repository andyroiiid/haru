#include "APlayer.h"

#include <PxRigidActor.h>
#include <characterkinematic/PxController.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/system/Window.h>

#include "../Scene.h"
#include "../GameStatics.h"

APlayer::APlayer(
        const glm::vec3 &position, float yaw,
        float mouseSpeed,
        float flySpeed
) : m_physicsScene(GameStatics::GetPhysicsScene()),
    m_window(GameStatics::GetWindow()),
    m_controller(m_physicsScene->CreateController({position.x, position.y, position.z}, 0.4f, 1.8f)),
    m_mouseSpeed(mouseSpeed),
    m_flySpeed(flySpeed) {
    m_controller->setUserData(this);
    GetTransform().SetPosition(position).RotateY(yaw);
    m_previousPosition = position;
}

APlayer::~APlayer() {
    PX_RELEASE(m_controller)
}

void APlayer::Update(const float deltaTime) {
    // turn
    {
        const glm::vec2 &deltaMousePos = m_window->GetMouseDeltaPosition();
        GetTransform()
                .RotateX(m_mouseSpeed * -deltaMousePos.y)
                .RotateY(m_mouseSpeed * -deltaMousePos.x)
                .ClampPitch();
    }

    // move
    {
        const glm::vec3 inputDirection = GetInputDirection();
        m_inputVelocity = inputDirection * m_flySpeed;
    }

    // sync position
    {
        const physx::PxVec3 position = toVec3(m_controller->getPosition());
        const float timeError = m_physicsScene->GetFixedUpdateTimeError();
        const glm::vec3 predictedPosition{
                position.x + m_projectedVelocity.x * timeError,
                position.y + m_projectedVelocity.y * timeError,
                position.z + m_projectedVelocity.z * timeError
        };
        GetTransform().SetPosition(predictedPosition + glm::vec3{0.0f, 0.5f, 0.0f}); // + 0.5 for the eye height
    }
}

void APlayer::FixedUpdate(float fixedDeltaTime) {
    // move character controller
    {
        const glm::vec3 displacement = m_inputVelocity * fixedDeltaTime;
        m_controller->move(
                {displacement.x, displacement.y, displacement.z},
                0.01f,
                fixedDeltaTime,
                physx::PxControllerFilters()
        );
    }

    // calc projected velocity
    {
        const physx::PxVec3 pos = toVec3(m_controller->getPosition());
        const glm::vec3 currentPosition{pos.x, pos.y, pos.z};
        m_projectedVelocity = (currentPosition - m_previousPosition) / fixedDeltaTime;
        m_previousPosition = currentPosition;
    }
}

void APlayer::Draw(Renderer &renderer) {
    renderer.DrawPointLight(
            GetTransform().GetPosition(),
            {0.5f, 0.5f, 0.5f},
            64.0f
    );
}

glm::vec3 APlayer::GetInputDirection() {
    Transform &transform = GetTransform();

    return transform.GetHorizontalRightVector() * m_window->GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
           glm::vec3{0.0f, 1.0f, 0.0f} * m_window->GetKeyAxis(GLFW_KEY_E, GLFW_KEY_Q) +
           transform.GetHorizontalForwardVector() * m_window->GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
}
