#pragma once

namespace Titan {

	class Camera;
	class Texture2D;

	struct SSAOParameters
	{
		int kernelSize = 64;
		float radius = 0.5;
		float bias = 0.025;
	};

	class RendererSSAO
	{
	public:
		static void Init();
		static void RenderSSAO(Camera camera, std::shared_ptr<Texture2D>& gPosition, std::shared_ptr<Texture2D>& gNormal);
		static void RenderSSAOBlur();
		static std::shared_ptr<Texture2D> GetSSAOTexture();
		static SSAOParameters SSAOParams;

		static void RenderDebug();
	};
	
}