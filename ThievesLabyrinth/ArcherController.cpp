#include "ArcherController.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "AnimatorComponent.h"
#include "Time.h"
#include "MeshRenderer.h"
#include "CapsuleCollider.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "PhysicsManager.h"
#include "BoxCollider.h"
#include "PathAgent.h"
#include "ParticleEffect.h"

#include "EnumTypes.h"
#include "Stats.h"
#include "Entity.h"

#include <random>

#define MAX_DISTANCE 25.0f
#define MIN_DISTANCE 15.0f
#define PERFECT_DISTANCE 20.0f

CArcherController::CArcherController(IEntity* pcOwner) : IEnemyController(pcOwner)
{
	InitializeComponents();
	InitializeProjectile();

	m_nEnemyType = eEnemyType::ARCHER;

	m_bRestartCycle = true;
	m_fMovementTimer = 0.0f;
	m_fMaxMovementTime = 0.0f;

	m_fWaitTimer = 0.0f;
	m_fMaxWaitTime = 1.2f;

	m_fSafeDistance = 2.0f;
	m_fDeathTimer = 0.0f;
}

CArcherController::~CArcherController()
{
}

void CArcherController::Update(float fDeltaTime)
{
	IEnemyController::Update(fDeltaTime);

	if (IsDead())
	{
		m_pcAnimator->SetLoop(false);

		m_pcOwner->SetActiveState(false);
		m_pcMeshRenderer->SetActiveState(true);
		this->SetActiveState(true);

		m_pcRigidbody->SetVelocity({ 0,0,0 });
		
		m_fDeathTimer += fDeltaTime * 0.25f;

		return;
	}

	CMath::TVECTOR3 tPosition = m_pcTransform->GetPosition();
	CMath::TVECTOR3 tPlayerPosition = m_pcPlayerTransform->GetPosition();
	CMath::TVECTOR3 tDistanceVec = CMath::TVECTOR3(tPlayerPosition.x, 0, tPlayerPosition.z) - CMath::TVECTOR3(tPosition.x, 0, tPosition.z);
	CMath::TVECTOR3 tDirection = CMath::Vector3Normalize(tDistanceVec);
	float fDistance = CMath::Vector3Magnitude(tDistanceVec);
	float fSpeed = m_pcStats->GetMovementSpeed();

	m_pcTransform->TurnTo(m_pcPlayerTransform, fDeltaTime, 120.0f);

	if (m_bRestartCycle)
	{
		float random = CMath::RandomFloat(0.6f, 1.6f);
		float randomWait = CMath::RandomFloat(0.2f, 0.5f);
		m_fHorizontalDir = rand() % 2 == 0 ? -1.0f : 1.0f;
		m_fMaxMovementTime = random;
		m_fMaxWaitTime = randomWait;
		m_fMovementTimer = 0.0f;
		m_fWaitTimer = 0.0f;
		m_nAttackCount = 0;
		m_bIsAttacking = false;
		m_bHasAttacked = false;

		m_bRestartCycle = false;
	}

	if (fDistance > MAX_DISTANCE && !m_bIsAttacking)
	{
		m_pcAgent->Start();
		SwitchAnimation(eEnemyAnimation::WALK, false);
	}

	else if (m_fMovementTimer < m_fMaxMovementTime)
	{
		if (fDistance < MIN_DISTANCE)
		{
			m_pcAgent->Stop();
 			CMath::TVECTOR3 tOrigin = m_pcTransform->GetPosition() + CMath::TVECTOR3(0, 2, 0);
			CEntityMask mask;
			mask.Add(eEntity::ENVIRONMENT);

			TRayHit hit;
			if (!CPhysicsManager::Raycast(tOrigin, -tDirection, hit, 10.0f, &mask))
			{
				m_pcRigidbody->SetVelocity(tDirection * -fSpeed);
				SwitchAnimation(eEnemyAnimation::WALK, false);
			}

			else
			{
				m_pcRigidbody->SetVelocity({ 0,0,0 });
				SwitchAnimation(eEnemyAnimation::IDLE, false);
			}
		}

		else if (fDistance > PERFECT_DISTANCE)
		{
			m_pcRigidbody->SetVelocity(((*(CMath::TVECTOR3*)&m_pcTransform->GetMatrix().r[0] * m_fHorizontalDir) + tDirection) * fSpeed);
			SwitchAnimation(eEnemyAnimation::WALK, false);
		}
		else
		{
			m_pcRigidbody->SetVelocity(((*(CMath::TVECTOR3*)&m_pcTransform->GetMatrix().r[0] * m_fHorizontalDir) - tDirection) * fSpeed);
			SwitchAnimation(eEnemyAnimation::WALK, false);
		}

		m_fMovementTimer += fDeltaTime;

	}

	else if (m_nAttackCount < 3)
	{
		m_bIsAttacking = true;

		m_pcRigidbody->SetVelocity({ 0,0,0 });
		SwitchAnimation(eEnemyAnimation::ATTACK, false, 0.0f, m_pcStats->GetAttackSpeed());

		if (m_pcAnimator->GetCurrentAnimationTime() <= 1.7f)
		{
			m_bHasAttacked = false;
		}

		if (!m_bHasAttacked && m_pcAnimator->GetCurrentAnimationTime() > 1.7f)
		{
			Attack();
			m_nAttackCount++;
			m_bHasAttacked = true;
		}
	}

	else if (m_fWaitTimer < m_fMaxWaitTime)
	{
		SwitchAnimation(eEnemyAnimation::IDLE, false);
		m_pcRigidbody->SetVelocity({ 0,0,0 });
		m_fWaitTimer += fDeltaTime;
	}

	else
	{
		m_bRestartCycle = true;
	}
}

void CArcherController::Attack()
{
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::ARROW_WOOSH));
	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcProjectileRef);
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage()); //Modify damage based on items/stats/buffs/debuffs

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward(*(CMath::TVECTOR3*)&pcOwnerTransform->GetMatrix().r[2]);

	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition() + tForward * 1.5f + CMath::TVECTOR3{ 0,3,0 });
	pcProjTransform->SetRotation(pcOwnerTransform->GetRotation());

	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity(CMath::TVECTOR3(tForward) * 30);

	CParticleEmitter* pcEmitter = pcNewProjectile->GetComponent<CParticleEmitter>();

	pcEmitter->SetEmitterDirection(tForward);
	pcEmitter->RefreshEmitterSettings();

	pcNewProjectile->SetActiveState(true);

}

void CArcherController::InitializeProjectile()
{
	m_pcProjectileRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::TRANSFORM);

	CBoxCollider* pcProjCollider = (CBoxCollider*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::BOX_COLLIDER);
	pcProjCollider->SetCenter({0,0,0});
	pcProjCollider->SetExtent({ .2f,.2f, 1.0f });
	pcProjCollider->SetTrigger(true);

	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::RIGIDBODY);

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ARROW);
	pcMesh->SetIndexBuffer(eIndexBuffer::ARROW);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eSRV::ARROW_DIFFUSE, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::CLAMP);

	CParticleEmitter* pcEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PARTICLE_EMITTER);

	pcEmitter->SetMaxParticles(30);
	pcEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
	pcEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
	pcEmitter->SetTexture(eSRV::PARTICLE_SPARK, 0);
	pcEmitter->SetTexture(eSRV::NONE, 1);
	pcEmitter->SetTexture(eSRV::NONE, 2);
	pcEmitter->SetInputLayout(eInputLayout::DEFAULT);
	pcEmitter->SetVertexShader(eVertexShader::DEFAULT);
	pcEmitter->SetPixelShader(ePixelShader::PARTICLE);
	pcEmitter->SetSampler(eSampler::WRAP);

	pcEmitter->SetEmitterShape(eEmitter::CONE);
	pcEmitter->SetEmissionAngle(0.0f);
	pcEmitter->SetMaxSpeed(8.0f);
	pcEmitter->SetMinSpeed(15.0f);
	pcEmitter->SetLife(.5f);
	pcEmitter->SetOffset({ 0, 0, 0 });
	pcEmitter->SetLoop(true);
	pcEmitter->SetMaxRotationSpeed(2);
	pcEmitter->SetMinRotationSpeed(-2);

	pcEmitter->SetParticleStartScale(CMath::TVECTOR3(.2f, .2f, .5f));
	pcEmitter->SetParticleEndScale(CMath::TVECTOR3(.2f, .2f, .5f));

	pcEmitter->SetMaxExtent({ .2f, 0, .2f });
	pcEmitter->SetMinExtent({ -.2f, 0, -.2f });

	pcEmitter->SetParticlesPerEmission(4);
	pcEmitter->SetEmissionRate(.05f);

	CProjectileComponent* pcProjComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PROJECTILE_COMPONENT);
	pcProjComponent->SetDamage(0.5f);
	pcProjComponent->SetLifeTime(5.0f);
	pcProjComponent->SetCasterEntity(eEntity::ENEMY);
	m_pcProjectileRef->SetActiveState(false);
}

CMath::TVECTOR3 CArcherController::CalculateMoveDirection(CMath::TVECTOR3 tDirectionPlayer, float fDistancePlayer)
{
	if (fDistancePlayer > MAX_DISTANCE)
		return tDirectionPlayer;

	if (fDistancePlayer < MIN_DISTANCE)
		return -tDirectionPlayer;

	if (fDistancePlayer > PERFECT_DISTANCE)
	{
		return CMath::Vector3Normalize(*(CMath::TVECTOR3*)&m_pcTransform->GetMatrix().r[0] + tDirectionPlayer);
	}

	return CMath::Vector3Normalize(*(CMath::TVECTOR3*)&m_pcTransform->GetMatrix().r[0] - tDirectionPlayer);
}


CArcherController & CArcherController::operator=(CArcherController & cCopy)
{
	m_pcPlayerTransform = cCopy.m_pcPlayerTransform;
	m_pcProjectileRef = cCopy.m_pcProjectileRef;
	return *this;
}