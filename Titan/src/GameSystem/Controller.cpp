#include "tpch.h"
#include "Controller.h"
//#include "FrameRateController.h"
#include "GameObject.h"
#include "Transform.h"
//#include "Body.h"
//#include "CollisionManager.h"
//#include "EventManager.h"

#include "Input.h"
#include "KeyCodes.h"

//extern EventManager* gpEventManager;

Controller::Controller()
	:Component(COMPONENT_TYPE::CONTROLLER)
{

}

Controller::~Controller()
{

}

void Controller::Update()
{
	if (!mpOwner)
		return;

	Transform* pTransform = static_cast<Transform*>(mpOwner->GetComponent(COMPONENT_TYPE::TRANSFORM));
	if (!pTransform)
		return;

	if (Titan::Input::IsKeyPressed(TITAN_KEY_UP) || Titan::Input::IsKeyPressed(TITAN_KEY_W))
	{
		pTransform->m_PosY += 0.1;
	}
	else if (Titan::Input::IsKeyPressed(TITAN_KEY_DOWN) || Titan::Input::IsKeyPressed(TITAN_KEY_S))
	{
		pTransform->m_PosY -= 0.1;
	}
	else if(Titan::Input::IsKeyPressed(TITAN_KEY_RIGHT) || Titan::Input::IsKeyPressed(TITAN_KEY_D))
	{
		pTransform->m_PosX += 0.1;
	}
	else if (Titan::Input::IsKeyPressed(TITAN_KEY_LEFT) || Titan::Input::IsKeyPressed(TITAN_KEY_A))
	{
		pTransform->m_PosX -= 0.1;
	}
}

//void Controller::HandleObjectEvent(ObjectEvent* pObjectEvent)
//{
//    printf("I HAVE CALLED!!\n");
//    
//    if(pObjectEvent->mObjectEventType == OBJECT_EVENT_TYPE::COLLIDE)
//    {   
//        PlayerHitEvent *phe = new PlayerHitEvent();
//        gpEventManager->BroadCastEventToSubscribers(phe);
//        
//		LateEvent * pLateEvent = new LateEvent();
//		pLateEvent->mTimer = 1000.0f;
//		gpEventManager->AddTimedEvent(pLateEvent);
//    }
//}


//PlayerHitEvent::PlayerHitEvent(ObjectEvent* pEvent)
//{
//
//}
//
//PlayerHitEvent::~PlayerHitEvent()
//{
//    
//}
