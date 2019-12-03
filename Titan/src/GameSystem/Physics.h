#pragma once
#include "Component.h"
#include "Shape.h"

#include <list>

class Rigidbody;
class Shape;

class Physics
{
public:
	Physics();
    ~Physics();
    
    void Update(float frameTime);
};

class Contact
{
public:
	Contact();
	~Contact() {}
public:
	Rigidbody* m_Rigibodies[2];
};

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();
		
	void Reset();
	bool CheckCollisionAndGenerateContact(Shape* pShape1, float Pos1X, float Pos1Y, Shape* pShape2, float Pos2X, float Pos2Y);
public:
	std::list<Contact*> m_Contacts;
	bool(*CollisionFunctions[Shape::NUM][Shape::NUM]) (Shape* pShape1, float Pos1X, float Pos1Y, Shape* pShape2, float Pos2X, float Pos2Y, std::list<Contact*>& Contacts);
};

class CollideEvent : public ObjectEvent
{
public: 
	CollideEvent() { SetObjectEventType(OBJECT_EVENT_TYPE::COLLIDE); }
	CollideEvent(OBJECT_EVENT_TYPE type) { SetObjectEventType(type); }
	~CollideEvent() {}
};