#include "KnightController.h"

#include "EventManager.h"
#include "Entity.h"
#include "EnumTypes.h"
#include "Time.h"

#include "Transform.h"
#include "Rigidbody.h"
#include "Stats.h"
#include "MeshRenderer.h"
#include "AnimatorComponent.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "ParticleEffect.h"
#include "PathAgent.h"

#include "InputManager.h"

CKnightController::CKnightController(IEntity* pcOwner) : IEnemyController(pcOwner)
{
	InitializeComponents();
	InitializeProjectile();

	m_nEnemyType = eEnemyType::KNIGHT;

	m_fSafeDistance = 4.0f;
	m_fDeathTimer = 0.0f;

	m_bIsAttacking = false;
	m_bIsDeflecting = false;
	m_fBlockMaxRange = 25.0f;
	m_fBlockMinRange = 10.0f;
}

void CKnightController::Update(float fDeltaTime)
{
	/*
		if blocking
			Block Timer--

		else if can block
			Block

		else
			if can see player

				if is close to player
					stay still
					attack

				else
					go towards player ray

			else
				go towards player ai
	
	*/
	IEnemyController::Update(fDeltaTime);

	if (IsDead())
	{
		// Play Death animation		
	
		m_pcAnimator->SetLoop(false);

		m_pcOwner->SetActiveState(false);
		m_pcOwner->GetComponent<CMeshRenderer>()->SetActiveState(true);
		m_pcOwner->GetComponent<CKnightController>()->SetActiveState(true);

		m_pcRigidbody->SetVelocity({ 0,0,0 });
		//Update time for dissolve shader
		m_fDeathTimer += fDeltaTime * .25f;		

		return;
	}


	if(m_fBlockCooldown > 0)
		m_fBlockCooldown -= fDeltaTime;

	CMath::TVECTOR3 tVectorToTarget = m_pcPlayerTransform->GetPosition() - m_pcTransform->GetPosition();
	float fAttackRange = m_pcOwner->GetComponent<CCapsuleCollider>()->GetRadius() * m_pcTransform->GetScale().x + 2.2f;


	m_pcTransform->TurnTo(m_pcPlayerTransform, fDeltaTime);
	m_pcAgent->Stop();
	if(IsBlocking())
	{
		m_fLastBlockTime -= fDeltaTime;

		if(!IsBlocking())
		{
			m_fBlockCooldown = 2.0f;

			m_pcParticleEmitter->Disable();
		}
	}
	else if(CMath::Vector3Magnitude(tVectorToTarget) <= fAttackRange)
	{
		m_pcRigidbody->SetVelocity({ 0, 0, 0 });
		//m_pcTransform->TurnTo(m_pcPlayerTransform, fDeltaTime);

		// Play attack animation
		SwitchAnimation(eEnemyAnimation::ATTACK, false, 0.0f, m_pcStats->GetAttackSpeed());
		m_pcAnimator->SetLoop(true);

		m_bIsAttacking = true;

		if (m_pcAnimator->GetCurrentAnimationTime() <= 0.6f)
		{
			m_bHasAttacked = false;
		}

		if(!m_bHasAttacked && m_pcAnimator->GetCurrentAnimationTime() > 0.6f)
		{
			Attack();
			m_bHasAttacked = true;
		}
	}
	else if(CMath::Vector3Magnitude(tVectorToTarget) <= m_fBlockMaxRange && m_fBlockCooldown <= 0 && !m_bIsAttacking)
	{
		m_pcRigidbody->SetVelocity({ 0, 0, 0 });
		//m_pcTransform->TurnTo(m_pcPlayerTransform, fDeltaTime);

		//Block

		if(m_pcAnimator->GetCurrentAnimation() != eEnemyAnimation::BLOCK)
		{
			m_pcAnimator->SetAnimation(eEnemyAnimation::BLOCK);
			m_pcAnimator->SetAnimationTime(0.0);
			m_pcAnimator->SetSpeed(1.0f);

			m_bIsDeflecting = rand() % 2 == 0;

			if(m_bIsDeflecting)
				m_pcParticleEmitter->RefreshEmitterSettings();
		}

		m_fLastBlockTime = 2.0F;
	}
	else
	{
		m_pcAgent->Start();
		// Play walk animation
		if(m_pcAnimator->GetCurrentAnimation() != eEnemyAnimation::WALK)
		{
			m_pcAnimator->SetAnimation(eEnemyAnimation::WALK);
			m_pcAnimator->SetAnimationTime(0.0);
		}
	}
}

void CKnightController::InitializeComponents()
{
	IEnemyController::InitializeComponents();
	m_pcParticleEmitter = m_pcOwner->GetComponent<CParticleEmitter>();
}

void CKnightController::InitializeProjectile()
{
	m_pcProjectileRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::TRANSFORM);

	CCapsuleCollider* pcMeleeCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::CAPSULE_COLLIDER);
	pcMeleeCollider->SetRadius(0.5f);
	pcMeleeCollider->SetHeight(2.2f);
	pcMeleeCollider->SetTrigger(true);

	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::RIGIDBODY);

	CProjectileComponent* pcMeleeComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PROJECTILE_COMPONENT);
	pcMeleeComponent->SetDamage(1.0f);
	pcMeleeComponent->SetLifeTime(0.3f);
	pcMeleeComponent->SetCasterEntity(eEntity::ENEMY);
	m_pcProjectileRef->SetActiveState(false);
}

bool CKnightController::IsBlocking()
{
	return m_fLastBlockTime > 0;
}

bool CKnightController::IsDeflecting()
{
	return m_bIsDeflecting;
}


void CKnightController::Attack()
{
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::KNIGHT_ATTACK));
	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcProjectileRef);
	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage()); //Modify damage based on items/stats/buffs/debuffs
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward = { *(CMath::TVECTOR3*)&pcOwnerTransform->GetMatrix().r[2] };

	pcProjTransform->SetMatrix(pcOwnerTransform->GetMatrix());
	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition() + CMath::TVECTOR3(0,2,0));

	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity(tForward * 6.0f);

	pcNewProjectile->SetActiveState(true);
	m_bIsAttacking = false;
}

CKnightController::~CKnightController()
{
}

CKnightController & CKnightController::operator=(CKnightController & cCopy)
{
	m_pcPlayerTransform = cCopy.m_pcPlayerTransform;
	return *this;
}
