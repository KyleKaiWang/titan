#include "tpch.h"
#include "Physics.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "GameObjectManager.h"
//#include "EventManager.h"

extern GameObjectManager* gpGameObjectManager;
CollisionManager* gpCollisionManager;
//extern EventManager *gpEventManager;

Physics::Physics()
{
	gpCollisionManager = new CollisionManager();
}

Physics::~Physics()
{
}

void Physics::Update(float frameTime)
{
    /*for(GameObject* pObj : gpGameObjectManager->m_GameObjects)
    {
        Rigidbody* pBody = (Rigidbody*)static_cast<Rigidbody*>(pObj->GetComponent(COMPONENT_TYPE::RIGIDBODY));
        if(pBody)
			pBody->Integrate(200.0f, frameTime);
    }*/
    
	gpCollisionManager->Reset();
    
    auto pObject1 = gpGameObjectManager->m_GameObjects.begin();
    auto pObjectLast = gpGameObjectManager->m_GameObjects.end();
    
    for(; pObject1 != pObjectLast; ++pObject1)
    {
        Rigidbody* pBody1 = static_cast<Rigidbody*>((*pObject1)->GetComponent(COMPONENT_TYPE::RIGIDBODY));
        if(!pBody1)
            continue;
        
        for(auto pObject2 = pObject1 + 1; pObject2 != pObjectLast; ++pObject2)
        {
            Rigidbody* pBody2 = static_cast<Rigidbody*>((*pObject2)->GetComponent(COMPONENT_TYPE::RIGIDBODY));
            
            if(!pBody2)
                continue;
			
			Transform* pTransform1 = static_cast<Transform*>(pBody1->m_Owner->GetComponent(COMPONENT_TYPE::TRANSFORM));
			Transform* pTransform2 = static_cast<Transform*>(pBody2->m_Owner->GetComponent(COMPONENT_TYPE::TRANSFORM));
			if (pTransform1 && pTransform2)
			{
				bool collide = gpCollisionManager->CheckCollisionAndGenerateContact(pBody1->m_Shape, pTransform1->m_PosX, pTransform1->m_PosY, pBody2->m_Shape, pTransform2->m_PosX, pTransform2->m_PosY);
				if (collide)
					TITAN_CORE_INFO("Collide! {0} {1} {2} {3}", "Obj1:", pBody1->m_Shape->m_Type, "Obj2", pBody2->m_Shape->m_Type);
			}
        }
    }

    /*for(auto pContact : gpCollisionManager->mContacts)
    {
        CollideEvent collideEvent;
        
        pContact->m_Rigidbody[0]->m_Owner->HandleEvent(&collideEvent);
        pContact->m_Rigidbody[1]->m_Owner->HandleEvent(&collideEvent);
    }*/
    
    //TO DO
    /*for(GameObject* pObj : gpGameObjectManager->mGameObjects)
    {
        Rigidbody* pBo = static_cast<Rigidbody*>(pObj->GetComponent(COMPONENT_TYPE::BODY));
        Transform* pTr = static_cast<Transform*>(pObj->GetComponent(COMPONENT_TYPE::TRANSFORM));
        if(pBo && pTr)
            pTr->mPosX = pBo->mPosX;
        pTr->mPosY = pBo->mPosY;
    }*/
}

bool CheckCollisionCircleCircle(Shape* pShape1, float PosX1, float PosY1, Shape* pShape2, float PosX2, float PosY2, std::list<Contact*>& Contacts)
{
	Contact* pContact = new Contact();
	pContact->m_Rigibodies[0] = pShape1->m_OwnerRigidbody;
	pContact->m_Rigibodies[1] = pShape2->m_OwnerRigidbody;
	Contacts.push_back(pContact);
	return true;
}

bool CheckCollisionCircleAABB(Shape* pShape1, float Pos1X, float Pos1Y, Shape* pShape2, float Pos2X, float Pos2Y, std::list<Contact*>& Contacts) {

	Contact* pNewContact = new Contact();
	pNewContact->m_Rigibodies[0] = pShape1->m_OwnerRigidbody;
	pNewContact->m_Rigibodies[1] = pShape2->m_OwnerRigidbody;
	Contacts.push_back(pNewContact);
	return false;
}

bool CheckCollisionAABBCircle(Shape* pShape1, float Pos1X, float Pos1Y, Shape* pShape2, float Pos2X, float Pos2Y, std::list<Contact*>& Contacts) {

	Contact* pNewContact = new Contact();
	pNewContact->m_Rigibodies[0] = pShape1->m_OwnerRigidbody;
	pNewContact->m_Rigibodies[1] = pShape2->m_OwnerRigidbody;
	Contacts.push_back(pNewContact);
	return false;
}

bool CheckCollisionAABBAABB(Shape* pShape1, float Pos1X, float Pos1Y, Shape* pShape2, float Pos2X, float Pos2Y, std::list<Contact*>& Contacts) {

	ShapeAABB* pS1 = static_cast<ShapeAABB*>(pShape1);
	ShapeAABB* pS2 = static_cast<ShapeAABB*>(pShape2);

	bool collision = !(Pos1X + pS1->m_Width / 2 <= Pos2X - pS2->m_Width / 2 || Pos1X - pS1->m_Width >= Pos2X + pS2->m_Width / 2 ||
		Pos1Y + pS1->m_Height / 2 <= Pos2Y - pS2->m_Height / 2 || Pos1Y - pS1->m_Height >= Pos2Y + pS2->m_Height / 2);

	if (collision)
	{
		Contact* pNewContact = new Contact();
		pNewContact->m_Rigibodies[0] = pShape1->m_OwnerRigidbody;
		pNewContact->m_Rigibodies[1] = pShape2->m_OwnerRigidbody;
		Contacts.push_back(pNewContact);
	}
	return collision;

	return false;
}

CollisionManager::CollisionManager()
{
	CollisionFunctions[(unsigned int)Shape::SHAPE_TYPE::CIRCLE][(unsigned int)Shape::SHAPE_TYPE::CIRCLE] = CheckCollisionCircleCircle;
	CollisionFunctions[(unsigned int)Shape::SHAPE_TYPE::CIRCLE][(unsigned int)Shape::SHAPE_TYPE::AABB] = CheckCollisionCircleAABB;
	CollisionFunctions[(unsigned int)Shape::SHAPE_TYPE::AABB][(unsigned int)Shape::SHAPE_TYPE::CIRCLE] = CheckCollisionAABBCircle;
	CollisionFunctions[(unsigned int)Shape::SHAPE_TYPE::AABB][(unsigned int)Shape::SHAPE_TYPE::AABB] = CheckCollisionAABBAABB;
}
CollisionManager::~CollisionManager()
{
	Reset();
}

void CollisionManager::Reset()
{
	for (auto contact : m_Contacts)
	{
		delete contact;
	}

	m_Contacts.clear();
}

bool CollisionManager::CheckCollisionAndGenerateContact(Shape* pShape1, float Pos1X, float Pos1Y, Shape* pShape2, float Pos2X, float Pos2Y)
{
	return CollisionFunctions[pShape1->m_Type][pShape2->m_Type](pShape1, Pos1X, Pos1Y, pShape2, Pos2X, Pos2Y, m_Contacts);
}

Contact::Contact()
{
	m_Rigibodies[0] = nullptr;
	m_Rigibodies[1] = nullptr;
}
