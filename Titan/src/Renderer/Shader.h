#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Titan {

	class Shader
	{
	public:
		Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		~Shader();

		void Bind() const;
		void UnBind() const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string GetShaderString(const std::string& shaderSourcePath);
		uint32_t m_RendererID;
	};
}


