#pragma once
#include <glm/glm.hpp>

namespace Titan {

	class OpenGLDebugDraw
	{
	public:
		static void Init();
		static void End();

		// Adds a line segment to the debug drawing queue. 
		static void DrawLine(const glm::vec3& fromPosition, const glm::vec3& toPosition, glm::vec3 color, float lineWidth = 1.0f, float duration = 0.0f, bool depthEnabled = true);
	
		// Adds an axis-aligned cross (3 lines converging at // a point) to the debug drawing queue. 
		static void DrawCross(const glm::vec3& position, glm::vec3 color, float size, float duration = 0.0f, bool depthEnabled = true);
	
		// Adds a circle to the debug drawing queue. 
		static void DrawCircle(const glm::vec3& centerPosition, const glm::vec3& planeNormal, float radius, glm::vec3 color, float duration = 0.0f, bool depthEnabled = true);
		
		// Adds an axis-aligned bounding box to the debug queue. 
		static void DrawAABB(const glm::vec3& position, const glm::vec2& size, float rotation, glm::vec3& color, float lineWidth, float duration, bool depthEnabled);
	};
}


