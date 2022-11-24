//
// Created by andyroiiid on 11/20/2022.
//

#include "Game.h"

#include <haru/core/Debug.h>
#include <haru/render/DeferredRenderer.h>

#include "ADirectionalLight.h"
#include "APointLight.h"
#include "AFlyCamera.h"
#include "APhysBoxStatic.h"
#include "APhysBoxDynamic.h"
#include "ALevelGeom.h"

void Game::Init() {
    DebugLog("Game init");

    m_renderer = std::make_unique<DeferredRenderer>();
    m_physics = std::make_unique<PhysicsSystem>();
    m_physicsScene = std::make_unique<PhysicsScene>(m_physics.get());

    Window->SetCursorEnabled(false);

    m_scene.CreateActor<ADirectionalLight>(0.0f);
    m_scene.CreateActor<AFlyCamera>(
            m_physicsScene.get(),
            Window,
            &m_scene,
            glm::vec3{0.0f, 1.8f, 15.0f}
    );

    m_scene.CreateActor<APointLight>(glm::vec3{12.0f, 1.0f, 12.0f}, glm::vec3{1.0f, 0.5f, 0.0f}, 128.0f);
    m_scene.CreateActor<APointLight>(glm::vec3{12.0f, 1.0f, -12.0f}, glm::vec3{1.0f, 1.0f, 1.0f}, 128.0f);
    m_scene.CreateActor<APointLight>(glm::vec3{-12.0f, 1.0f, 12.0f}, glm::vec3{1.0f, 1.0f, 1.0f}, 128.0f);
    m_scene.CreateActor<APointLight>(glm::vec3{-12.0f, 1.0f, -12.0f}, glm::vec3{0.4f, 0.8f, 1.0f}, 128.0f);
    m_scene.CreateActor<APointLight>(glm::vec3{0.0f, 9.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 1.0f}, 128.0f);

    m_scene.CreateActor<APhysBoxStatic>(
            m_physicsScene.get(),
            glm::vec3{0.0f, -10.5f, 0.0f},
            glm::vec3{20.0f, 0.5f, 20.0f}
    );
    m_scene.CreateActor<APhysBoxStatic>(
            m_physicsScene.get(),
            glm::vec3{0.0f, 10.5f, 0.0f},
            glm::vec3{20.0f, 0.5f, 20.0f}
    );
    m_scene.CreateActor<APhysBoxStatic>(
            m_physicsScene.get(),
            glm::vec3{20.5f, 0.0f, 0.0f},
            glm::vec3{0.5f, 11.0f, 20.0f}
    );
    m_scene.CreateActor<APhysBoxStatic>(
            m_physicsScene.get(),
            glm::vec3{-20.5f, 0.0f, 0.0f},
            glm::vec3{0.5f, 11.0f, 20.0f}
    );
    m_scene.CreateActor<APhysBoxStatic>(
            m_physicsScene.get(),
            glm::vec3{0.0f, 0.0f, 20.5f},
            glm::vec3{21.0f, 11.0f, 0.5f}
    );
    m_scene.CreateActor<APhysBoxStatic>(
            m_physicsScene.get(),
            glm::vec3{0.0f, 0.0f, -20.5f},
            glm::vec3{21.0f, 11.0f, 0.5f}
    );

    m_scene.CreateActor<ALevelGeom>("data/hello.haru");
}

void Game::Shutdown() {
    m_scene.Cleanup();

    Window->SetCursorEnabled(true);

    m_physicsScene.reset();
    m_physics.reset();
    m_renderer.reset();

    DebugLog("Game shutdown");
}

void Game::Resize(const glm::ivec2 &size) {
    m_renderer->Resize(size);
}

void Game::Update(const float deltaTime) {
    m_physicsScene->Update(deltaTime, m_timeScale);

    if (Window->IsKeyDown(GLFW_KEY_ESCAPE)) {
        Window->Close();
    }

    if (Window->IsKeyDown(GLFW_KEY_DELETE)) {
        if (const auto box = m_scene.FindFirstActorOfClass<APhysBoxDynamic>()) {
            box->Destroy();
        }
    }

    if (Window->IsKeyDown(GLFW_KEY_TAB)) {
        m_timeScale = 0.1f;
    } else {
        m_timeScale = 1.0f;
    }

    m_scene.Update(deltaTime * m_timeScale);
}

void Game::Draw() {
    m_renderer->BeginDraw();
    m_scene.Draw(*m_renderer);
    m_renderer->EndDraw();
}