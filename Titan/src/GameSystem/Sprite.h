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

    void Update();
    void Initialize() {}
	void Serialize(FILE** fpp);
	void HandleObjectEvent(ObjectEvent* pObjectEvent);
	
	void Draw();
	inline std::shared_ptr<Titan::Texture2D> GetTexture() const { return m_Texture; };

private:
	glm::vec2 m_Size;
	glm::vec2 m_Pos;
	std::shared_ptr<Titan::Texture2D> m_Texture;
	
};
