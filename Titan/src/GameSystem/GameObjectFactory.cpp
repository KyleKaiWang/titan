#include "tpch.h"
#include "GameObjectFactory.h"
#include "GameObjectManager.h"

extern GameObjectManager* gpGameObjectManager;

GameObjectFactory::GameObjectFactory()
{
}

GameObjectFactory::~GameObjectFactory()
{
}

GameObject* GameObjectFactory::LoadObject(const char* pFileName)
{
	GameObject* pNewObject = nullptr;
	FILE* fp;
	fopen_s(&fp, pFileName, "r");
	if (fp)
	{
		pNewObject = new GameObject();

		while (!feof(fp))
		{
			char componentType[256];
			memset(componentType, 0, sizeof(char));

			fscanf_s(fp, "%255s", componentType, sizeof(componentType));

			Component* pNewComponent = nullptr;
			if (0 == strcmp(componentType, "Transform"))
				pNewComponent = pNewObject->AddComponent(COMPONENT_TYPE::TRANSFORM);
			else if (0 == strcmp(componentType, "Sprite"))
				pNewComponent = pNewObject->AddComponent(COMPONENT_TYPE::SPRITE);
			/*else if (0 == strcmp(componentType, "UpDown"))
				pNewComponent = pNewObject->AddComponent(COMPONENT_TYPE::UP_DOWN);
			else if (0 == strcmp(componentType, "Controller"))
				pNewComponent = pNewObject->AddComponent(COMPONENT_TYPE::CONTROLLER);*/
            /*else if (0 == strcmp(componentType, "Body"))
                pNewComponent = pNewObject->AddComponent(COMPONENT_TYPE::BODY);*/

			if (nullptr != pNewComponent)
				pNewComponent->Serialize(&fp);
		}
		fclose(fp);
	}
	gpGameObjectManager->m_GameObjects.push_back(pNewObject);
	return pNewObject;
}

void GameObjectFactory::LoadLevel(const char* pFileName)
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

			GameObject* pNewGameObject = LoadObject(fullpath.c_str());

			Transform* pTransform = static_cast<Transform*>(pNewGameObject->GetComponent(COMPONENT_TYPE::TRANSFORM));
			pTransform->Serialize(&fp);
            pNewGameObject->Initialize();
		}
		fclose(fp);
	}
}
