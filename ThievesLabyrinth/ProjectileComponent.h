#pragma once
#include "Component.h"

class CTransform;

class CProjectileComponent : public IComponent
{
	float m_fTimeLeft;
	float m_fDamage;
	int   m_nCasterEntityType;
	float m_fCost;

	float  m_fRotationSpeed;

	CTransform* m_pcProjectileTransform;

public:
	CProjectileComponent(IEntity* pcOwner);
	
	void SetLifeTime(float fTime);
	void SetDamage(float fDamage);
	void SetCasterEntity(int nType);
	void SetCost(float fManaCost);
	void SetRotationSpeed(float fRotationSpeed);

	float GetTimeLeft();
	float GetDamage();
	int	  GetCasterEntity();
	float GetCost();

	void Update(float fDeltaTime);


	void Destroy(int nNewTexture);

	CProjectileComponent& operator=(CProjectileComponent& pCComponentToCopy);
	
	~CProjectileComponent();
};

