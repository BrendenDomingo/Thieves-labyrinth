#pragma once
#include "Component.h"

class IEntity;
class CTransform;
class CRigidbody;
class CMeshRenderer;
class CAnimatorComponent;
class CStats;
class CProjectileEntity;
class CPathAgent;

class IEnemyController : public IComponent
{
protected:
	CTransform*			m_pcTransform;
	CRigidbody*			m_pcRigidbody;
	CMeshRenderer*		m_pcMeshRenderer;
	CAnimatorComponent*	m_pcAnimator;
	CStats*				m_pcStats;
	CPathAgent*			m_pcAgent;

	CProjectileEntity*	m_pcProjectileRef;
	CTransform*			m_pcPlayerTransform;

	int					m_nEnemyType;
	float				m_fSafeDistance;

	float				m_fFlashingTime;

	float m_fDeathTimer;

	virtual void InitializeComponents();
	virtual void InitializeProjectile() = 0;

	bool RaycastToPlayer() const;
	bool IsMoving() const;
	bool IsDead() const;

	float GetDistanceToPlayer() const;
	void SwitchAnimation(int nAnimation, bool bOverride, float fTime = 0.0f, float fSpeed = 1.0f);

public:
	IEnemyController(IEntity* pcOwner) : IComponent(pcOwner) {}
	virtual ~IEnemyController() {}

	virtual void Update(float fDeltaTime);
	void GetPlayerReference();

	float GetSafeDistance();
	float GetDeathTimer();

	float GetFlashTime();

	void ResetFlashTimer();

	int GetEnemyType();
};