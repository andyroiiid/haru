#include "actors/APlayer.h"

#include <PxRigidDynamic.h>
#include <characterkinematic/PxController.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/system/Window.h>

#include "Scene.h"
#include "GameStatics.h"
#include "actors/APhysBox.h"

APlayer::APlayer(
        const glm::vec3 &position, float yaw,
        float mouseSpeed
) : m_controller(GameStatics::GetPhysicsScene()->CreateController({position.x, position.y, position.z}, 0.4f, 1.8f)),
    m_mouseSpeed(mouseSpeed) {
    m_controller->getActor()->userData = this;
    GetTransform().SetPosition(position).RotateY(yaw);
    m_previousPosition = position;
}

APlayer::~APlayer() {
    PX_RELEASE(m_controller)
}

void APlayer::ReadMovementInput() {
    Window *window = GameStatics::GetWindow();
    Transform &transform = GetTransform();
    m_movementInput =
            transform.GetHorizontalRightVector() * window->GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
            transform.GetHorizontalForwardVector() * window->GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
}

void APlayer::Update(const float deltaTime) {
    // turn
    {
        const glm::vec2 &deltaMousePos = GameStatics::GetWindow()->GetMouseDeltaPosition();
        GetTransform()
                .RotateX(m_mouseSpeed * -deltaMousePos.y)
                .RotateY(m_mouseSpeed * -deltaMousePos.x)
                .ClampPitch();
    }

    ReadMovementInput();

    // sync position
    {
        const physx::PxVec3 position = toVec3(m_controller->getPosition());
        const float timeError = GameStatics::GetPhysicsScene()->GetFixedUpdateTimeError();
        const glm::vec3 predictedPosition{
                position.x + m_velocity.x * timeError,
                position.y + m_velocity.y * timeError,
                position.z + m_velocity.z * timeError
        };
        GetTransform().SetPosition(predictedPosition + glm::vec3{0.0f, 0.5f, 0.0f}); // + 0.5 for the eye height
    }

    // raycast for current target
    bool currLmb = GameStatics::GetWindow()->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    if (!m_prevLmb && currLmb) {
        const glm::vec3 position = GetTransform().GetPosition();
        const glm::vec3 forward = GetTransform().GetForwardVector();

        physx::PxRaycastBuffer buffer = GameStatics::GetPhysicsScene()->Raycast(
                {position.x, position.y, position.z},
                {forward.x, forward.y, forward.z},
                10.0f,
                PHYSICS_LAYER_0
        );

        if (buffer.hasBlock) {
            const physx::PxRaycastHit &hit = buffer.block;

            m_lastHitPosition.x = hit.position.x;
            m_lastHitPosition.y = hit.position.y;
            m_lastHitPosition.z = hit.position.z;

            m_lastHitNormal.x = hit.normal.x;
            m_lastHitNormal.y = hit.normal.y;
            m_lastHitNormal.z = hit.normal.z;

            if (hit.actor) {
                m_currentTarget = static_cast<Actor *>(hit.actor->userData);

                auto *physBox = m_currentTarget->Cast<APhysBox>();
                if (physBox) {
                    physBox->AddImpulse(glm::normalize(m_lastHitPosition - position) * 10.0f);
                }
            }
        } else {
            m_lastHitPosition = {};
            m_lastHitNormal = {};
        }
    }
    m_prevLmb = currLmb;
}

void APlayer::CalcHorizontalAcceleration(const glm::vec3 &direction, float acceleration, float drag) {
    m_acceleration.x = direction.x * acceleration - m_velocity.x * drag;
    m_acceleration.z = direction.z * acceleration - m_velocity.z * drag;
}

void APlayer::UpdateAcceleration() {
    m_acceleration = {};
    CalcHorizontalAcceleration(m_movementInput, m_groundAcceleration, m_groundDrag);
    m_acceleration.y = -m_gravity;
}

void APlayer::FixedUpdate(float fixedDeltaTime) {
    // move character controller
    UpdateAcceleration();

    m_velocity += m_acceleration * fixedDeltaTime;
    const glm::vec3 displacement = m_velocity * fixedDeltaTime;
    physx::PxControllerCollisionFlags flags = m_controller->move(
            {displacement.x, displacement.y, displacement.z},
            0.01f,
            fixedDeltaTime,
            physx::PxControllerFilters()
    );

    // calc projected velocity
    const physx::PxVec3 pos = toVec3(m_controller->getPosition());
    const glm::vec3 currentPosition{pos.x, pos.y, pos.z};
    m_velocity = (currentPosition - m_previousPosition) / fixedDeltaTime;
    if (flags.isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN)) {
        m_velocity.y = 0.0f;
    }
    m_previousPosition = currentPosition;
}

void APlayer::Draw(Renderer &renderer) {
    renderer.DrawPointLight(
            GetTransform().GetPosition(),
            {0.4f, 0.4f, 0.4f},
            64.0f
    );

    m_hitLine.UpdateData(
            {
                    {{0.0f, 0.0f, 0.0f}},
                    {m_lastHitPosition},
                    {m_lastHitPosition},
                    {m_lastHitPosition + m_lastHitNormal},
            }
    );
    renderer.DrawLines(m_hitLine, {1.0f, 0.0f, 0.0f, 1.0f});
}
