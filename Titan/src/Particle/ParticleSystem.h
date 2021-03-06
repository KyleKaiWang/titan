#pragma once
#include <glm/glm.hpp>
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

namespace Titan {

	struct Particle
	{
		glm::vec3 Position;
		glm::vec3 Speed;
		glm::vec3 Velocity, VelocityVariation;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd, SizeVariation;
		float LifeTime = 1.0f;
		float LifeRemaining = 1.0f;
		bool Active = false;
	};

	class ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();
		
		void Update(float ts);
		void Render();
		void Emit(const Particle& particle);

		void SetPoolSize(uint32_t size) { m_ParticlePoolSize = size; m_ParticlePool.resize(m_ParticlePoolSize); }
		
	private:
		std::vector<Particle> m_ParticlePool;
		uint32_t m_PoolIndex = 999;
		uint32_t m_ParticlePoolSize = 1000;
		
		struct BufferData
		{
			std::shared_ptr<VertexArray> ParticleVertexArray;
			std::shared_ptr<VertexBuffer> ParticleVertexBuffer;
			std::shared_ptr<IndexBuffer> ParticleIndexBuffer;
			std::shared_ptr<Shader> ParticleShader;
		};
		BufferData m_BufferData;
	};
}