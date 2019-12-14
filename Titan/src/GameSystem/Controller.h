#pragma once
#include "Component.h"

class GameObject;

class Controller : public Component
{
public:
	Controller();
	~Controller();
	
	void Update(float timeStep);
	virtual void Serialize(FILE** fpp) {}
    void Initialize() {}
    void HandleObjectEvent(ObjectEventType eventType);

};

