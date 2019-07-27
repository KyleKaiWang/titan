#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Titan {

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void UnBind() const;

		void UploadUniformMat4(const std::string& name, glm::mat4& matrix);
	private:
		uint32_t m_RendererID;
	};
}


