#pragma once
#include "EnemyController.h"

class CStats;
class CAnimatorComponent;
class IEntity;
class CProjectileEntity;
class CRigidbody;
class CTransform;
class CPathAgent;

class CMageController : public IEnemyController
{
private:
	bool					m_bIsAttacking;
	bool					m_bCanSeePlayer;
	bool					m_bTeleportDone;
	bool					m_bTeleportStart;

	bool					m_bHasAttacked;

	float					m_fAttackTimer;
	float					m_fMaxAttackTime;

	float					m_fSightCheckTimer;
	float					m_fMaxSightCheckTime;

	float					m_fTeleportTimer;
	float					m_fMaxTeleportTime;

public:
	CMageController(IEntity* pcOwner);
	~CMageController();
	
	void Update(float fDeltaTime) override;

	CMageController& operator=(CMageController& cCopy);

private:
	void InitializeTimers();
	void ZeroTimers();
	void UpdateTimers(float fDeltaTime);
	void InitializeProjectile() override;

	void Attack();

	void Teleport();
	void BeginDeath();

	void SetPathNavigation(bool bNavigate);
};
