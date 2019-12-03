#pragma once
#include <stdio.h>
class ObjectEvent;

enum class COMPONENT_TYPE
{
    TRANSFORM,
    SPRITE,
    CONTROLLER,
    UP_DOWN,
    RIGIDBODY
};

class GameObject;

class Component
{
public:
    Component(COMPONENT_TYPE type);
    virtual ~Component() { }

    virtual void Update() = 0;
    virtual void Initialize() = 0;
    virtual void Serialize(FILE **fpp) = 0;
    GameObject* m_Owner;

    COMPONENT_TYPE GetType() const { return m_Type; }
    virtual void HandleObjectEvent(ObjectEvent* objectEvent) {}

private:
    COMPONENT_TYPE m_Type;
};
