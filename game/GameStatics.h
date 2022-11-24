//
// Created by andyroiiid on 11/24/2022.
//

#pragma once

class Window;

class Renderer;

class PhysicsSystem;

class PhysicsScene;

class Scene;


class GameStatics {
public:
    static GameStatics &Get() {
        static GameStatics singleton;
        return singleton;
    }

    static Window *GetWindow() { return Get().m_window; }

    static Renderer *GetRenderer() { return Get().m_renderer; }

    static PhysicsSystem *GetPhysicsSystem() { return Get().m_physics; }

    static PhysicsScene *GetPhysicsScene() { return Get().m_physicsScene; }

    static Scene *GetScene() { return Get().m_scene; }

private:
    GameStatics() = default;

    friend class Game;

    Window *m_window = nullptr;
    Renderer *m_renderer = nullptr;
    PhysicsSystem *m_physics = nullptr;
    PhysicsScene *m_physicsScene = nullptr;
    Scene *m_scene = nullptr;
};
