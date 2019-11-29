#include "tpch.h"
#include "Component.h"

Component::Component(COMPONENT_TYPE type)
{
	mpOwner = nullptr;
    mType = type;
}