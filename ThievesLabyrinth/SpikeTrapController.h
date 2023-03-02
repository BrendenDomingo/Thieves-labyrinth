#pragma once
#include "Component.h"

class CBoxCollider;
class CTransform;

class CSpikeTrapController : public IComponent
{
	float	m_fActiveTime;
	float	m_fCooldown;
	float	m_fCurrentTime;
	float	m_fDamagePerHit;
	bool	m_bIsTrapActive;

	void ToggleTrap();

	CBoxCollider* m_pcCollider;
	CTransform*	  m_pcTransform;

public:
	CSpikeTrapController(IEntity* pcOwner);

	void Update(float fDeltaTime);

	void SetDamage(float fDamage);

	float GetDamage();
	
	CSpikeTrapController& operator=(CSpikeTrapController& cToCopy);

	~CSpikeTrapController();
};

