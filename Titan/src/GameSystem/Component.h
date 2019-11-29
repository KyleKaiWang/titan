#pragma once
#include <stdio.h>
class ObjectEvent;

enum class COMPONENT_TYPE
{
    TRANSFORM,
    SPRITE,
    CONTROLLER,
    UP_DOWN,
    BODY
};

class GameObject;

class Component
{
public:
    Component(COMPONENT_TYPE Type);
    virtual ~Component() { }

    virtual void Update() = 0;
    virtual void Initialize() = 0;
    virtual void Serialize(FILE **fpp) = 0;
    GameObject* mpOwner;

    COMPONENT_TYPE GetType() const { return mType; }
    virtual void HandleObjectEvent(ObjectEvent* pObjectEvent) {}

private:
    COMPONENT_TYPE mType;
};
