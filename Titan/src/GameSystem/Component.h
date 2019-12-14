#pragma once
#include <stdio.h>

class GameObject;
enum class ObjectEventType;

enum class CompoentType
{
    TRANSFORM,
    SPRITE,
    CONTROLLER,
    RIGIDBODY
};

class Component
{
public:
    Component(CompoentType componentType);
    virtual ~Component() { }

    virtual void Update(float frameTime) = 0;
    virtual void Initialize() = 0;
    virtual void Serialize(FILE **fpp) = 0;
    GameObject* m_Owner;

    CompoentType GetType() const { return m_Type; }
    virtual void HandleObjectEvent(ObjectEventType eventType) {}

private:
    CompoentType m_Type;
};
