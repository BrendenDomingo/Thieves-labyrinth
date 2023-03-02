#pragma once
#include "Component.h"

class CTransform;
class CProjectileEntity;
class CEnvironmentEntity;
class CRigidbody;
class CAnimatorComponent;
class CStats;
class CInventory;
class CPathAgent;
class CParticleEmitter;
class CMeshRenderer;

class CFireDemonController : public IComponent
{
	CRigidbody*				m_pcRigidbody;
	CTransform*				m_pcTransform;
	CTransform*				m_pEnemyTransform;
	CAnimatorComponent*		m_pcAnimator;
	CStats*					m_pcStats;
	CInventory*				m_pcInventory;
	CPathAgent*				m_pcAgent;
	CParticleEmitter*		m_pcEmitter;
	CMeshRenderer*			m_pcMeshRenderer;

	float					m_fBasicAttacktime;
	float					m_fFireBallAttackTime;
	float					m_fAttackAnimTime;
	float					m_fAoeAttackTime;
	float					m_fFirewallAttackTime;
	float					m_fEnemyRadius;
	float					m_fDamageMultiplier;
	float					m_fResistanceMultiplier;
	float					m_fSpeedMultiplier;
	CMath::TVECTOR3			m_tBuffTimes;
	CMath::TVECTOR3			m_tMaxBuffTimes;
	float					m_fCooldownMultiplier;
	float					m_fAttackSpeed;

	float					m_fFireballCooldown;
	float					m_fAOECooldown;
	float					m_fFirewallCooldown;

	float					m_fFlashingTime;

	CProjectileEntity*		m_pcBasicAttackRef;
	CProjectileEntity*		m_pcFireballRef;
	CProjectileEntity*		m_pcAOERef;
	CProjectileEntity*		m_pcFirewallRef;
	CEnvironmentEntity*		m_pcClickIndicator;

	bool					m_bIsAttacking;
	bool					m_bIsAttackingSpecial[3];
	bool					m_bMoving;
	bool					m_bMouseControls;
	bool					m_bCheckFloorRay;
	bool					m_bIsNavigating;

	bool					m_bHoverItem;

	bool					m_bPrevFrameClick;

	CMath::TVECTOR3			m_tClickPos;

	// Bool for debugging
	//bool					m_bDebug;

	void InitComponents();
	void InitIndicator();
	void InitBasicProjectile();
	void InitFireball();
	void InitAOEProj();
	void InitFireWall();

	void ResetTimers();
	void ResetMultipliers();

	void MoveToMouseDirection();
	void MoveToMousePosition();

	void BeginBasic(CMath::TVECTOR3 tDirection);
	void BeginBasicAnimation();

	void InstantiateIndicator(CMath::TVECTOR3 tPosition);

	void ShootBasic();
	void FireballAttack();
	void AOE(CMath::TVECTOR3 vPosition);
	void Firewall(CMath::TVECTOR3 vPosition);
	void Inventory();
	void UpdateTimers(float fDeltaTime);
	void KeyboardMovement();
	void MouseMovement();
	void GetMouseRay(CMath::TVECTOR3& tStart, CMath::TVECTOR3& tEnd);
	IEntity* GetSelectedEnemy();
	CMath::TVECTOR3 GetFloorPosition();
	void RefillMana();

public:
	CFireDemonController(IEntity* pcOwner);
	void Update(float fDeltaTime);

	float GetResistance();
	CMath::TVECTOR3 GetAttackTime();
	CMath::TVECTOR3 GetBuffTimes();
	CMath::TVECTOR3 GetMaxBuffTime();

	float GetFireballCoolDown();
	float GetAOECooldown();
	float GetFirewallCooldown();

	void UseMouseControls(bool bMouseControls);
	void InitProjectiles();

	float GetFlashTime();

	void ResetFlashTimer();

	bool IsOverItem();

	void OverItem(bool bConfirm);

	~CFireDemonController();
};

