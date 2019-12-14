#include "SandboxGameObjectFactory.h"
#include "GameSystem/GameObjectManager.h"
#include "GameSystem/Transform.h"
#include "GameSystem/Sprite.h"
#include "GameSystem/Rigidbody.h"
#include "PlayerController.h"

#include "AI.h"
#include "Bullet.h"

extern GameObjectManager* gpGameObjectManager;

SandboxGameObjectFactory::SandboxGameObjectFactory()
{
}

SandboxGameObjectFactory::~SandboxGameObjectFactory()
{
}

GameObject* SandboxGameObjectFactory::LoadObjectFromFile(const char* pFileName)
{
	//GameObjectFactory::LoadObjectFromFile(pFileName);
	
	GameObject* pNewObject = nullptr;
	FILE* fp;
	fopen_s(&fp, pFileName, "r");
	if (fp)
	{
		while (!feof(fp))
		{
			char stringBuffer[256];
			memset(stringBuffer, 0, sizeof(char));
			fscanf_s(fp, "%255s", stringBuffer, sizeof(stringBuffer));

			//Check Class Type
			if (0 == strcmp(stringBuffer, "ClassType"))
			{
				char className[256];
				memset(className, 0, 256 * sizeof(char));
				fscanf_s(fp, "%255s", className, sizeof(className));
			
				if (0 == strcmp(className, "GameObject"))
				{
					pNewObject = new GameObject();
				}
				else if (0 == strcmp(className, "AI"))
				{
					pNewObject = new AI();
					pNewObject->Serialize(&fp);
				}
				else if (0 == strcmp(className, "Bullet"))
				{
					pNewObject = new Bullet();
				}
			}

			//Check Components
			Component* pNewComponent = nullptr;
			if (0 == strcmp(stringBuffer, "Transform"))
				pNewComponent = ComponentTypeHandler(CompoentType::TRANSFORM);
			else if (0 == strcmp(stringBuffer, "Sprite"))
				pNewComponent = ComponentTypeHandler(CompoentType::SPRITE);
			else if (0 == strcmp(stringBuffer, "Controller"))
				pNewComponent = ComponentTypeHandler(CompoentType::CONTROLLER);
			else if (0 == strcmp(stringBuffer, "Rigidbody"))
				pNewComponent = ComponentTypeHandler(CompoentType::RIGIDBODY);

			if (!pNewObject)
				return nullptr;
				
			if (pNewComponent)
			{
				pNewObject->AddComponent(pNewComponent);
				pNewComponent->Serialize(&fp);
			}

			//Check Tag
			if (0 == strcmp(stringBuffer, "Tag"))
			{
				char tagName[256];
				memset(tagName, 0, 256 * sizeof(char));

				fscanf_s(fp, "%255s\n", tagName, sizeof(tagName));

				pNewObject->SetTag(tagName);
			}
		}
		fclose(fp);
			
		if (!pNewObject)
			return nullptr;
		else
		{
			pNewObject->SetGameID(++gpGameObjectManager->s_NextGameID);
			pNewObject->Initialize();
			gpGameObjectManager->m_GameObjects.push_back(pNewObject);

			return pNewObject;
		}
	}
	return nullptr;
}

GameObject* SandboxGameObjectFactory::CreateGameObject(GameObjectClass type)
{
	GameObject* pNewGameObject = nullptr;
	
	switch (type)
	{
	case GameObjectClass::Bullet:
	{
		pNewGameObject = LoadObjectFromFile("../Sandbox/assets/data/Bullet.txt");
		break;
	}
	default:
		break;
	}
	return pNewGameObject;
}

Component* SandboxGameObjectFactory::ComponentTypeHandler(CompoentType componentType)
{
	Component* pComponent = nullptr;

	switch (componentType)
	{
	case CompoentType::CONTROLLER:
	{
		pComponent = new PlayerController();
		break;
	}
	case CompoentType::TRANSFORM:
	{
		pComponent = new Transform();
		break;
	}
	case CompoentType::SPRITE:
	{
		pComponent = new Sprite();
		break;
	}
	case CompoentType::RIGIDBODY:
	{
		pComponent = new Rigidbody();
		break;
	}
	}
	return pComponent;
}

