#pragma once
#include "Core.h"
#include "Events/Event.h"

namespace Titan {

	class TITAN_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string GetDebugName() { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}
