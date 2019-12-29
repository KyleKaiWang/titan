#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Renderer/VertexArray.h"
#include "Renderer/Buffer.h"

namespace Titan {
	
	class Mesh
	{
	public:
		Mesh();
		Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents, std::vector<unsigned int> indices);
		virtual ~Mesh();

		virtual void Init();

		std::vector<glm::vec3> m_Positions;
		std::vector<glm::vec2> m_UV;
		std::vector<glm::vec3> m_Normals;

		std::vector<uint32_t> m_Indices;
		inline const std::shared_ptr<VertexArray>& GetMeshVertexArray() { return m_VertexArray; }

	protected:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

	class Cube : public Mesh
	{
	public:
		Cube(float _size = 1.0f);
	};
}