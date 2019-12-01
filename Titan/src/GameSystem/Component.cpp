#include "tpch.h"
#include "Component.h"

Component::Component(COMPONENT_TYPE type)
{
	m_Owner = nullptr;
    m_Type = type;
}