//
// Created by andyroiiid on 11/20/2022.
//

#include "Game.h"

#include <haru/core/Debug.h>
#include <haru/render/DeferredRenderer.h>

#include "actors/ADirectionalLight.h"
#include "actors/ACamera.h"
#include "actors/APlayerNoClip.h"
#include "actors/APhysxBox.h"
#include "actors/ALevelGeom.h"

void Game::Init() {
    DebugLog("Game init");

    m_renderer = std::make_unique<DeferredRenderer>();
    m_physics = std::make_unique<PhysicsSystem>();
    m_physicsScene = std::make_unique<PhysicsScene>(m_physics.get());
    m_scene = std::make_unique<Scene>();

    Window->SetCursorEnabled(false);

    m_scene->CreateActor<ADirectionalLight>(1.0f);

    auto *camera = m_scene->CreateActor<ACamera>(Window);
    auto *player = m_scene->CreateActor<APlayerNoClip>(
            m_physicsScene.get(),
            Window,
            m_scene.get(),
            glm::vec3{0.0f, 1.8f, 15.0f}
    );
    camera->SetTargetActor(player);

    m_scene->CreateActor<ALevelGeom>(m_physics.get(), m_physicsScene.get(), "data/hello.haru");
}

void Game::Shutdown() {
    Window->SetCursorEnabled(true);

    m_scene.reset();
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
        if (const auto box = m_scene->FindFirstActorOfClass<APhysxBox>()) {
            box->Destroy();
        }
    }

    if (Window->IsKeyDown(GLFW_KEY_TAB)) {
        m_timeScale = 0.1f;
    } else {
        m_timeScale = 1.0f;
    }

    m_scene->Update(deltaTime * m_timeScale);
}

void Game::Draw() {
    m_renderer->BeginDraw();
    m_scene->Draw(*m_renderer);
    m_renderer->EndDraw();
}
