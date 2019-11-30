#pragma once
#include "Component.h"
//#include "EventManager.h"

class GameObject;

class Controller : public Component
{
public:
	Controller();
	~Controller();
	
	void Update();
	virtual void Serialize(FILE** fpp) {}
    void Initialize() {}
    //void HandleObjectEvent(ObjectEvent* pObjectEvent);
};

//class PlayerHitEvent : public ObjectEvent
//{
//public:
//	PlayerHitEvent() : ObjectEvent(OBJECT_EVENT_TYPE::PLAYER_HIT) {}
//	PlayerHitEvent(ObjectEvent* pObjectEvent);
//    ~PlayerHitEvent();
//};
