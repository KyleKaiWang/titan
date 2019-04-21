#pragma once

#include "Event.h"

namespace Titan {
	
	class MouseEvent : public Event
	{
	public:
		MouseEvent(float x, float y)
			:m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::streamstring ss;
			ss << "MouseEvent: " << m_MouseX << "," << m_MouseY;
			return ss.str();
		}

	private:
		float m_MouseX, m_MouseY
	};

	
	class TITAN_API MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float xOffset, float yOffset)
			:m_XOffset(xOffset), m_YOffset(yOffset)

		inline float GetXOffset const { return m_XOffset; }
		inline float GetYOffset const { return m_YOffset; }

		std::string ToString() const override
		{
			std::streamstring ss;
			ss << "MouseScrollEvent: " << m_XOffset << "," << m_YOffset;
			return ss.str();
		}

	private:
		float m_XOffset, m_YOffset
	}
}