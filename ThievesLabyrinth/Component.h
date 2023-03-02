#pragma once
#include "Math.h"
class IEntity;

class IComponent
{
	bool m_bState;
public:
	IComponent(IEntity* pcOwner);

	int m_nComponentType;

	IEntity* m_pcOwner;

	bool IsActive();

	void SetActiveState(bool bState);

	virtual~IComponent() = 0;
};






