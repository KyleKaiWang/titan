#pragma once
#include "GameSystem/GameObject.h"

enum FireType
{
	Regular = 0, RegularBounce, DualShot, DualShotBounce, TriShot
};

class AI : public GameObject
{
public:
	AI();
	~AI();

	virtual void Initialize();
	virtual void Update(float timeStep);
	virtual void HandleObjectEvent(ObjectEventType eventType);
	virtual void Serialize(FILE** fpp);

	void Fire();
	void SetBulletTrajectory(GameObject* bullet, glm::vec2 dir, float speed);
	
	inline void SetCanFire(bool canFire) { m_CanFire = canFire; }
	inline bool GetCanFire() const { return m_CanFire; }

	inline void SetFireRate(float fireRate) { m_FireRate = fireRate; }

	int AI_Category = 0;
private:
	void DoFire(FireType type);

private:
	bool m_CanFire = false;
	float m_FireRate;
	float m_NextFireTime;
	
};