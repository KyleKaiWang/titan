#include "tpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Titan {
    
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near_clip, float far_clip)
        : m_FOV(fov), m_AspectRatio(aspect), m_NearClip(near_clip), m_FarClip(far_clip)
    {
		SetPerspectiveMatrix(fov, aspect, near_clip, far_clip);
    }

	void PerspectiveCamera::SetPerspectiveMatrix(float fov, float aspect, float near_clip, float far_clip)
	{
		m_FOV = fov;
		m_AspectRatio = aspect;
		m_NearClip = near_clip;
		m_FarClip = far_clip;
		m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspect, near_clip, far_clip);
		UpdateViewMatrix();
	}

    void PerspectiveCamera::UpdateViewMatrix()
    {
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		SetOrthoProjection(left, right, bottom, top);
	}

	void OrthographicCamera::SetOrthoProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top);
		UpdateViewMatrix();
	}

	void OrthographicCamera::UpdateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}