#pragma once
#include <vector>
#include "Component.h"
#include "Controller.h"
#include "Transform.h"
#include "Sprite.h"

enum class COMPONENT_TYPE;

class GameObject
{
public:
     GameObject();
     ~GameObject();

    virtual void Serialize(FILE **fpp) {}
    void Initialize();
	void Update();

    Component *GetComponent(COMPONENT_TYPE Type);
    Component *AddComponent(COMPONENT_TYPE Type);
    std::vector<Component*> m_Components;
    
    void HandleObjectEvent(ObjectEvent* pObjectEvent);
};
