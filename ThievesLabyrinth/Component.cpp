#include "Component.h"


IComponent::IComponent(IEntity* pcOwner)
{
	m_pcOwner = pcOwner;
}

bool IComponent::IsActive()
{
	return m_bState;
}

void IComponent::SetActiveState(bool bState)
{
	m_bState = bState;
}

IComponent::~IComponent()
{
}

