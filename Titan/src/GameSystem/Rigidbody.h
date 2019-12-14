#pragma once
#include "Component.h"
class Shape;
class Rigidbody : public Component
{
public:
    Rigidbody();
    ~Rigidbody();
    
    void Update(float frameTime) {}
    void Serialize(FILE** fpp);
    void Initialize();
    void Integrate(float gravity, float deltaTime);
	void HandleObjectEvent(ObjectEventType eventType);

	void AddShape(Shape* shape);
    
public:
  /*  float mPosX, mPosY;
    float mPrevPosX, mPrevPosY;
    float mVelX,mVelY;
    float mAccX, mAccY;
    float mTotalForceX, mTotalForceY;
    float mMass, mInvMass;*/
	bool Immune = false;
	bool CanBounce = false;
	bool IgnoreSameTag = true;
	bool DrawDebugCollision = false;
    Shape* m_Shape;
};

