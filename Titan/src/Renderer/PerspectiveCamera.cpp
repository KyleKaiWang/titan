#include "tpch.h"
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Titan {
    
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near_clip, float far_clip)
        : m_ProjectionMatrix(glm::perspective(fov, aspect, near_clip, far_clip)), m_FOV(fov), m_AspectRatio(aspect), m_NearClip(near_clip), m_FarClip(far_clip)
    {
		//m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
		//m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
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
}