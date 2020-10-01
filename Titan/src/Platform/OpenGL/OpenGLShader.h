#pragma once

#include "Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Titan {
	
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexShaderPath, const std::string& geometryShaderPath, const std::string& fragmentShaderPath);
		OpenGLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		OpenGLShader(const std::string& computeShaderPath);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual uint32_t GetShaderID() const override { return m_RendererID; }

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetInt2(const std::string& name, const glm::ivec2& value) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetMat3(const std::string& name, const glm::mat3& matrix) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetBlock(const std::string& name, uint32_t value) override;

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformInt2(const std::string& name, const glm::ivec2& value);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformBlock(const std::string& name, uint32_t bindpoint);
	private:
		uint32_t m_RendererID;
		std::string GetShaderString(const std::string& shaderSourcePath);
	};
}