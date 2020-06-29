#include "tpch.h"
#include "Scene.h"
#include "Renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Titan {

	unsigned int Scene::CounterID = 0;
	
	Scene::Scene()
	{
		m_Root.reset(new SceneNode(Scene::CounterID++));
		m_CameraController = nullptr;
	}

	void Scene::InitScene()
	{
		if (!m_Root)
			TITAN_CORE_ASSERT(m_Root, "Root node doesn't exit!");
		
		//Init child
		std::for_each(m_Root->m_Children.begin(), m_Root->m_Children.end(), [](SceneNode* node)
		{
			node->Init();
		});
	}

	void Scene::Update(float ts)
	{
		
	}

	void Scene::PreRender()
	{
		Titan::Renderer::BeginScene(m_CameraController->GetCamera());
	}

	void Scene::Render()
	{
		if (!m_Root)
			return;
		m_Root->RenderChildren(this);
	}

	void Scene::PostRender()
	{
		Titan::Renderer::EndScene();
	}

	SceneNode* Scene::MakeSceneNode()
	{
		SceneNode* newNode = new SceneNode(Scene::CounterID++);
		newNode->SetMaterial(std::make_shared<Material>());
		newNode->SetMesh(std::make_shared<Mesh>());

		return newNode;
	}

	void Scene::AddSceneNode(SceneNode* node)
	{
		if(node->m_ID >= 0)
			m_Root->AddChild(node);
	}

	SceneNode::SceneNode()
	{
		m_Material = std::make_shared<Material>();
		m_Mesh = std::make_shared<Mesh>();
	}

	SceneNode::SceneNode(unsigned int id)
		: m_ID(id)
	{
		m_Material = std::make_shared<Material>();
		m_Mesh = std::make_shared<Mesh>();
	}
	SceneNode::~SceneNode()
	{
		for (unsigned int i = 0; i < m_Children.size(); ++i)
		{
			delete m_Children[i];
		}
	}

	void SceneNode::AddChild(SceneNode* node)
	{
		if (node->m_Parent)
		{
			node->m_Parent->RemoveChild(node->GetSceneNodeID());
		}
		node->m_Parent = this;
		m_Children.push_back(node);
	}

	void SceneNode::RemoveChild(unsigned int id)
	{
		auto it = std::find(m_Children.begin(), m_Children.end(), GetChild(id));
		if (it != m_Children.end())
			m_Children.erase(it);
	}

	SceneNode* SceneNode::GetChild(unsigned int id)
	{
		for (auto i = 0; i < m_Children.size(); ++i)
		{
			if (m_Children[i]->GetSceneNodeID() == id)
				return m_Children[i];
		}
		return nullptr;
	}

	void SceneNode::Init()
	{
		m_Mesh->Init();
		for (auto child : m_Children)
			child->Init();
	}

	void SceneNode::Render(Scene* scene)
	{
		//TODO: Fix shader submitting
		//Renderer::Submit(m_Material->GetShader(), m_Mesh->GetMeshVertexArray(), m_Transform);
	}

	void SceneNode::RenderChildren(Scene* scene)
	{
		auto i = m_Children.begin();
		auto end = m_Children.end();
		if (m_Children.size() == 1)
		{
			(*i)->Render(scene);
			(*i)->RenderChildren(scene);
		}
		else 
		{
			while (i != end)
			{
				(*i)->Render(scene);
				(*i)->RenderChildren(scene);
			}
		}
	}

	void SceneNode::UpdateTransform()
	{
		m_Transform = glm::translate(m_Transform, m_Position);
		m_Transform = glm::scale(m_Transform, m_Scale);
		m_Transform = glm::rotate(m_Transform, glm::radians(m_RotationDegree), m_Rotation);
		if (m_Parent)
		{
			m_Transform = m_Parent->m_Transform * m_Transform;
		}
	}
	
	
	
}