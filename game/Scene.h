//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include <memory>
#include <vector>

#include "Actor.h"

class Scene {
public:
    MOVE_ONLY(Scene)

    Scene() = default;

    ~Scene() = default;

    void Update(float deltaTime);

    void FixedUpdate(float fixedDeltaTime);

    void Draw(Renderer &renderer);

    template<class T, class... Args, std::enable_if_t<!std::is_array_v<T>, int>  = 0>
    T *CreateActor(Args &&... args) {
        std::unique_ptr<T> actor = std::make_unique<T>(std::forward<Args>(args)...);
        T *actorRef = actor.get();
        m_pendingCreationActors.emplace_back(std::move(actor));
        return actorRef;
    }

    template<class T>
    [[nodiscard]] T *FindFirstActorOfClass() const {
        return reinterpret_cast<T *>(FindFirstActorOfClassImpl(T::ClassName));
    }

private:
    [[nodiscard]] Actor *FindFirstActorOfClassImpl(const std::string &className) const;

    std::vector<std::unique_ptr<Actor>> m_actors;
    std::vector<std::unique_ptr<Actor>> m_pendingCreationActors;
    std::vector<std::unique_ptr<Actor>> m_pendingDestroyActors;
};
