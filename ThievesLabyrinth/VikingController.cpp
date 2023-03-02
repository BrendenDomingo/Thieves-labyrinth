#include "VikingController.h"

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
#include "PathAgent.h"



CVikingController::CVikingController(IEntity* pcOwner) : IEnemyController(pcOwner)
{
	InitializeComponents();
	InitializeProjectile();

	m_nEnemyType = eEnemyType::VIKING;
	m_bEnemyCountSet = false;

	m_fSafeDistance = 4.0f;
	m_fDeathTimer = 0.0;

	m_bIsAttacking = false;
	m_bHasAttacked = false;
	m_bIsMad = false;

}

void CVikingController::Update(float fDeltaTime)
{
	IEnemyController::Update(fDeltaTime);

	if (!m_bEnemyCountSet)
	{
		m_nEnemyCount = m_pcOwner->m_pcRoom->m_nEnemyCount;
		m_bEnemyCountSet = true;
	}

	if (IsDead())
	{	 		
		

		m_pcOwner->SetActiveState(false);
		m_pcOwner->GetComponent<CMeshRenderer>()->SetActiveState(true);
		m_pcOwner->GetComponent<CVikingController>()->SetActiveState(true);

		m_pcRigidbody->SetVelocity({ 0,0,0 });
		
		m_fDeathTimer += fDeltaTime * .25f;		

		return;
	}

	if (!m_bIsMad && m_pcOwner->m_pcRoom->m_nEnemyCount <= m_nEnemyCount / 2)
	{
		m_pcStats->SetMovementSpeed(m_pcStats->GetMovementSpeed() * 1.5f);
		m_pcStats->SetAttackSpeed(m_pcStats->GetAttackSpeed() * 2.0f);
		m_bIsMad = true;
	}

	CMath::TVECTOR3 tVectorToTarget = m_pcPlayerTransform->GetPosition() - m_pcTransform->GetPosition();
	float fDis = m_pcOwner->GetComponent<CCapsuleCollider>()->GetRadius() * m_pcTransform->GetScale().x + 2.0f;
	if (CMath::Vector3Magnitude(tVectorToTarget) > fDis)
	{
		m_pcAgent->Start();
		m_pcTransform->TurnTo(m_pcPlayerTransform, fDeltaTime);

		// Play Viking walk animation
		if (m_pcAnimator->GetCurrentAnimation() != eEnemyAnimation::WALK)
		{
			m_pcAnimator->SetAnimation(eEnemyAnimation::WALK);
			m_pcAnimator->SetAnimationTime(0.0);
		}
	}
	else
	{
		m_pcAgent->Stop();
		m_pcRigidbody->SetVelocity({ 0, 0, 0 });
		m_pcTransform->TurnTo(m_pcPlayerTransform, fDeltaTime);

		// Play Viking attack animation
		SwitchAnimation(eEnemyAnimation::ATTACK, false, 0.0f, m_pcStats->GetAttackSpeed());
		m_pcAnimator->SetLoop(true);

		m_bIsAttacking = true;

		if (m_pcAnimator->GetCurrentAnimationTime() <= 0.9f)
		{
			m_bHasAttacked = false;
		}

		if (!m_bHasAttacked && m_pcAnimator->GetCurrentAnimationTime() > 0.9f)
		{
			Attack();
			m_bHasAttacked = true;
		}
	}
}

void CVikingController::Attack()
{
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

void CVikingController::InitializeProjectile()
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

CVikingController & CVikingController::operator=(CVikingController & cCopy)
{
	m_pcPlayerTransform = cCopy.m_pcPlayerTransform;

	return *this;
}

CVikingController::~CVikingController()
{
}
