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
		float ball_radius = 0.25;
		float sigma = 0.5;
		float kappa = 1.5;
		float beta = 0.0;

		// Parameters for the blurring pass
		int filter_scale = 2;
		float edge_sharpness = 1;
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