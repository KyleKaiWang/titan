#include "tpch.h"
#include "Component.h"

Component::Component(CompoentType componentType)
{
	m_Owner = nullptr;
    m_Type = componentType;
}