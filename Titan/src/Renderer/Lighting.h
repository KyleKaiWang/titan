#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Titan {

	struct Light
	{
		glm::vec3 Position                 = glm::vec3(5, 2, 5);
		glm::vec3 Direction                = glm::vec3(-1,-1,-1);

		glm::mat4 ProjectionMatrix         = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -50.0f, 1000.0f);
		//glm::mat4 ProjectionMatrix         = glm::perspective(glm::radians(45.0f), (float)1024 / (float)1024, 0.1f, 100.0f);
		glm::mat4 ViewMatrix               = glm::lookAt(Position, (Position + 10.f * Direction), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 SpaceMatrix              = ProjectionMatrix * ViewMatrix;
		
		glm::vec3 Intensity                = glm::vec3(1.0f); 
	};
}