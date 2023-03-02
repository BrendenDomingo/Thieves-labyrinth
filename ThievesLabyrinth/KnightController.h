#pragma once
#include "EnemyController.h"

class CTransform;
class CProjectileEntity;
class CRigidbody;
class CAnimatorComponent;
class CStats;
class CMeshRenderer;
class CParticleEmitter;

class CKnightController : public IEnemyController
{
private:
	CParticleEmitter*		m_pcParticleEmitter;

	float					m_fLastBlockTime;
	
	float					m_fBlockCooldown;
	
	float					m_fBlockMaxRange;

	float					m_fBlockMinRange;

	bool					m_bIsAttacking;

	bool					m_bIsDeflecting;

	bool					m_bHasAttacked;

	void Attack();

	void InitializeComponents() override;
	void InitializeProjectile() override;

public:
	CKnightController(IEntity* pcOwner);
	~CKnightController();

	CKnightController& operator=(CKnightController& cCopy);

	void Update(float fDeltaTime) override;

	bool IsBlocking();

	bool IsDeflecting();
};

