#include "tpch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Sprite.h"
#include "GameObject.h"
#include "GameObjectManager.h"

#include "Shape.h"
extern GameObjectManager* gpGameObjectManager;

Rigidbody::Rigidbody()
    : Component(CompoentType::RIGIDBODY)
{
	m_Shape = nullptr;
}

Rigidbody::~Rigidbody()
{
    if(m_Shape)
        delete m_Shape;
}

void Rigidbody::Initialize()
{
}

void Rigidbody::Integrate(float Gravity, float DeltaTime)
{
}

void Rigidbody::HandleObjectEvent(ObjectEventType eventType)
{
	if (eventType == ObjectEventType::COLLIDE)
	{
		TITAN_INFO("Rigidbody : Trigger collide event!");

		//TODO: Hidden Sprite, destroy object 
		if(!m_Owner->Destroyed)
			gpGameObjectManager->DestroyGameObject(m_Owner->GetGameID());
	}
}

void Rigidbody::AddShape(Shape* shape)
{
	m_Shape = shape;
}

void Rigidbody::Serialize(FILE** fpp)
{
    char shapeType[256];
    memset(shapeType, 0, 256 * sizeof(char));
    
    fscanf_s(*fpp, "%255s\n", shapeType, sizeof(shapeType));
    
    if(0 == strcmp(shapeType, "Circle"))
    {
		m_Shape = new ShapeCircle();
		m_Shape->m_OwnerRigidbody = this;
        
        ShapeCircle* pCircle = static_cast<ShapeCircle*>(m_Shape);
        fscanf_s(*fpp, "%f\n", &pCircle->m_Radius);
    }
    else if(0 == strcmp(shapeType, "AABB"))
    {
		m_Shape = new ShapeAABB();
		m_Shape->m_OwnerRigidbody = this;

        ShapeAABB* pAABB = static_cast<ShapeAABB*>(m_Shape);
		fscanf_s(*fpp, "%f %f\n", &pAABB->m_Width, &pAABB->m_Height);
    }
}

