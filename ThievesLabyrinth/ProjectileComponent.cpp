#include "ProjectileComponent.h"
#include "EntityManager.h"
#include "Time.h"
#include "ParticleEffect.h"
#include "EnumTypes.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"

CProjectileComponent::CProjectileComponent(IEntity* pcOwner) : IComponent(pcOwner)
{  
	m_nComponentType = eComponent::PROJECTILE_COMPONENT;
	m_fDamage = 0.0f;
	m_fTimeLeft = 10.0f;
	m_fCost = 0.0f;
	m_fRotationSpeed = 0.0f;

	m_pcProjectileTransform = m_pcOwner->GetComponent<CTransform>();
}

void CProjectileComponent::SetLifeTime(float fTime)
{
	m_fTimeLeft = fTime;
}

float CProjectileComponent::GetTimeLeft()
{
	return m_fTimeLeft;
}

void CProjectileComponent::SetDamage(float fDamage)
{
	m_fDamage = fDamage;
}

void CProjectileComponent::SetCasterEntity(int nType)
{
	m_nCasterEntityType = nType;
}

void CProjectileComponent::SetCost(float fManaCost)
{
	m_fCost = fManaCost;
}

void CProjectileComponent::SetRotationSpeed(float fRotationSpeed)
{
	m_fRotationSpeed = fRotationSpeed;
}

float CProjectileComponent::GetDamage()
{
	return m_fDamage;
}

int CProjectileComponent::GetCasterEntity()
{
	return m_nCasterEntityType;
}

float CProjectileComponent::GetCost()
{
	return m_fCost;
}

void CProjectileComponent::Update(float fDeltaTime)
{
	m_fTimeLeft -= fDeltaTime;

	if(m_fTimeLeft <= 0)
	{
		CEntityManager::AddEntityToDeletionQueue(m_pcOwner);
	}
	else
	{
		m_pcProjectileTransform->SetMatrix(CMath::MatrixXRotation(m_fRotationSpeed * -CTime::GetDeltaF()) * m_pcProjectileTransform->GetMatrix());
	}
}


void CProjectileComponent::Destroy(int nNewTexture)
{
	m_pcOwner->GetComponent<CMeshRenderer>()->SetActiveState(false);
	m_pcOwner->GetComponent<CRigidbody>()->SetVelocity({0,0,0});

	if(m_pcOwner->GetComponent<CCapsuleCollider>())
		m_pcOwner->GetComponent<CCapsuleCollider>()->SetActiveState(false);
	else
		m_pcOwner->GetComponent<CBoxCollider>()->SetActiveState(false);

	SetLifeTime(0.5f);

	m_pcOwner->GetComponent<CParticleEmitter>()->DisableOnCollision(nNewTexture);
}


CProjectileComponent& CProjectileComponent::operator=(CProjectileComponent & pCComponentToCopy)
{
	SetLifeTime(pCComponentToCopy.GetTimeLeft());
	SetDamage(pCComponentToCopy.GetDamage());
	SetCasterEntity(pCComponentToCopy.GetCasterEntity());
	SetCost(pCComponentToCopy.GetCost());
	SetRotationSpeed(pCComponentToCopy.m_fRotationSpeed);

	return *this;
}

CProjectileComponent::~CProjectileComponent()
{
}
