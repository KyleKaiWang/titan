#pragma once
#include "Renderer/Material.h"
#include "Mesh/Mesh.h"
#include "Renderer/Camera.h"
#include "Renderer/CameraController.h"

namespace Titan {

	class Scene
	{
		friend class SceneNode;
	public:
		Scene();
		virtual ~Scene() {}

		virtual void InitScene();
		virtual void Update(float ts);
		virtual void PreRender();
		virtual void Render();
		virtual void PostRender();

		SceneNode* MakeSceneNode();
		void AddSceneNode(SceneNode* node);

		inline void SetCameraController(CameraController* cameraController) { m_CameraController = cameraController; }
		inline CameraController* GetCameraController() { return m_CameraController; }

	protected:
		std::shared_ptr<SceneNode> m_Root;
		CameraController* m_CameraController;
		static unsigned int CounterID;
	};

	class SceneNode
	{
		friend class Scene;

	public:
		SceneNode();
		SceneNode(unsigned int id);
		~SceneNode();

		void AddChild(SceneNode* node);
		void RemoveChild(unsigned int id);
		SceneNode* GetChild(unsigned int id);
		
		void SetSceneNodeID() { m_ID = Scene::CounterID++; }
		inline unsigned int GetSceneNodeID() const { return m_ID; }

		std::shared_ptr<Material> GetMaterial() const { return m_Material; }
		std::shared_ptr<Mesh> GetMesh() const { return m_Mesh; }
		void SetMaterial(std::shared_ptr<Material>& material) { m_Material = material; }
		void SetMesh(std::shared_ptr<Mesh>& mesh) { m_Mesh = mesh; }

		void Init();
		void Render(Scene* scene);
		void RenderChildren(Scene* scene);

		void UpdateTransform();
	private:
		unsigned int m_ID;
		std::shared_ptr<Material> m_Material;
		std::shared_ptr<Mesh> m_Mesh;

		std::vector<SceneNode*> m_Children;
		SceneNode* m_Parent = nullptr;

		glm::mat4 m_Transform = glm::mat4(1.0f);
		glm::mat4 m_PrevTransform = glm::mat4(1.0f);
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f);
		float m_RotationDegree = 0.0f;
		glm::vec3 m_Scale = glm::vec3(1.0f);
	};
}
