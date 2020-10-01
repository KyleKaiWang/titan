#include "tpch.h"
#include "CameraController.h"

#include "Input.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Titan {

	CameraController::CameraController(float fov, float aspect, float near_clip, float far_clip)
		:m_AspectRatio(aspect), m_Camera(fov, aspect, near_clip, far_clip)
	{
	}

	void CameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(TITAN_KEY_RIGHT) || Input::IsKeyPressed(TITAN_KEY_D))
		{
			m_CameraPosition = m_CameraPosition + m_Camera.GetRight() * m_CameraMoveSpeed * (float)ts;
		}
		else if (Input::IsKeyPressed(TITAN_KEY_LEFT) || Input::IsKeyPressed(TITAN_KEY_A))
		{
			m_CameraPosition = m_CameraPosition - m_Camera.GetRight() * m_CameraMoveSpeed * (float)ts;
		}

		if (Input::IsKeyPressed(TITAN_KEY_DOWN) || Input::IsKeyPressed(TITAN_KEY_S))
		{
			m_CameraPosition = m_CameraPosition - m_Camera.GetFront() * m_CameraMoveSpeed * (float)ts;
		}
		else if (Input::IsKeyPressed(TITAN_KEY_UP) || Input::IsKeyPressed(TITAN_KEY_W))
		{
			m_CameraPosition = m_CameraPosition + m_Camera.GetFront() * m_CameraMoveSpeed * (float)ts;
		}

		if (m_Rotation)
		{
			if (Input::IsKeyPressed(TITAN_KEY_E))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(TITAN_KEY_Q))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetPerspectiveMatrix(glm::radians(m_ZoomLevel), m_AspectRatio, m_Camera.GetNear(), m_Camera.GetFar());
	}
	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseMovedEvent>(TITAN_BIND_EVENT_FUNC(CameraController::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(TITAN_BIND_EVENT_FUNC(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(TITAN_BIND_EVENT_FUNC(CameraController::OnWindowResized));

	}

	bool CameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		if (Input::IsMouseButtonPressed(TITAN_MOUSE_BUTTON_LEFT))
		{
			float xoffset = e.GetX() - m_MouseX;
			float yoffset = m_MouseY - e.GetY();
			m_MouseX = e.GetX();
			m_MouseY = e.GetY();

			float sensitivity = 0.05;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			m_Yaw += xoffset;
			m_Pitch += yoffset;

			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;

			glm::vec3 front;
			front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			front.y = sin(glm::radians(m_Pitch));
			front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			m_Camera.SetFront(glm::normalize(front));
		}
		m_MouseX = e.GetX();
		m_MouseY = e.GetY();
		return false;
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_CameraPosition.z -= e.GetYOffset();
		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetPerspectiveMatrix(glm::radians(m_AspectRatio * m_ZoomLevel), m_AspectRatio, m_Camera.GetNear(), m_Camera.GetFar());
		return false;
	}

}
