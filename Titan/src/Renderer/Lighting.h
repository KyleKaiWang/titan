#pragma once
#include "Renderer/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Titan {

	struct Light
	{
		glm::vec3 Position              = glm::vec3(100);
		glm::vec3 Direction             = glm::vec3(1, -0.5, 0.5);
		glm::vec3 Ambient				= glm::vec3(0.8f);
		glm::vec3 Diffuse				= glm::vec3(100.0f);
		glm::vec3 Specular				= glm::vec3(0.2f);

		float zNear = 0.5f;
		float zFar = 1000.0f;

		virtual void UniformBinding(const std::shared_ptr<Shader>& shader)
		{
			shader->SetFloat3("u_LightPos", Position);
			shader->SetFloat3("u_LightDir", Direction);
			shader->SetFloat3("u_LightAmbient", Ambient);
			shader->SetFloat3("u_LightDiffuse", Diffuse);
			shader->SetFloat3("u_LightSpecular", Specular);
		}

		virtual void UpdateViewProjection() {}
	};
	
	struct DirectionalLight : public Light
	{
		float orthoLeft = -10.0f * 1.2f;
		float orthoRight = 10.0f * 1.2f;
		float orthoButtom = -10.0f;
		float orthoTop = 10.0f;

		glm::mat4 ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoButtom, orthoTop, zNear, zFar);
		glm::mat4 ViewMatrix = glm::lookAt(Position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		glm::mat4 SpaceMatrix = ProjectionMatrix * ViewMatrix;

		virtual void UniformBinding(const std::shared_ptr<Shader>& shader) override
		{
			shader->SetMat4("u_LightSpaceMatrix", SpaceMatrix);
			shader->SetFloat3("u_LightPos", Position);
			shader->SetFloat3("u_LightDir", Direction);
			shader->SetFloat3("u_LightAmbient", Ambient);
			shader->SetFloat3("u_LightDiffuse", Diffuse);
			shader->SetFloat3("u_LightSpecular", Specular);

			shader->SetFloat3("dirLight.direction", Direction);
			shader->SetFloat3("dirLight.color", Ambient);
			shader->SetFloat("u_FarPlane", zFar);
		}

		virtual void UpdateViewProjection() override
		{
			ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoButtom, orthoTop, zNear, zFar);
			ViewMatrix = glm::lookAt(Position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			SpaceMatrix = ProjectionMatrix * ViewMatrix;
		}
	};

	struct SpotLight : public Light
	{
		float Fov = glm::radians(60.f);
		float AspectRatio = (float)1280 / (float)720;
		
		glm::mat4 ProjectionMatrix = glm::perspective(Fov, AspectRatio, zNear, zFar);;
		glm::mat4 ViewMatrix = glm::lookAt(Position, Position + Direction, glm::vec3(0.0, 1.0, 0.0));

		glm::mat4 SpaceMatrix = ProjectionMatrix * ViewMatrix;

		virtual void UniformBinding(const std::shared_ptr<Shader>& shader) override
		{
			shader->SetMat4("u_LightSpaceMatrix", SpaceMatrix);
			shader->SetFloat3("u_LightPos", Position);
			shader->SetFloat3("u_LightDir", Direction);
			shader->SetFloat3("u_LightAmbient", Ambient);
			shader->SetFloat3("u_LightDiffuse", Diffuse);
			shader->SetFloat3("u_LightSpecular", Specular);
		}

		virtual void UpdateViewProjection() override
		{
			glm::mat4 ProjectionMatrix = glm::perspective(Fov, AspectRatio, zNear, zFar);
			glm::mat4 ViewMatrix = glm::lookAt(Position, Position + Direction, glm::vec3(0.0, 1.0, 0.0));
			SpaceMatrix = ProjectionMatrix * ViewMatrix;
		}
	};

	struct PointLight : public Light
	{
		float Linear = 1.0f;
		float Quadratic = 0.7f;
		float Radius = 1.8f;
		
		glm::mat4 LookAtPerFace[6];
		glm::mat4 ProjectionMatrix;

		virtual void UniformBinding(const std::shared_ptr<Shader>& shader) override
		{
			shader->SetFloat3("u_LightPos", Position);
			shader->SetFloat3("u_LightDir", Direction);
			shader->SetFloat3("u_LightAmbient", Ambient);
			shader->SetFloat3("u_LightDiffuse", Diffuse);
			shader->SetFloat3("u_LightSpecular", Specular);
		}
	};

	struct GPULight {
		glm::vec4 position;
		glm::vec4 color;
		unsigned int enabled;
		float intensity;
		float range;
		float padding;
	};
}