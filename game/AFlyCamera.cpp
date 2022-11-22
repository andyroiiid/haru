#include "AFlyCamera.h"

#include <PxRigidActor.h>

#include <haru/physics/PhysicsSystem.h>
#include <haru/system/Window.h>

#include "APhysBoxDynamic.h"
#include "Scene.h"

AFlyCamera::AFlyCamera(
        PhysicsSystem *physics,
        Window *window,
        Scene *scene,
        const glm::vec3 &position, float yaw,
        const CameraConfig &cameraConfig
) : m_physics(physics), m_window(window), m_scene(scene), m_cameraConfig(cameraConfig) {
    GetTransform().SetPosition(position).RotateY(yaw);
}

void AFlyCamera::Update(const float deltaTime) {
    Transform &transform = GetTransform();

    const glm::vec2 &deltaMousePos = m_window->GetMouseDeltaPosition();
    transform
            .RotateX(m_cameraConfig.MouseSpeed * -deltaMousePos.y)
            .RotateY(m_cameraConfig.MouseSpeed * -deltaMousePos.x)
            .ClampPitch();

    static constexpr glm::vec3 UP{0.0f, 1.0f, 0.0f};
    if (const glm::vec3 inputDirection =
                transform.GetHorizontalRightVector() * m_window->GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
                UP * m_window->GetKeyAxis(GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL) +
                transform.GetHorizontalForwardVector() * m_window->GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
            inputDirection.x != 0.0f || inputDirection.y != 0.0f || inputDirection.z != 0.0f) {
        float flySpeed = m_cameraConfig.FlySpeed;
        if (m_window->IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
            flySpeed *= 2.0f;
        }
        transform.Translate(normalize(inputDirection) * (flySpeed * deltaTime));
    }

    const bool currF = m_window->IsKeyDown(GLFW_KEY_F);
    if (currF && !m_prevF) {
        m_flashLight = !m_flashLight;
    }
    m_prevF = currF;

    const glm::vec3 &position = transform.GetPosition();
    const glm::vec3 &forward = transform.GetForwardVector();

    const bool currLmb = m_window->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    if (currLmb && !m_prevLmb) {
        m_scene->CreateActor<APhysBoxDynamic>(
                m_physics,
                position,
                glm::vec3{0.5f, 0.5f, 0.5f},
                forward * 20.0f
        );
    }
    m_prevLmb = currLmb;

    const bool currRmb = m_window->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT);
    if (currRmb && !m_prevRmb) {
        const auto hit = m_physics->Raycast(
                {position.x, position.y, position.z},
                {forward.x, forward.y, forward.z},
                10.0f
        );
        if (hit.hasBlock) {
            const auto actor = static_cast<Actor *>(hit.block.actor->userData);
            if (actor->IsClass<APhysBoxDynamic>()) {
                actor->Destroy();
            }
        }
    }
    m_prevRmb = currRmb;
}

void AFlyCamera::Draw(Renderer &renderer) {
    const glm::ivec2 screenSize = m_window->GetSize();

    renderer.SetCameraMatrices(
            GetTransform().GetInverseMatrix(),
            glm::perspective(
                    glm::radians(m_cameraConfig.FieldOfViewDegrees),
                    static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y),
                    m_cameraConfig.NearPlane,
                    m_cameraConfig.FarPlane
            )
    );

    if (m_flashLight) {
        renderer.DrawPointLight(
                GetTransform().GetPosition(),
                {1.0f, 1.0f, 1.0f},
                64.0f
        );
    }
}
