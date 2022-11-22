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
    const std::string &GetActorClassName() const override { \
        static const std::string s_ClassName = #className; \
        return s_ClassName; \
    }

template<class T>
struct ActorClassNameTraits;

#define REGISTER_ACTOR_CLASS(className) \
    template <> struct ActorClassNameTraits<className> { \
        static inline const std::string &value = #className; \
    };

class Actor {
public:
    MOVE_ONLY(Actor)

    Actor() = default;

    virtual ~Actor() = default;

    virtual const std::string &GetActorClassName() const = 0;

    template<class T>
    bool IsClass() const {
        return ActorClassNameTraits<T>::value == GetActorClassName();
    }

    virtual void Awake() = 0;

    virtual void Cleanup() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Draw(Renderer &renderer) = 0;

    bool IsPendingDestroy() const { return m_pendingDestroy; }

    void Destroy() { m_pendingDestroy = true; }

    const Transform &GetTransform() const { return m_transform; }

protected:
    Transform &GetTransform() { return m_transform; }

private:
    bool m_pendingDestroy = false;
    Transform m_transform;
};
