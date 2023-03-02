#pragma once
#include "EnemyController.h"

class CTransform;
class CProjectileEntity;
class CRigidbody;
class CAnimatorComponent;
class CStats;

class CVikingController : public IEnemyController
{
	int						m_nEnemyCount;

	bool					m_bIsAttacking;

	bool					m_bHasAttacked;

	bool					m_bIsMad;

	bool					m_bEnemyCountSet;

public:
	CVikingController(IEntity* pcOwner);
	~CVikingController();

	void Update(float fDeltaTime);

	CVikingController& operator=(CVikingController& cCopy);

private:
	void Attack();
	void InitializeProjectile() override;

};

