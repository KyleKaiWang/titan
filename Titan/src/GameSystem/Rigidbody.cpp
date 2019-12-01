#include "Rigidbody.h"
#include "Transform.h"
#include "GameObject.h"
//#include "CollisionManager.h"

#include "tpch.h"
#include "Shape.h"

Rigidbody::Rigidbody()
    : Component(COMPONENT_TYPE::RIGIDBODY)
{
    /*mPosX = 0;
    mPosY = 0;
    mPrevPosX = mPrevPosY = 0.0f;
    mVelX = mVelY = 0.0f;
    mAccX = mAccY = 0.0f;
    mTotalForceX =  mTotalForceY = 0.0f;
    mMass = mInvMass= 0.0f;*/
    
	m_Shape = nullptr;
}

Rigidbody::~Rigidbody()
{
    if(m_Shape)
        delete m_Shape;
}

void Rigidbody::Initialize()
{
    /*Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(COMPONENT_TYPE::TRANSFORM));
    if(pTransform)
        mPrevPosX = mPosX = pTransform->mPosX;
        mPrevPosY = mPosY = pTransform->mPosY;*/
}

void Rigidbody::Integrate(float Gravity, float DeltaTime)
{
		//mPrevPosX = mPosX;
		//mPrevPosY = mPosY;

		//mTotalForceY += Gravity;

		////Acceleration
		//mAccX = mTotalForceX * mInvMass;
		//mAccY = mTotalForceY * mInvMass;

		////Velocity
		//mVelX = mAccX * DeltaTime + mVelX;
		//mVelY = mAccY * DeltaTime + mVelY;

		//mPosX = mVelX * DeltaTime + mPosX;
		//mPosY = mVelY * DeltaTime + mPosY;

		////Clear Force
		//mTotalForceX = mTotalForceY = 0.0f;

		////Update the owner's transform component
		//Transform* pTransform = static_cast<Transform*>(m_Owner->GetComponent(COMPONENT_TYPE::TRANSFORM));

		//if (pTransform)
		//{
		//	pTransform->m_PosX = mPosX;
		//	pTransform->m_PosY = mPosY;
		//}
	
}

void Rigidbody::AddShape(Shape* shape)
{
	m_Shape = shape;
}

void Rigidbody::Serialize(FILE** fpp)
{
    /*float mMass = 0;
    fscanf_s(*fpp, "%f\n", &mMass);
    
    if(mMass != 0.0f)
        mInvMass = 1.0f / mMass;
    else
        mInvMass = 0.0f;*/
    
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

