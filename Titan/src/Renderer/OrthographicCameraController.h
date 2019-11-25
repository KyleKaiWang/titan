#pragma once
#include "Renderer/OrthographicCamera.h"
#include "Core/Timestep.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

namespace Titan {
	
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		bool m_TextureMode = true;
	private:
		OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraMoveSpeed = 5.0f;
		float m_CameraRotationSpeed = 180.0f;
		float m_ZoomLevel = 1.0f;
		float m_AspectRatio;
		bool m_Rotation;

		
	};
}