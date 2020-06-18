#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Titan {

	struct Light
	{
		glm::vec3 Position                 = glm::vec3(5, 5, 5);
		glm::vec3 Direction                = glm::vec3(-1, -0.1, -1);

		//glm::mat4 ProjectionMatrix         = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
		glm::mat4 ProjectionMatrix         = glm::perspective(glm::radians(60.f), (float)1280 / (float)720, 0.1f, 100.0f);
		glm::mat4 ViewMatrix               = glm::lookAt(Position, (Position + 100.f * Direction), glm::vec3(0.0, 1.0, 0.0));
		//glm::mat4 ViewMatrix				= glm::lookAt(Position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 SpaceMatrix              = ProjectionMatrix * ViewMatrix;
		
		glm::vec3 Intensity                = glm::vec3(1.0f); 
	};
}