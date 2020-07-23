#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Core.h"

namespace Titan {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetShaderID() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetInt2(const std::string& name, const glm::ivec2& value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetMat3(const std::string& name, const glm::mat3& matrix) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetBlock(const std::string& name, uint32_t value) = 0;

		static Ref<Shader> Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		static Ref<Shader> Create(const std::string& computeShaderPath);
	};
}


