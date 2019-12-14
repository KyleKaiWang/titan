#pragma once
#include "GameSystem/Controller.h"

class GameObject;

class PlayerController : public Controller
{
public:
	PlayerController();
	~PlayerController();
	
	void Update(float timeStep);
	virtual void Serialize(FILE** fpp) {}
    void Initialize() {}
    void HandleObjectEvent(ObjectEventType eventType);

	float m_NextShootTime;
	float m_FireRate;
private:
	bool m_BounceBullet = false;
};

