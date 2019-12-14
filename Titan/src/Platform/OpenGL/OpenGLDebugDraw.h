#pragma once
#include <glm/glm.hpp>
#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"
#include "OpenGLShader.h"

namespace Titan {
	
	enum DebugDrawType
	{
		Line,
		Circle,
		AABB
	};

	class OpenGLDebugDraw
	{
	public:
		OpenGLDebugDraw();
		~OpenGLDebugDraw();

		void Init();

		void DrawDebugList();

		// Adds a line segment to the debug drawing queue. 
		static void AddLine(const glm::vec3& fromPosition, const glm::vec3& toPosition, glm::vec3 color, float lineWidth = 1.0f, float duration = 0.0f, bool depthEnabled = true);
	
		// Adds an axis-aligned cross (3 lines converging at // a point) to the debug drawing queue. 
		static void AddCross(const glm::vec3& position, glm::vec3 color, float size, float duration = 0.0f, bool depthEnabled = true);
	
		// Adds a wireframe sphere to the debug drawing queue. 
		static void AddSphere(const glm::vec3& centerPosition, float radius, glm::vec3 color, float duration = 0.0f, bool depthEnabled = true);
	
		// Adds a circle to the debug drawing queue. 
		static void AddCircle(const glm::vec3& centerPosition, const glm::vec3& planeNormal, float radius, glm::vec3 color, float duration = 0.0f, bool depthEnabled = true);
		
		// Adds an axis-aligned bounding box to the debug queue. 
		static void AddAABB(const glm::vec3& position, const glm::vec2& size, float rotation, glm::vec3& color, float lineWidth, float duration, bool depthEnabled);

	private:
		static std::list<DebugDrawType> m_DebugDrawList;
		//Ref<VertexArray> m_DebugDrawVertexArray;
		//Ref<Shader> m_DebugDrawShader;
	};
}


