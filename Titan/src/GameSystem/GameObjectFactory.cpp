#include "tpch.h"
#include "GameObjectFactory.h"
#include "GameObjectManager.h"

#include "Transform.h"
#include "Sprite.h"
#include "Controller.h"
#include "Rigidbody.h"

#include <unordered_map>

extern GameObjectManager* gpGameObjectManager;

GameObjectFactory::GameObjectFactory()
{
}

GameObjectFactory::~GameObjectFactory()
{
}

GameObject* GameObjectFactory::LoadObjectFromFile(const char* pFileName)
{
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
			//if (0 == strcmp(stringBuffer, "ClassType"))
			//{
			//	char className[256];
			//	memset(className, 0, 256 * sizeof(char));
			//	fscanf_s(fp, "%255s", className, sizeof(className));
			//
			//	if (0 == strcmp(className, "GameObject"))
			//	{
			//		pNewObject = new GameObject();
			//	}
			//	else if (0 == strcmp(className, "AI"))
			//	{
			//		pNewObject = new AI();
			//		pNewObject->Serialize(&fp);
			//	}
			//	else if (0 == strcmp(className, "Bullet"))
			//	{
			//		pNewObject = new Bullet();
			//	}
			//}

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

void GameObjectFactory::LoadLevelFromFile(const char* pFileName)
{
	FILE* fp;
	fopen_s(&fp, pFileName, "r");

	if (fp)
	{
		while (!feof(fp))
		{
			char archetypeName[256];
			memset(archetypeName, 0, sizeof(archetypeName));

			fscanf_s(fp, "%255s", archetypeName, sizeof(archetypeName));

			std::string fullpath = "../Sandbox/assets/data/";
			fullpath += archetypeName;

			GameObject* pNewGameObject = LoadObjectFromFile(fullpath.c_str());

			//Initialize each game object's first position by level data
			Transform* pTransform = static_cast<Transform*>(pNewGameObject->GetComponent(CompoentType::TRANSFORM));
			pTransform->Serialize(&fp);
           
		}
		fclose(fp);
	}
}

GameObject* GameObjectFactory::CreateGameObject(const char* pFileName)
{
	GameObject* pNewGameObject = LoadObjectFromFile(pFileName);
	return pNewGameObject;
}

Component* GameObjectFactory::ComponentTypeHandler(CompoentType componentType)
{
	Component* pComponent = nullptr;

	switch (componentType)
	{
		case CompoentType::CONTROLLER :
		{
			pComponent = new Controller();
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
