#include "MageController.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "AnimatorComponent.h"
#include "PhysicsManager.h"
#include "Time.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "PathAgent.h"
#include "ParticleEffect.h"
#include "LightComponent.h"

#include "EnumTypes.h"
#include "Stats.h"
#include "Entity.h"

#include <random>

CMageController::CMageController(IEntity* pcOwner) : IEnemyController(pcOwner)
{
	InitializeComponents();
	InitializeProjectile();
	InitializeTimers();

	m_nEnemyType = eEnemyType::MAGE;

	m_fSafeDistance = 2.0f;
	m_fDeathTimer = 0.0f;

	m_bHasAttacked = false;
	m_bCanSeePlayer = false;
	m_bTeleportDone = false;
	m_bTeleportStart = false;

}

CMageController::~CMageController()
{
}

void CMageController::Update(float fDeltaTime)
{
	IEnemyController::Update(fDeltaTime);

	if (!IsDead())
	{
		if (m_fTeleportTimer < m_fMaxTeleportTime)
		{
			if (m_bCanSeePlayer)
			{
				m_pcTransform->TurnTo(m_pcPlayerTransform, fDeltaTime, 1000);

				if (!m_bHasAttacked)
				{
					if (!m_bIsAttacking)
					{
						SwitchAnimation(eEnemyAnimation::ATTACK, true, 0.0f, 0.8f);

						m_fAttackTimer = 0.0f;
						m_bIsAttacking = true;
					}
				}

				if (m_bIsAttacking && m_fAttackTimer > 0.35f / 0.8f)
				{
					Attack();
					m_bHasAttacked = true;
					m_bIsAttacking = false;
				}

				if (m_fAttackTimer > m_fMaxAttackTime)
				{
					SwitchAnimation(eEnemyAnimation::IDLE, false);
				}
			}

			if (m_fSightCheckTimer >= m_fMaxSightCheckTime)
			{
				m_bCanSeePlayer = RaycastToPlayer();
				m_fSightCheckTimer = 0.0f;
			}

			UpdateTimers(fDeltaTime);
		}

		else 
		{
			if (!m_bTeleportDone)
			{
				SwitchAnimation(eEnemyAnimation::IDLE, false);

				if (m_pcOwner->GetComponent<CMeshRenderer>()->GetPixelShader() != ePixelShader::DISSOLVE)
					m_pcOwner->GetComponent<CMeshRenderer>()->SetPixelShader(ePixelShader::DISSOLVE);

				m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0, 0, 0));
				m_fDeathTimer += fDeltaTime * 0.25f;

				if (m_fDeathTimer > 0.7f)
				{
					Teleport();
					m_bTeleportDone = true;
				}
			}

			else
			{
				m_fDeathTimer -= fDeltaTime * 0.25f;

				if (m_fDeathTimer <= 0.3f)
				{
					ZeroTimers();
					m_bHasAttacked = false;
					m_bTeleportDone = false;
					m_pcOwner->GetComponent<CMeshRenderer>()->SetPixelShader(ePixelShader::DEFAULT);
				}
			}
		}
	}

	else // if is dead
	{		
			BeginDeath();
		
			m_fDeathTimer += fDeltaTime * 0.25f;
		
	}
}

void CMageController::Attack()
{
	int nRand = rand() % (eSFX::FIREBALL2 - eSFX::FIREBALL1 + 1) + eSFX::FIREBALL1;
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, nRand));

	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcProjectileRef);
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage()); //Modify damage based on items/stats/buffs/debuffs

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward(*(CMath::TVECTOR3*)&pcOwnerTransform->GetMatrix().r[2]);

	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition() + tForward * 1.5f + CMath::TVECTOR3{0,3,0});
	pcProjTransform->SetRotation(pcOwnerTransform->GetRotation());

	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity(CMath::TVECTOR3(tForward) * 40);

	CParticleEmitter* pcEmitter = pcNewProjectile->GetComponent<CParticleEmitter>();

	pcEmitter->SetEmitterDirection(tForward);
	pcEmitter->SetMaxInitialRotation(m_pcTransform->GetRotation().y);
	pcEmitter->SetMinInitialRotation(m_pcTransform->GetRotation().y);

	pcEmitter->RefreshEmitterSettings();

	m_pcOwner->m_pcRoom->AddLight(pcNewProjectile->GetComponent<CLightComponent>());

	pcNewProjectile->SetActiveState(true);

	m_bIsAttacking = false;

}

void CMageController::InitializeTimers()
{
	ZeroTimers();

	m_fMaxSightCheckTime = 0.3f;
	m_fMaxTeleportTime = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
	m_fMaxAttackTime = 0.45f / 0.8f;
}

void CMageController::ZeroTimers()
{
	m_fDeathTimer = 0.0;
	m_fAttackTimer = 0.0f;
	m_fSightCheckTimer = 0.0f;
	m_fTeleportTimer = 0.0f;
}

void CMageController::UpdateTimers(float fDeltaTime)
{
	m_fSightCheckTimer += fDeltaTime;
	m_fTeleportTimer += fDeltaTime;
	m_fAttackTimer += fDeltaTime;
}

void CMageController::InitializeProjectile()
{
	m_pcProjectileRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CTransform* pcTransform = (CTransform*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::TRANSFORM);
	pcTransform->SetScale(CMath::TVECTOR3(2, 2, 2));

	CCapsuleCollider* pcProjCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::CAPSULE_COLLIDER);
	pcProjCollider->SetRadius(1.0f);
	pcProjCollider->SetHeight(2.1f);
	pcProjCollider->SetTrigger(true);

	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::RIGIDBODY);

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::SPHERE);
	pcMesh->SetIndexBuffer(eIndexBuffer::SPHERE);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eSRV::MAGE_PROJECTILE_DIFFUSE, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::CLAMP);

	CProjectileComponent* pcProjComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PROJECTILE_COMPONENT);
	pcProjComponent->SetDamage(10.0f);
	pcProjComponent->SetLifeTime(5.0f);
	pcProjComponent->SetCasterEntity(eEntity::ENEMY);
	pcProjComponent->SetRotationSpeed(20.0f);

	CLightComponent* pcProjLight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::LIGHT);

	pcProjLight->SetColor({ 109.0f / 255.0f, 17.0f / 255.0f, 39 / 145.0f, 1 });
	pcProjLight->SetPosition({ 0, 0, 0 });
	pcProjLight->SetRadius(15.0f);

	CParticleEmitter* pcEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PARTICLE_EMITTER);

	pcEmitter->SetMaxParticles(30);
	pcEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
	pcEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
	pcEmitter->SetTexture(eSRV::ARCANE_PARTICLE, 0);
	pcEmitter->SetTexture(eSRV::NONE, 1);
	pcEmitter->SetTexture(eSRV::NONE, 2);
	pcEmitter->SetInputLayout(eInputLayout::DEFAULT);
	pcEmitter->SetVertexShader(eVertexShader::DEFAULT);
	pcEmitter->SetPixelShader(ePixelShader::PARTICLE);
	pcEmitter->SetSampler(eSampler::WRAP);

	pcEmitter->SetEmitterShape(eEmitter::CONE);
	pcEmitter->SetEmissionAngle(0.0f);
	pcEmitter->SetMaxSpeed(-5.0f);
	pcEmitter->SetMinSpeed(-10.0f);
	pcEmitter->SetLife(.5f);
	pcEmitter->SetOffset({ 0, 0, 0 });
	pcEmitter->SetLoop(true);
	pcEmitter->SetMaxRotationSpeed(2);
	pcEmitter->SetMinRotationSpeed(-2);

	pcEmitter->SetParticleStartScale(CMath::TVECTOR3(4, 4, .5f));
	pcEmitter->SetParticleEndScale(CMath::TVECTOR3(0.5f, 0.5f, .5f));

	pcEmitter->SetMaxExtent({ .7f, 0, .7f });
	pcEmitter->SetMinExtent({ -.7f, 0, -.7f });

	pcEmitter->SetParticlesPerEmission(4);
	pcEmitter->SetEmissionRate(.05f);
	m_pcProjectileRef->SetActiveState(false);
}

void CMageController::SetPathNavigation(bool bNavigate)
{
	if (bNavigate)
	{
		m_pcAgent->Start();
	}

	else
	{
		m_pcAgent->Stop();
	}
}

void CMageController::Teleport()
{
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::TELEPORT));
	CMath::TVECTOR3 tNewDir;
	float fDistanceHit = 0;
	int hitCounter = 10;

	do
	{
		float fRotation = (float)rand() / (float)RAND_MAX * 3.14159f * 2.0f;
		tNewDir = CMath::Vector3Transform(CMath::TVECTOR3(0, 0, 1), CMath::MatrixYRotation(fRotation));

		TRayHit hit;
		CPhysicsManager::Raycast(m_pcTransform->GetPosition() + CMath::TVECTOR3(0, 2, 0), tNewDir, hit, 1000);

		fDistanceHit = CMath::Vector3Magnitude(m_pcTransform->GetPosition() - hit.tEntryPoint);
	} while (fDistanceHit <= 5.0f && --hitCounter > 0);

	float fMagnitude = (float)rand() / (float)RAND_MAX * fDistanceHit - 6.0f;
	fMagnitude += 3.0f;

	m_pcTransform->SetPosition(m_pcTransform->GetPosition() + tNewDir * fMagnitude);
	m_pcTransform->LookAt(CMath::Vector3Normalize(m_pcPlayerTransform->GetPosition() - m_pcTransform->GetPosition()));
}
void CMageController::BeginDeath()
{
	//m_fDeathTimer = 0.0f;

	//SwitchAnimation(eEnemyAnimation::DEATH, false);
	m_pcAnimator->SetLoop(false);

	m_pcOwner->SetActiveState(false);
	m_pcOwner->GetComponent<CMeshRenderer>()->SetActiveState(true);
	m_pcOwner->GetComponent<CMageController>()->SetActiveState(true);

	m_pcRigidbody->SetVelocity({ 0,0,0 });
}

CMageController& CMageController::operator=(CMageController & cCopy)
{
	m_pcPlayerTransform = cCopy.m_pcPlayerTransform;
	m_pcProjectileRef = cCopy.m_pcProjectileRef;

	return *this;
}