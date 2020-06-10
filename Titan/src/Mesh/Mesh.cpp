#include "tpch.h"
#include "Mesh.h"
#include <glm/gtc/constants.hpp>

const float TAU = 6.2831853071;

namespace Titan {

	Mesh::Mesh()
	{

	}

	Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents, std::vector<unsigned int> indices)
		:m_Positions(positions), m_UV(uv), m_Normals(normals), m_Indices(indices)
	{

	}

	Mesh::~Mesh()
	{

	}

	void Mesh::Init()
	{
		bool uv = false, normal = false;
		std::vector<float> data;
		for (int i = 0; i < m_Positions.size(); ++i)
		{
			data.push_back(m_Positions[i].x);
			data.push_back(m_Positions[i].y);
			data.push_back(m_Positions[i].z);
			
			if (m_UV.size() > 0)
			{
				uv = true;
				data.push_back(m_UV[i].x);
				data.push_back(m_UV[i].y);
			}
			if (m_Normals.size() > 0)
			{
				normal = true;
				data.push_back(m_Normals[i].x);
				data.push_back(m_Normals[i].y);
				data.push_back(m_Normals[i].z);
			}
		}

		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(&data[0], data.size() * sizeof(float));
		
		if (uv && normal)
		{
			m_VertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float3, "a_Normal" }
			});
		}
		else if (uv)
		{
			m_VertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			});
		}
		else if(normal)
		{
			m_VertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" }
			});
		}
		else
		{
			m_VertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" }
			});
		}
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		if (m_Indices.size() > 0)
		{
			m_IndexBuffer = Titan::IndexBuffer::Create(&m_Indices[0], m_Indices.size());
			m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		}
		m_VertexArray->Unbind();
	}

	Cube::Cube(float _size)
	{
		float size = _size / 2.0f;

		m_Positions = std::vector<glm::vec3>{
			// Front
			glm::vec3(-size, -size, size), 
			glm::vec3(size, -size, size), 
			glm::vec3(size,  size, size),  
			glm::vec3(-size,  size, size),
			// Right
			glm::vec3(size, -size, -size),
			glm::vec3(size, size, -size),
			glm::vec3(size,  size, size), 
			glm::vec3(size,  -size, size),
			// Back
			glm::vec3(-size, -size, -size), 
			glm::vec3(-size,  size, -size), 
			glm::vec3(size,  size, -size), 
			glm::vec3(size, -size, -size),
			// Left
			glm::vec3(-size, -size, size), 
			glm::vec3(-size,  size, size), 
			glm::vec3(-size,  size, -size), 
			glm::vec3(-size, -size, -size),
			// Bottom
			glm::vec3(-size, -size, size), 
			glm::vec3(-size, -size, -size), 
			glm::vec3(size, -size, -size), 
			glm::vec3(size, -size, size),
			// Top
			glm::vec3(-size,  size, size),
			glm::vec3(size,  size, size),
			glm::vec3(size,  size, -size),
			glm::vec3(-size,  size, -size)
		};

		m_UV = std::vector<glm::vec2>{
			// Front
			glm::vec2(0.0f, 0.0f), 
			glm::vec2(1.0f, 0.0f), 
			glm::vec2(1.0f, 1.0f), 
			glm::vec2(0.0f, 1.0f),
			// Right	
			glm::vec2(0.0f, 0.0f), 
			glm::vec2(1.0f, 0.0f), 
			glm::vec2(1.0f, 1.0f), 
			glm::vec2(0.0f, 1.0f),
			// Back	
			glm::vec2(0.0f, 0.0f), 
			glm::vec2(1.0f, 0.0f), 
			glm::vec2(1.0f, 1.0f), 
			glm::vec2(0.0f, 1.0f),
			// Left	
			glm::vec2(0.0f, 0.0f), 
			glm::vec2(1.0f, 0.0f), 
			glm::vec2(1.0f, 1.0f), 
			glm::vec2(0.0f, 1.0f),
			// Bottom	
			glm::vec2(0.0f, 0.0f), 
			glm::vec2(1.0f, 0.0f), 
			glm::vec2(1.0f, 1.0f), 
			glm::vec2(0.0f, 1.0f),
			// Top	
			glm::vec2(0.0f, 0.0f), 
			glm::vec2(1.0f, 0.0f), 
			glm::vec2(1.0f, 1.0f), 
			glm::vec2(0.0f, 1.0f)
		};

		m_Normals = std::vector<glm::vec3>{
			// Front
			glm::vec3(0.0f, 0.0f, 1.0f), 
			glm::vec3(0.0f, 0.0f, 1.0f), 
			glm::vec3(0.0f, 0.0f, 1.0f), 
			glm::vec3(0.0f, 0.0f, 1.0f),
			// Right		  
			glm::vec3(1.0f, 0.0f, 0.0f), 
			glm::vec3(1.0f, 0.0f, 0.0f), 
			glm::vec3(1.0f, 0.0f, 0.0f), 
			glm::vec3(1.0f, 0.0f, 0.0f),
			// Back		  
			glm::vec3(0.0f, 0.0f, -1.0f), 
			glm::vec3(0.0f, 0.0f, -1.0f), 
			glm::vec3(0.0f, 0.0f, -1.0f), 
			glm::vec3(0.0f, 0.0f, -1.0f),
			// Left		  			  
			glm::vec3(-1.0f, 0.0f, 0.0f), 
			glm::vec3(-1.0f, 0.0f, 0.0f), 
			glm::vec3(-1.0f, 0.0f, 0.0f), 
			glm::vec3(-1.0f, 0.0f, 0.0f),
			// Bottom		  		  
			glm::vec3(0.0f, -1.0f, 0.0f), 
			glm::vec3(0.0f, -1.0f, 0.0f), 
			glm::vec3(0.0f, -1.0f, 0.0f), 
			glm::vec3(0.0f, -1.0f, 0.0f),
			// Top		  
			glm::vec3(0.0f, 1.0f, 0.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f)
		};

		m_Indices = std::vector<uint32_t>{
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};
	}
	Torus::Torus(float r1, float r2, uint32_t numSteps1, uint32_t numSteps2)
	{
		m_Positions.resize((numSteps1 + 1) * (numSteps2 + 1));
		m_Normals.resize((numSteps1 + 1) * (numSteps2 + 1));
		m_UV.resize((numSteps1 + 1) * (numSteps2 + 1));

		std::vector<glm::vec3> p(numSteps1 + 1);
		float a = 0.0f;
		float step = 2.0f * glm::pi<float>() / numSteps1;
		for (int i = 0; i <= numSteps1; ++i)
		{
			float x = cos(a) * r1;
			float y = sin(a) * r1;
			p[i].x = x;
			p[i].y = y;
			p[i].z = 0.0f;
			a += step;
		}

		for (int i = 0; i <= numSteps1; ++i)
		{
			glm::vec3 u = glm::normalize(glm::vec3(0.0f) - p[i]) * r2;     
			glm::vec3 v = glm::vec3(0.0f, 0.0f, 1.0f) * r2;

			// create the vertices of each minor ring segment:
			float a = 0.0f;
			float step = 2.0f * glm::pi<float>() / numSteps2;
			for (int j = 0; j <= numSteps2; ++j)
			{
				float c = cos(a);
				float s = sin(a);

				m_Positions[i * (numSteps2 + 1) + j] = p[i] + c * u + s * v;

				m_UV[i * (numSteps2 + 1) + j].x = ((float)i) / ((float)numSteps1) * TAU;
				m_UV[i * (numSteps2 + 1) + j].y = ((float)j) / ((float)numSteps2);
				m_Normals[i * (numSteps2 + 1) + j] = glm::normalize(c * u + s * v);
				a += step;
			}
		}

		m_Indices.resize(numSteps1 * numSteps2 * 6);

		int index = 0;
		for (int i = 0; i < numSteps1; ++i)
		{
			int i1 = i;
			int i2 = (i1 + 1);

			for (int j = 0; j < numSteps2; ++j)
			{
				int j1 = j;
				int j2 = (j1 + 1);

				m_Indices[index++] = i1 * (numSteps2 + 1) + j1;
				m_Indices[index++] = i1 * (numSteps2 + 1) + j2;
				m_Indices[index++] = i2 * (numSteps2 + 1) + j1;
				
				m_Indices[index++] = i2 * (numSteps2 + 1) + j2;
				m_Indices[index++] = i2 * (numSteps2 + 1) + j1;
				m_Indices[index++] = i1 * (numSteps2 + 1) + j2;
			}
		}
	}

	Sphere::Sphere(unsigned int xSegments, unsigned int ySegments)
	{
		for (unsigned int y = 0; y <= ySegments; ++y)
		{
			for (unsigned int x = 0; x <= xSegments; ++x)
			{
				float xSegment = (float)x / (float)xSegments;
				float ySegment = (float)y / (float)ySegments;
				float xPos = std::cos(xSegment * TAU) * std::sin(ySegment * glm::pi<float>()); // TAU is 2PI
				float yPos = std::cos(ySegment * glm::pi<float>());
				float zPos = std::sin(xSegment * TAU) * std::sin(ySegment * glm::pi<float>());

				m_Positions.push_back(glm::vec3(xPos, yPos, zPos));
				m_UV.push_back(glm::vec2(xSegment, ySegment));
				m_Normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < ySegments; ++y)
		{
			for (int x = 0; x < xSegments; ++x)
			{
				m_Indices.push_back((y + 1) * (xSegments + 1) + x);
				m_Indices.push_back(y * (xSegments + 1) + x);
				m_Indices.push_back(y * (xSegments + 1) + x + 1);
				
				m_Indices.push_back((y + 1) * (xSegments + 1) + x);
				m_Indices.push_back(y * (xSegments + 1) + x + 1);
				m_Indices.push_back((y + 1) * (xSegments + 1) + x + 1);
			}
		}
	}

	Skybox::Skybox(float _size)
	{
		float size = _size * 0.5f;

		m_Positions = std::vector<glm::vec3>{
			// Front
			glm::vec3(-size, -size, size),
			glm::vec3(size, -size, size),
			glm::vec3(size,  size, size),
			glm::vec3(-size,  size, size),
			// Right
			glm::vec3(size, -size, size),
			glm::vec3(size, -size, -size),
			glm::vec3(size,  size, -size),
			glm::vec3(size,  size, size),
			// Back
			glm::vec3(-size, -size, -size),
			glm::vec3(-size,  size, -size),
			glm::vec3( size,  size, -size),
			glm::vec3( size, -size, -size),
			// Left
			glm::vec3(-size, -size, size),
			glm::vec3(-size,  size, size),
			glm::vec3(-size,  size, -size),
			glm::vec3(-size, -size, -size),
			// Bottom
			glm::vec3(-size, -size, size),
			glm::vec3(-size, -size, -size),
			glm::vec3( size, -size, -size),
			glm::vec3( size, -size, size),
			// Top
			glm::vec3(-size,  size, size),
			glm::vec3( size,  size, size),
			glm::vec3( size,  size, -size),
			glm::vec3(-size,  size, -size)
		};

		m_Indices = std::vector<uint32_t>{
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};
	}

	Plane::Plane(float xsize, float zsize, int xdivs, int zdivs, float smax, float tmax)
	{
		int nPoints = (xdivs + 1) * (zdivs + 1);
		std::vector<float> p(3 * nPoints);
		std::vector<float> n(3 * nPoints);
		std::vector<float> tex(2 * nPoints);
		//std::vector<float> tang(4 * nPoints);
		std::vector<uint32_t> el(6 * xdivs * zdivs);

		float x2 = xsize / 2.0f;
		float z2 = zsize / 2.0f;
		float iFactor = (float)zsize / zdivs;
		float jFactor = (float)xsize / xdivs;
		float texi = smax / xdivs;
		float texj = tmax / zdivs;
		float x, z;
		int vidx = 0, tidx = 0;
		for (int i = 0; i <= zdivs; i++) {
			z = iFactor * i - z2;
			for (int j = 0; j <= xdivs; j++) {
				x = jFactor * j - x2;
				float xPos = p[vidx] = x;
				float yPos = p[vidx + 1] = 0.0f;
				float zPos = p[vidx + 2] = z;
				
				m_Positions.push_back(glm::vec3(xPos, yPos, zPos));

				float t1 = tex[tidx] = j * texi;
				float t2 = tex[tidx + 1] = (zdivs - i) * texj;

				m_UV.push_back(glm::vec2(t1, t2));

				float n1 = n[vidx] = 0.0f;
				float n2 = n[vidx + 1] = 1.0f;
				float n3 = n[vidx + 2] = 0.0f;
				m_Normals.push_back(glm::vec3(n1, n2, n3));

				vidx += 3;
				tidx += 2;
			}
		}

		//for (int i = 0; i < nPoints; i++) {
		//	tang[i * 4 + 0] = 1.0f;
		//	tang[i * 4 + 1] = 0.0f;
		//	tang[i * 4 + 2] = 0.0f;
		//	tang[i * 4 + 3] = 1.0f;
		//}

		uint32_t rowStart, nextRowStart;
		for (int i = 0; i < zdivs; i++) {
			rowStart = (uint32_t)(i * (xdivs + 1));
			nextRowStart = (uint32_t)((i + 1) * (xdivs + 1));
			for (int j = 0; j < xdivs; j++) {
				m_Indices.push_back(rowStart + j);
				m_Indices.push_back(nextRowStart + j);
				m_Indices.push_back(nextRowStart + j + 1);
				m_Indices.push_back(rowStart + j);
				m_Indices.push_back(nextRowStart + j + 1);
				m_Indices.push_back(rowStart + j + 1);
			}
		}
	}
}
