
#include "EnemyController.h"
#include "InputManager.h"
#include "PhysicsManager.h"
#include "EnumTypes.h"
#include "Entity.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "EntityManager.h"
#include "CapsuleCollider.h"
#include "EventManager.h"
#include "Rigidbody.h"
#include "Stats.h"
#include "AnimatorComponent.h"
#include "PathAgent.h"
#include "Time.h"

void IEnemyController::InitializeComponents()
{
	m_pcTransform = m_pcOwner->GetComponent<CTransform>();
	m_pcRigidbody = m_pcOwner->GetComponent<CRigidbody>();
	m_pcMeshRenderer = m_pcOwner->GetComponent<CMeshRenderer>();
	m_pcAnimator = m_pcOwner->GetComponent<CAnimatorComponent>();
	m_pcStats = m_pcOwner->GetComponent<CStats>();
	m_pcAgent = m_pcOwner->GetComponent<CPathAgent>();

	if(m_pcAnimator) SwitchAnimation(eEnemyAnimation::IDLE, false);
}

void IEnemyController::InitializeProjectile()
{
	m_pcProjectileRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);

	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::CAPSULE_COLLIDER);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::RIGIDBODY);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::MESH_RENDERER);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PROJECTILE_COMPONENT);

	m_pcProjectileRef->SetActiveState(false);
}

bool IEnemyController::RaycastToPlayer() const
{
	CMath::TVECTOR3 tOffset(0, 2, 0);

	CMath::TVECTOR3 tVectorToTarget = m_pcPlayerTransform->GetPosition() + tOffset - m_pcTransform->GetPosition() + tOffset;
	CMath::TVECTOR3 tDirection = CMath::Vector3Normalize(tVectorToTarget);
	float fMagnitude = CMath::Vector3Magnitude(tVectorToTarget);

	CEntityMask mask;
	mask.Add(eEntity::ENVIRONMENT);
	mask.Add(eEntity::PLAYER);

	TRayHit hit;
	if (CPhysicsManager::Raycast(m_pcTransform->GetPosition() + CMath::TVECTOR3(0, 2, 0), tDirection, hit, fMagnitude, &mask))
	{
		if (hit.tEntity->m_nEntityType == eEntity::PLAYER)
			return true;
	}

	return false;
}

bool IEnemyController::IsMoving() const
{
	CMath::TVECTOR3 tVelocity = m_pcRigidbody->GetVelocity();
	return CMath::Vector3Dot(tVelocity, tVelocity) > 0.0f;
}

bool IEnemyController::IsDead() const
{
	return m_pcStats->GetHP() <= 0;
}

float IEnemyController::GetDistanceToPlayer() const
{
	return CMath::Vector3Magnitude(m_pcTransform->GetPosition() - m_pcPlayerTransform->GetPosition());
}

void IEnemyController::SwitchAnimation(int nAnimation, bool bOverride, float fTime, float fSpeed)
{
	int nCurrent = m_pcAnimator->GetCurrentAnimation();
	float fCurrentSpeed = m_pcAnimator->GetSpeed();

	if (!bOverride && nCurrent == nAnimation && fSpeed == fCurrentSpeed)
		return;

	m_pcAnimator->SetAnimation(nAnimation);
	m_pcAnimator->SetAnimationTime(fTime);
	m_pcAnimator->SetSpeed(fSpeed);
}

void IEnemyController::Update(float fDeltaTime)
{
	if(m_fFlashingTime > 0.0f)
	{
		m_fFlashingTime -= CTime::GetDeltaF();

		if(m_fFlashingTime <= 0.0f)
		{
			if(IsDead())
			{
				m_pcMeshRenderer->SetPixelShader(ePixelShader::DISSOLVE);
			}
			else
			{
				m_pcMeshRenderer->SetPixelShader(ePixelShader::DEFAULT);
			}

		}
	}

	if(IsDead())
	{
		return;
	}

	CMath::TVECTOR3 vStart, vEnd;
	CInputManager::GetMouseWorldSpace({ 0,0 }, vStart, vEnd);
	TRayHit tHit;

	if (CPhysicsManager::Raycast(vStart, CMath::Vector3Normalize(m_pcTransform->GetPosition() - vStart), tHit, CMath::Vector3Magnitude(m_pcTransform->GetPosition() - vStart), nullptr))
	{
		bool bIsHidden = tHit.tEntity->m_nEntityType == eEntity::ENVIRONMENT || tHit.tEntity->m_nEntityType == eEntity::DOOR;
		m_pcMeshRenderer->SetGlowEffect(bIsHidden);
	}

}

void IEnemyController::GetPlayerReference()
{
	m_pcPlayerTransform = CEventManager::SendEntityMessage(TEntityMessage(0))->GetComponent<CTransform>();
	if (m_pcAgent) m_pcAgent->SetTarget(m_pcPlayerTransform);
}

float IEnemyController::GetSafeDistance()
{
	return m_fSafeDistance;
}

float IEnemyController::GetDeathTimer()
{
	return m_fDeathTimer;
}

float IEnemyController::GetFlashTime()
{
	return m_fFlashingTime;
}

void IEnemyController::ResetFlashTimer()
{
	m_fFlashingTime = 0.2f;
	m_pcMeshRenderer->SetPixelShader(ePixelShader::FLASH);
}

int IEnemyController::GetEnemyType()
{
	return m_nEnemyType;
}
