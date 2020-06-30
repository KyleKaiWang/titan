#pragma once
#include "Renderer/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Titan {

	struct Light
	{
		glm::vec3 Position              = glm::vec3(5, 5, 5);
		glm::vec3 Direction             = glm::vec3(-1, -0.1, -1);
		glm::vec3 Ambient				= glm::vec3(0.3f);
		glm::vec3 Diffuse				= glm::vec3(0.8f);
		glm::vec3 Specular				= glm::vec3(0.2f);

		//Orthographic 
		glm::mat4 ProjectionMatrix      = glm::ortho(50.0f, -50.0f, 50.0f, -50.0f, 30.0f, -30.0f);
		glm::mat4 ViewMatrix            = glm::lookAt(Direction, Position + Direction, glm::vec3(0.0, 1.0, 0.0));

		//Perspective
		//glm::mat4 ProjectionMatrix      = glm::perspective(glm::radians(60.f), (float)1280 / (float)720, 0.0f, 100.0f);
		//glm::mat4 ViewMatrix			  = glm::lookAt(Direction, Position + Direction, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 SpaceMatrix           = ProjectionMatrix * ViewMatrix;
		glm::vec3 Attenuation           = glm::vec3(1.0f);
		glm::vec3 Intensity             = glm::vec3(1.0f); 
		float CutOff					= glm::radians(5.5f);
		float OuterCutOff				= glm::radians(12.5f);

		void ShaderBinding(const std::shared_ptr<Shader>& shader) {
			shader->SetMat4("u_LightSpaceMatrix", SpaceMatrix);
			shader->SetFloat3("u_LightPos", Position);
			shader->SetFloat3("u_LightDir", Direction);
			shader->SetFloat3("u_LightAmbient", Ambient);
			shader->SetFloat3("u_LightDiffuse", Diffuse);
			shader->SetFloat3("u_LightSpecular", Specular);
			shader->SetFloat3("u_LightAttenuation", Attenuation);
			shader->SetFloat("u_LightCutOff", CutOff);
			shader->SetFloat("u_LightOuterCutOff", OuterCutOff);
		}
	};

	class PointLight : public Light
	{
	public:
		float constant = 1.0f;
		float Linear = 1.0f;
		float Quadratic = 0.7f;
		float Radius = 1.8f;
	};
}