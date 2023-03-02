#include "ChickenController.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "AnimatorComponent.h"
#include "Time.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Level.h"
#include "LightComponent.h"
#include "ParticleEffect.h"

#include "EnumTypes.h"
#include "Stats.h"
#include "Entity.h"
#include "CapsuleCollider.h"
#include "PathAgent.h"


CChickenController::CChickenController(IEntity* pcOwner) : IEnemyController(pcOwner)
{
	m_nEnemyType = eEnemyType::CHICKEN;

	InitializeComponents();
	InitializeProjectile();
	m_pcEmitter = m_pcOwner->GetComponent<CParticleEmitter>();

	m_bExploding = false;
	m_tLastPos = CMath::TVECTOR3(0, 0, 0);

	m_fDeathTimer = 0.0f;
}

void CChickenController::Update(float fDeltaTime)
{
	IEnemyController::Update(fDeltaTime);

	// Make sure you aren't dead
	if (IsDead())
	{
	//	CComponentManager::DeleteComponent(eComponent::RIGIDBODY, m_pcRigidbody);
		m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0, 0, 0));
		m_pcStats->SetHP(-1);

		// Blow up if you haven't already
		if (!m_bExploding)
		{
			Explode();
		}

		return;
	}

	if(!m_pcEmitter->IsRunning())
	{
		m_pcEmitter->RefreshEmitterSettings();
	}

	// Where's the target at?
	m_tLastPos = m_pcTransform->GetPosition();
	CMath::TVECTOR3 tVectorToTarget = m_pcPlayerTransform->GetPosition() - m_tLastPos;
	m_pcAgent->Stop();
	// Are they close?
	if (CMath::Vector3Magnitude(tVectorToTarget) <= 3.0f)
	{
		// The time has come!
		m_pcOwner->m_pcRoom->m_nEnemyCount--;
		CLevel::UpdateDoors(m_pcOwner->m_pcRoom);
		CLevel::nTotalEnemyCount--;
		Explode();

		m_pcStats->SetHP(-1);

		m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0, 0, 0));
	}
	else
	{
		m_pcAgent->Start();
		// Get to the target!
		CMath::TVECTOR3 tNormalVelocity = CMath::Vector3Normalize(tVectorToTarget);
		CMath::TVECTOR3 tFinalVelocity = (tNormalVelocity * 0.5f) + m_pcRigidbody->GetVelocity();
		float fSpeed = m_pcStats->GetMovementSpeed();
		if (CMath::Vector3Magnitude(tFinalVelocity) > fSpeed)
		{
			tFinalVelocity = CMath::Vector3Normalize(tFinalVelocity) * fSpeed;
		}

		m_pcRigidbody->SetVelocity(tFinalVelocity);
		m_pcTransform->TurnTo(m_pcPlayerTransform, CTime::GetDeltaF());

		/*if (m_pcAnimator->GetCurrentAnimation() != eEnemyAnimation::WALK)
		{
			m_pcAnimator->SetAnimation(eEnemyAnimation::WALK);
			m_pcAnimator->SetAnimationTime(0.0);

		}*/
	}
}

CChickenController& CChickenController::operator=(CChickenController& cCopy)
{
	m_pcPlayerTransform = cCopy.m_pcPlayerTransform;

	return *this;
}

CChickenController::~CChickenController()
{
	// An explosive chicken leaves nothing behind
	// and nothing to leave behind
	// Blow up if you aren't already
	if (!m_bExploding)
	{
	//	Explode();
	}
}

void CChickenController::InitializeProjectile()
{
	m_pcProjectileRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::TRANSFORM);

	CCapsuleCollider* pcAOECollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::CAPSULE_COLLIDER);
	pcAOECollider->SetHeight(2.0f);
	pcAOECollider->SetRadius(4.0f);
	pcAOECollider->SetTrigger(true);

	CLightComponent* pcAOELight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::LIGHT);

	CProjectileComponent* pcAOEComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PROJECTILE_COMPONENT);
	pcAOEComponent->SetDamage(1.0f);
	pcAOEComponent->SetLifeTime(0.1f);
	pcAOEComponent->SetCasterEntity(eEntity::ENVIRONMENT);

	pcAOELight->SetColor({ 1, 0, 0, 1 });
	pcAOELight->SetPosition({ 0, 1, 0 });
	pcAOELight->SetRadius(30.0f);

	m_pcProjectileRef->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage());

	m_pcProjectileRef->SetActiveState(false);
}

void CChickenController::Explode()
{
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::EXPLOSION));
	CEventManager::SendAudioMessage(TAudioMessage(false, eAudio::SFX, eSFX::FUSE));
	m_pcProjectileRef->GetComponent<CTransform>()->SetPosition(m_tLastPos);
	m_pcProjectileRef->m_pcRoom = m_pcOwner->m_pcRoom;
	m_bExploding = true;
	m_pcProjectileRef->SetActiveState(true);



	//CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcProjectileRef);
	//pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage());
	//pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;
	//CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();

	//pcProjTransform->SetPosition(m_tLastPos);

	//pcNewProjectile->SetActiveState(true);

	CLightComponent* pcProjLight = m_pcProjectileRef->GetComponent<CLightComponent>();

	if (pcProjLight)
	{
		if (m_pcOwner->m_pcRoom)
		{
			m_pcOwner->m_pcRoom->AddLight(pcProjLight);
		}
	}

	m_pcEmitter->SetTexture(eSRV::EXPLOSION_PARTICLE, 0);
	m_pcEmitter->SetEmissionAngle(90);
	m_pcEmitter->SetMinSpeed(3);
	m_pcEmitter->SetMaxSpeed(6);
	m_pcEmitter->SetMaxParticles(30);
	m_pcEmitter->SetParticleStartScale({ 1,1,.5F });
	m_pcEmitter->SetParticleEndScale({ 10, 10, .5f });
	m_pcEmitter->SetMaxExtent({ 1, 0, 1 });
	m_pcEmitter->SetMinExtent({ -1, 0, -1 });

	m_pcEmitter->SetLoop(false);
	m_pcEmitter->SetParticlesPerEmission(30);
	m_pcEmitter->RefreshEmitterSettings();

	m_pcEmitter->SetMaxRotationSpeed(2.0f);
	m_pcEmitter->SetMinRotationSpeed(-2.0f);

	CParticleEmitter* m_pcFeathersEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(m_pcOwner, eComponent::PARTICLE_EMITTER);

	m_pcFeathersEmitter->SetEmitterShape(eEmitter::RADIAL);
	m_pcFeathersEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
	m_pcFeathersEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
	m_pcFeathersEmitter->SetInputLayout(eInputLayout::DEFAULT);
	m_pcFeathersEmitter->SetVertexShader(eVertexShader::DEFAULT);
	m_pcFeathersEmitter->SetPixelShader(ePixelShader::PASS_2D);
	m_pcFeathersEmitter->SetSampler(eSampler::WRAP);
	m_pcFeathersEmitter->SetTexture(eSRV::FEATHER_PARTICLE, 0);
	m_pcFeathersEmitter->SetTexture(eSRV::NONE, 1);
	m_pcFeathersEmitter->SetTexture(eSRV::NONE, 2);
	m_pcFeathersEmitter->SetEmissionAngle(90);
	m_pcFeathersEmitter->SetMinSpeed(3);
	m_pcFeathersEmitter->SetMaxSpeed(4);
	m_pcFeathersEmitter->SetMaxParticles(5);
	m_pcFeathersEmitter->SetParticleStartScale({ 1.0f, 1.0f,.5F });
	m_pcFeathersEmitter->SetParticleEndScale({ 1.0f, 1.0f, .5f });
	m_pcFeathersEmitter->SetMaxExtent({ 2, 0, 2 });
	m_pcFeathersEmitter->SetMinExtent({ -2, 0, -2 });
	m_pcFeathersEmitter->SetLife(.5f);
	m_pcFeathersEmitter->SetOffset({ 0, 1.5f, 0.0f });

	m_pcFeathersEmitter->SetLoop(false);
	m_pcFeathersEmitter->SetParticlesPerEmission(5);
	m_pcFeathersEmitter->SetEmissionRate(0.1f);

	m_pcFeathersEmitter->SetMinInitialRotation(-180.0f);
	m_pcFeathersEmitter->SetMaxInitialRotation(180.0f);
	m_pcFeathersEmitter->SetMaxRotationSpeed(10.0f);
	m_pcFeathersEmitter->SetMinRotationSpeed(-10.0f);
	m_pcFeathersEmitter->RefreshEmitterSettings();


	m_pcOwner->SetActiveState(false);
	m_pcEmitter->SetActiveState(true);
	m_pcFeathersEmitter->SetActiveState(true);
}
