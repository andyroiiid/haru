//
// Created by andyroiiid on 11/21/2022.
//

#pragma once
#pragma once

#include <string>

#include <haru/core/MoveOnly.h>
#include <haru/math/Transform.h>
#include <haru/render/Renderer.h>

#define DEFINE_ACTOR_CLASS(className) \
    NO_MOVE_OR_COPY(className) \
    static inline const std::string &ClassName = #className; \
    const std::string &GetActorClassName() const override { return ClassName; }

class Actor {
public:
    NO_MOVE_OR_COPY(Actor)

    Actor() = default;

    virtual ~Actor() = default;

    virtual const std::string &GetActorClassName() const = 0;

    template<class T>
    bool IsClass() const {
        return T::ClassName == GetActorClassName();
    }

    virtual void Update(float deltaTime) {}

    virtual void FixedUpdate(float fixedDeltaTime) {}

    virtual void Draw(Renderer &renderer) {}

    bool IsPendingDestroy() const { return m_pendingDestroy; }

    void Destroy() { m_pendingDestroy = true; }

    const Transform &GetTransform() const { return m_transform; }

protected:
    Transform &GetTransform() { return m_transform; }

private:
    bool m_pendingDestroy = false;
    Transform m_transform;
};
