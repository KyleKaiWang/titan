#pragma once

namespace Titan {

	class Camera;
	class Texture2D;

	struct SSAOParameters
	{
		// Parameters for the AO pass
		bool use_rendered_normals = false;
		int n_samples = 64;
		int turns = 64;
		float ball_radius = 0.25f;
		float sigma = 0.5f;
		float kappa = 1.0f;
		float beta = 0.0001f;

		// Parameters for the blurring pass
		int filter_scale = 2;
		float edge_sharpness = 1;
	};

	class RendererSSAO
	{
	public:
		RendererSSAO();
		~RendererSSAO();
		void Init();
		void RenderSSAO(Camera camera, std::shared_ptr<Texture2D>& gPosition, std::shared_ptr<Texture2D>& gNormal, std::function<void()> drawQuad);
		void RenderSSAOBlur(Camera camera, std::shared_ptr<Texture2D>& depthTex, std::shared_ptr<Texture2D>& gNormal, std::function<void()> drawQuad);
		std::shared_ptr<Texture2D> GetSSAOTexture();
		SSAOParameters SSAOParams;

		void RenderDebug();
	};
	
}