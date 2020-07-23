#pragma once

#include <glm/glm.hpp>

namespace Titan{
    
    class Camera
    {
    public:
        Camera() {}
        virtual ~Camera() {}
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(glm::vec3& position) { m_Position = position; UpdateViewMatrix(); }

        float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation) { m_Rotation = rotation; }

        virtual void UpdateViewMatrix() {};

    protected:
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;
    };

    class PerspectiveCamera : public Camera
    {
    public:
		PerspectiveCamera(float fov, float aspect, float near_clipping, float far_clipping);

		void SetPerspectiveMatrix(float fov, float aspect, float near_clipping, float far_clipping);

		const glm::vec3& GetFront() const { return m_Front; }
		void SetFront(glm::vec3& front) { m_Front = front; m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp)); m_Up = glm::normalize(glm::cross(m_Right, m_Front)); }
		const glm::vec3& GetRight() const { return m_Right; }

    private:
        void UpdateViewMatrix() override;
        
    private:
		glm::vec3 m_WorldUp = { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };

		float m_FOV;
		float m_AspectRatio;
		float m_NearClip;
		float m_FarClip;
    };

    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetOrthoProjection(float left, float right, float bottom, float top);

    private:
        void UpdateViewMatrix() override;
    };
}
