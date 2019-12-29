#pragma once
#include "Core/Timestep.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/PerspectiveCamera.h"

namespace Titan {
	
	class CameraController
	{
	public:
		CameraController(float fov, float aspect, float near_clip, float far_clip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		PerspectiveCamera& GetCamera() { return m_Camera; }
		const PerspectiveCamera& GetCamera() const { return m_Camera; }
		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }

		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		bool m_TextureMode = true;
	private:
		PerspectiveCamera m_Camera;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0, 5.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraMoveSpeed = 5.0f;
		float m_CameraRotationSpeed = 180.0f;
		float m_ZoomLevel = 45.0f;
		float m_AspectRatio;
		bool m_Rotation = false;

		float m_MouseX;
		float m_MouseY;
		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
	};
}