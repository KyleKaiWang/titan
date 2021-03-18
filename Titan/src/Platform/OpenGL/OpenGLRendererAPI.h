#pragma once

#include "Renderer/RendererAPI.h"

#ifdef _DEBUG
#define GLCHECKERROR {GLenum err = glGetError(); if (err != GL_NO_ERROR) { throw EngineException(_CRT_WIDE(__FILE__), __LINE__, L"OpenGL error " + wStr(err)); } }
#else
#define GLCHECKERROR
#endif // DEBUG

namespace Titan {
	
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void DispatchCompute(uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z) override;
		virtual void PolyModeFill() override;;
		virtual void PolyModeLine() override;;
		virtual void BindDefaultFrameBuffer() override;;
		virtual void TransferColorBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h) override;
		virtual void TransferColorBit(uint32_t src, uint32_t src_tex, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_tex, uint32_t dest_w, uint32_t dest_h) override;
		virtual void TransferDepthBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h) override;
		virtual void TransferStencilBit(uint32_t src, uint32_t src_w, uint32_t src_h, uint32_t dest, uint32_t dest_w, uint32_t dest_h) override;
		virtual void Reverse_Z(bool b) override;
		virtual void DepthTest(bool test, bool write) override;
		virtual void DepthFunc(RendererAPI::CompareEnum e) override;
		virtual void StencilTest(bool test, bool write) override;
		virtual void CullFace(bool enabled, bool front) override;
		virtual void Blend(bool enabled) override;
		virtual void BlendFunc(RendererAPI::BlendFuncEnum e) override;
		virtual void DepthClamp(bool enabled) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	};
}