#include "tpch.h"
#include "Sprite.h"
#include "Renderer/Renderer2D.h"

namespace Titan {
	
	Sprite::Sprite()
	{
		m_SpriteVertexArray = VertexArray::Create();

		m_CurrentFrame = 0;
		m_StartFrame = 0;
		m_TotalFrames = m_TotalHorizontalFrame * m_TotalVerticalFrame;
		m_LastFrame = m_TotalFrames - 1;

		m_SpriteWidth = GetTextureWidth() / m_TotalHorizontalFrame;
		m_SpriteHeight = GetTextureHeight() / m_TotalVerticalFrame;
		m_ElapsedTime = 0;
		m_MSPerFrame = 1000 / 15;

		m_IsPaused = false;
		m_LoopingAnim = false;
	}

	Sprite::Sprite(std::shared_ptr<Texture2D>& texture2D)
		: m_Texture2D(texture2D)
	{
		m_SpriteVertexArray = VertexArray::Create();
	}

	Sprite::Sprite(std::shared_ptr<Texture2D>& texture2D, int numRowFrames, int numColumnFrames)
		: m_Texture2D(texture2D), m_TotalHorizontalFrame(numRowFrames), m_TotalVerticalFrame(numColumnFrames)
	{
		m_SpriteVertexArray = VertexArray::Create();

		m_CurrentFrame = 0;
		m_StartFrame = 0;
		m_TotalFrames = m_TotalHorizontalFrame * m_TotalVerticalFrame;
		m_LastFrame = m_TotalFrames - 1;
		m_SpriteWidth = GetTextureWidth() / m_TotalHorizontalFrame;
		m_SpriteHeight = GetTextureHeight() / m_TotalVerticalFrame;
		m_ElapsedTime = 0;
		m_MSPerFrame = 1000 / 15;

		m_IsPaused = false;
		m_LoopingAnim = false;
	}

	Sprite::Sprite(const std::string& path, int numRowFrames, int numColumnFrames)
		: m_Texture2D(Texture2D::Create(path)), m_TotalHorizontalFrame(numRowFrames), m_TotalVerticalFrame(numColumnFrames)
	{
		m_SpriteVertexArray = VertexArray::Create();

		m_CurrentFrame = 0;
		m_StartFrame = 0;
		m_TotalFrames = m_TotalHorizontalFrame * m_TotalVerticalFrame;
		m_LastFrame = m_TotalFrames - 1;
		m_SpriteWidth = GetTextureWidth() / m_TotalHorizontalFrame;
		m_SpriteHeight = GetTextureHeight() / m_TotalVerticalFrame;
		m_ElapsedTime = 0;
		m_MSPerFrame = 1000 / 15;

		m_IsPaused = false;
		m_LoopingAnim = false;
	}

	Sprite::~Sprite()
	{
	}

	void Sprite::Update(float ts)
	{
		if (m_IsPaused)
		{
			return;
		}

		m_ElapsedTime += ts;
		if (m_ElapsedTime >= m_MSPerFrame)
		{
			const DWORD numFramesToAdvance = m_ElapsedTime / m_MSPerFrame;
			m_ElapsedTime -= numFramesToAdvance * m_MSPerFrame;
			
			int targetFrame = GetCurrentFrame() + numFramesToAdvance;
			
			//Go back the start frame if in the animation sequence
			if (m_LoopingAnim == true && (targetFrame >= GetLastFrame()))
			{
				targetFrame = GetStartFrame();
			}

			//Stay on the last frame if not looping
			else if ((m_LoopingAnim == false) && (targetFrame >= GetLastFrame()))
			{
				targetFrame = GetLastFrame() - 1;
			}

			SetCurrentFrame(targetFrame);
		}
	}

	void Sprite::Draw()
	{
		float texWidth = GetTextureWidth();
		float texHeight = GetTextureHeight();
		
		float perTexCoordOffsetX = (float)(m_SpriteWidth / texWidth);
		float perTexCoordOffsetY = (float)(m_SpriteHeight / texHeight);

		float texCoordX = (GetCurrentFrame() % m_TotalHorizontalFrame) * perTexCoordOffsetX;
		float texCoordY = ((GetCurrentFrame() / m_TotalHorizontalFrame) * (perTexCoordOffsetY));

		DrawInternal(glm::vec2(0.5f), glm::vec2(texCoordX, texCoordY), glm::vec2(perTexCoordOffsetX, perTexCoordOffsetY));
	}

	void Sprite::DrawInternal(const glm::vec2& rect, const glm::vec2& texCoord, const glm::vec2& texCoordOffset)
	{
		float quadVertices[] = {
		   -rect.x, -rect.y, 0.0f, texCoord.x, texCoord.y,
			rect.x, -rect.y, 0.0f, texCoord.x + texCoordOffset.x, texCoord.y,
			rect.x,  rect.y, 0.0f, texCoord.x + texCoordOffset.x, texCoord.y + texCoordOffset.y,
		   -rect.x,  rect.y, 0.0f, texCoord.x, texCoord.y + texCoordOffset.y
		};

		m_SpriteVertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		m_SpriteVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SpriteVertexArray->AddVertexBuffer(m_SpriteVertexBuffer);

		Renderer2D::DrawQuad(m_SpriteVertexArray, nullptr, glm::vec3(0.0f), glm::vec2(1.0f), 0.0f, m_Texture2D);
	}

	void Sprite::SetAnimSequence(int startWithFrame, int numFrames)
	{
		SetCurrentFrame(startWithFrame);
		SetStartFrame(startWithFrame);
		SetLastFrame(startWithFrame + numFrames);
	}
}