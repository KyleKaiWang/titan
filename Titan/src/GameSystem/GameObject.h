#pragma once
#include <vector>
#include <string>
#include "Component.h"
#include <glm/glm.hpp>

enum class CompoentType;

enum class ObjectEventType
{
	COLLIDE
};

class GameObject
{
public:
     GameObject();
     virtual ~GameObject();

    virtual void Serialize(FILE **fpp) {}
	virtual void Initialize();
	virtual void Update(float timeStep);

    Component* GetComponent(CompoentType Type);
    Component* AddComponent(CompoentType Type);
	void AddComponent(Component* component);
    std::vector<Component*> m_Components;
    
    virtual void HandleObjectEvent(ObjectEventType eventType);
	
	inline glm::vec3& GetGameObjectPosition() { return glm::vec3(GetPosX(), GetPosY(), 0.0f); }
	float GetPosX();
	float GetPosY();

	inline void SetGameID(int id) { m_GameObjectID = id; }
	inline int GetGameID() const { return m_GameObjectID; }
	inline void SetTag(std::string tagName) { m_Tag = tagName; }
	inline std::string GetTag() const { return m_Tag; }
	inline void SetLifeSpan(float lifeSpan) { m_HasLifeSpan = true;  m_LifeSpan = lifeSpan; }
	inline float GetLifeSpan() const { return m_LifeSpan; }
	void FlagDestroyed();
	bool Destroyed = false;
	//bool EnableCollisionDebug = false;

	//LifeSpan would be actived if it's not 0.0f
	bool m_HasLifeSpan = false;
	float m_LifeSpan;

private:
	int m_GameObjectID;
	std::string m_Tag;

};
