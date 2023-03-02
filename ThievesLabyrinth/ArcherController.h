#pragma once
#include "EnemyController.h"

class CStats;
class CAnimatorComponent;
class IEntity;
class CProjectileEntity;
class CRigidbody;
class CTransform;


class CArcherController : public IEnemyController
{
	bool m_bRestartCycle;
	bool m_bIsAttacking;
	bool m_bHasAttacked;

	float m_fMovementTimer;
	float m_fMaxMovementTime;

	float m_fWaitTimer;
	float m_fMaxWaitTime;

	float m_fHorizontalDir;

	int m_nAttackCount;

public:
	CArcherController(IEntity* pcOwner);
	~CArcherController();

	void Update(float fDeltaTime) override;

	CArcherController& operator=(CArcherController& cCopy);

private:
	void Attack();
	void InitializeProjectile() override;
	CMath::TVECTOR3 CalculateMoveDirection(CMath::TVECTOR3 tDirectionPlayer, float fDistancePlayer);
};

