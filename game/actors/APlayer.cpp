#include "APlayer.h"

#include <PxRigidActor.h>
#include <characterkinematic/PxController.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/system/Window.h>

#include "../Scene.h"
#include "../GameStatics.h"
#include "APointLight.h"
#include "APhysBox.h"

APlayer::APlayer(
        const glm::vec3 &position, float yaw,
        float mouseSpeed
) : m_physicsScene(GameStatics::GetPhysicsScene()),
    m_window(GameStatics::GetWindow()),
    m_controller(m_physicsScene->CreateController({position.x, position.y, position.z}, 0.4f, 1.8f)),
    m_mouseSpeed(mouseSpeed) {
    m_controller->setUserData(this);
    GetTransform().SetPosition(position).RotateY(yaw);
    m_previousPosition = position;
}

APlayer::~APlayer() {
    PX_RELEASE(m_controller)
}

void APlayer::ReadMovementInput() {
    Transform &transform = GetTransform();
    m_movementInput =
            transform.GetHorizontalRightVector() * m_window->GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
            transform.GetHorizontalForwardVector() * m_window->GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
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

    ReadMovementInput();

    // sync position
    {
        const physx::PxVec3 position = toVec3(m_controller->getPosition());
        const float timeError = m_physicsScene->GetFixedUpdateTimeError();
        const glm::vec3 predictedPosition{
                position.x + m_velocity.x * timeError,
                position.y + m_velocity.y * timeError,
                position.z + m_velocity.z * timeError
        };
        GetTransform().SetPosition(predictedPosition + glm::vec3{0.0f, 0.5f, 0.0f}); // + 0.5 for the eye height
    }

    bool currLmb = m_window->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    if (currLmb && !m_prevLmb) {
        APointLight *&pointLight = m_pointLights[m_nextPointLight];
        if (pointLight) {
            pointLight->Destroy();
        }
        const glm::vec3 lightPosition = GetTransform().GetPosition() - glm::vec3{0.0f, 0.5f, 0.0f};
        const glm::vec3 lightColor = glm::vec3{
                m_random.Float(0.0f, 16.0f),
                m_random.Float(0.0f, 16.0f),
                m_random.Float(0.0f, 16.0f)
        };
        pointLight = GameStatics::GetScene()->CreateActor<APointLight>(lightPosition, lightColor, 4.0f);
        m_nextPointLight = (m_nextPointLight + 1) % MAX_NUM_POINT_LIGHTS;
    }
    m_prevLmb = currLmb;

    bool currRmb = m_window->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT);
    if (currRmb && !m_prevRmb) {
        APhysBox *&physBox = m_physBoxes[m_nextPhysBox];
        if (physBox) {
            physBox->Destroy();
        }
        const Transform &transform = GetTransform();
        const glm::vec3 boxPosition = transform.GetPosition() + transform.GetHorizontalForwardVector() * 5.0f + glm::vec3{0.0f, 5.0f, 0.0f};
        const glm::vec3 boxSize = glm::vec3{
                m_random.Float(0.5f, 2.0f),
                m_random.Float(0.5f, 2.0f),
                m_random.Float(0.5f, 2.0f)
        };
        physBox = GameStatics::GetScene()->CreateActor<APhysBox>(boxPosition, boxSize);
        m_nextPhysBox = (m_nextPhysBox + 1) % MAX_NUM_BOXES;
    }
    m_prevRmb = currRmb;
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
}
