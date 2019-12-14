#pragma once
#include "Component.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"

class Sprite : public Component
{
public:  
    Sprite();
    ~Sprite();

    void Update(float frameTime);
    void Initialize() {}
	void Serialize(FILE** fpp);
	void HandleObjectEvent(ObjectEventType eventType);

	inline void SetPosistion(const glm::vec2& position) { m_Pos = position; }
	inline void SetRotation(float rotate) { m_Rotate = rotate; }
	inline void SetScale(const glm::vec2& scale) { m_Scale = scale; }
	inline float GetRotation() const { return m_Rotate; }
	
	void DrawSprite();
	void SetSpriteTexture(const std::string& pFileName);
	inline void SetSpriteTexture(std::shared_ptr<Titan::Texture2D> texture) { m_Texture = texture; }
	inline std::shared_ptr<Titan::Texture2D> GetSpriteTexture() const { return m_Texture; };
	inline void SetVisible(bool visible) { IsVisiable = visible; }

private:
	glm::vec2 m_Scale;
	glm::vec2 m_Pos;
	float m_Rotate;
	std::shared_ptr<Titan::Texture2D> m_Texture;

	bool IsVisiable = true;
	
};
