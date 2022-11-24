//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/render/Renderer.h>
#include <haru/system/Window.h>

#include "Scene.h"

class Game final : public App {
public:
    void Init() override;

    void Shutdown() override;

    void Resize(const glm::ivec2 &size) override;

    void Update(float deltaTime) override;

    void Draw() override;

private:
    void CreateEssentials();

    void DestroyEssentials();

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<PhysicsSystem> m_physics;
    std::unique_ptr<PhysicsScene> m_physicsScene;
    std::unique_ptr<Scene> m_scene;

    float m_timeScale = 1.0f;
};
