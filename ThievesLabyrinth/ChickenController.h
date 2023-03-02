#pragma once
#include "EnemyController.h"

class CTransform;
class CProjectileEntity;
class CRigidbody;
class CAnimatorComponent;
class CStats;
class CParticleEmitter;

class CChickenController : public IEnemyController
{
private:
	CParticleEmitter*		m_pcEmitter;
	CMath::TVECTOR3			m_tLastPos;
	bool					m_bExploding;

public:
	CChickenController(IEntity* pcOwner);
	~CChickenController();


	void Update(float fDeltaTime) override;

	CChickenController& operator=(CChickenController& cCopy);

private:
	void InitializeProjectile() override;
	void Explode();

};

