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

	class Sphere : public Mesh
	{
	public:
		Sphere(unsigned int xSegments = 50, unsigned int ySegments = 50);
	};

	class Torus : public Mesh
	{
	public:
		Torus(float r1 = 0.7f, float r2 = 0.3f, uint32_t numSteps1 = 30, uint32_t numSteps2 = 30);
	};

	class Skybox : public Mesh
	{
	public:
		Skybox(float size = 50.0f);
	};

	class Plane : public Mesh
	{
	public:
		Plane(float xsize, float zsize, int xdivs, int zdivs, float smax = 1.0f, float tmax = 1.0f);
	};
}