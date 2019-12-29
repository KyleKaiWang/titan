#include "tpch.h"
#include "Mesh.h"

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
		std::vector<float> data;
		for (int i = 0; i < m_Positions.size(); ++i)
		{
			data.push_back(m_Positions[i].x);
			data.push_back(m_Positions[i].y);
			data.push_back(m_Positions[i].z);
		
			if (m_UV.size() > 0)
			{
				data.push_back(m_UV[i].x);
				data.push_back(m_UV[i].y);
			}
			if (m_Normals.size() > 0)
			{
				data.push_back(m_Normals[i].x);
				data.push_back(m_Normals[i].y);
				data.push_back(m_Normals[i].z);
			}
		}

		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(&data[0], data.size() * sizeof(float));
		
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
		});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		if (m_Indices.size() > 0)
		{
			m_IndexBuffer = Titan::IndexBuffer::Create(&m_Indices[0], sizeof(m_Indices));
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
			glm::vec3(size, -size, size), 
			glm::vec3(size, -size, -size), 
			glm::vec3(size,  size, -size), 
			glm::vec3(size,  size, size),
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
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
			glm::vec3(0.0f,  0.0f, -1.0f),
		
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
			glm::vec3(0.0f,  0.0f,  1.0f),
		
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
			glm::vec3(-1.0f,  0.0f,  0.0f),
		
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
			glm::vec3(1.0f,  0.0f,  0.0f),
		
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
			glm::vec3(0.0f, -1.0f,  0.0f),
		
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
			glm::vec3(0.0f,  1.0f,  0.0f),
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
}
