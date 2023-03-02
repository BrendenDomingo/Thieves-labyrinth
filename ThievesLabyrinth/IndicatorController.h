#pragma once
#include "Component.h"

class CTransform;

class CIndicatorController : public IComponent
{
	float m_fTimer;

	CTransform* m_pcTransform;

public:
	CIndicatorController(IEntity * pcOwner);

	void Update(float fDeltaTime);

	CIndicatorController& operator=(CIndicatorController & cToCopy);

	float GetTime();

	~CIndicatorController();
};

