#pragma once
#include "Component.h"
#include "EventManager.h"
class Shape;
class Rigidbody : public Component
{
public:
    Rigidbody();
    ~Rigidbody();
    
    void Update() {}
    void Serialize(FILE** fpp);
    void Initialize();
    void Integrate(float gravity, float deltaTime);
	void HandleObjectEvent(ObjectEvent* objectEvent);

	void AddShape(Shape* shape);
    
public:
  /*  float mPosX, mPosY;
    float mPrevPosX, mPrevPosY;
    float mVelX,mVelY;
    float mAccX, mAccY;
    float mTotalForceX, mTotalForceY;
    float mMass, mInvMass;*/
    
    Shape* m_Shape;
};

