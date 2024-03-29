//
// Created by andyroiiid on 11/20/2022.
//

#include "Game.h"

#include <haru/core/Debug.h>
#include <haru/render/DeferredRenderer.h>
#include <haru/render/Texture.h>
#include <tracy/Tracy.hpp>

#include "GameStatics.h"

#include "actors/AWorldLight.h"

#include "map/ParseMap.h"
#include "map/LoadEntities.h"

void Game::Init() {
    ZoneScoped;

    DebugLog("Game init");

    CreateEssentials();

    Window->SetCursorEnabled(false);

    std::vector<EntityDefinition> entities;
    ParseMap(m_startMap, entities);
    LoadEntities(entities);

    m_scene->CreateActor<AWorldLight>(
            glm::radians(30.0f),
            glm::radians(45.0f),
            glm::vec3{1.0f, 1.0f, 1.0f},
            glm::vec3{0.2f, 0.2f, 0.2f}
    );
}

void Game::Shutdown() {
    ZoneScoped;

    Window->SetCursorEnabled(true);

    DestroyEssentials();

    Texture::ClearCache();

    DebugLog("Game shutdown");
}

void Game::Resize(const glm::ivec2 &size) {
    m_renderer->Resize(size);
}

void Game::Update(const float deltaTime) {
    ZoneScoped;

    if (m_physicsScene->Update(deltaTime, m_timeScale)) {
        m_scene->FixedUpdate(m_physicsScene->GetFixedTimestep() * m_timeScale);
    }

    if (Window->IsKeyDown(GLFW_KEY_ESCAPE)) {
        Window->Close();
    }

    if (Window->IsKeyDown(GLFW_KEY_TAB)) {
        m_timeScale = 0.2f;
    } else {
        m_timeScale = 1.0f;
    }

    m_renderer->Update(deltaTime * m_timeScale);
    m_scene->Update(deltaTime * m_timeScale);
}

void Game::Draw() {
    ZoneScoped;

    m_renderer->BeginDraw();
    m_scene->Draw(*m_renderer);
    m_renderer->EndDraw();
}

void Game::CreateEssentials() {
    ZoneScoped;

    m_renderer = std::make_unique<DeferredRenderer>();
    m_physics = std::make_unique<PhysicsSystem>();
    m_physicsScene = std::make_unique<PhysicsScene>(m_physics.get());
    m_scene = std::make_unique<Scene>();

    GameStatics &gameStatics = GameStatics::Get();
    gameStatics.m_window = Window;
    gameStatics.m_renderer = m_renderer.get();
    gameStatics.m_physics = m_physics.get();
    gameStatics.m_physicsScene = m_physicsScene.get();
    gameStatics.m_scene = m_scene.get();
}

void Game::DestroyEssentials() {
    ZoneScoped;

    GameStatics &gameStatics = GameStatics::Get();
    gameStatics.m_window = nullptr;
    gameStatics.m_renderer = nullptr;
    gameStatics.m_physics = nullptr;
    gameStatics.m_physicsScene = nullptr;
    gameStatics.m_scene = nullptr;

    m_scene.reset();
    m_physicsScene.reset();
    m_physics.reset();
    m_renderer.reset();
}
