#include "FireDemonController.h"

#include "PhysicsManager.h"
#include "InputManager.h"
#include "EventManager.h"

#include "Time.h"
#include "Entity.h"
#include "LightComponent.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "Stats.h"
#include "MeshRenderer.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "AnimatorComponent.h"
#include "ParticleEffect.h"
#include "PathAgent.h"

#define ATTACK_SPEED_MULTIPLIER 1.5f

#define BASIC_TOTAL_TIME 0.8f / ATTACK_SPEED_MULTIPLIER
#define BASIC_START_TIME 0.3f / ATTACK_SPEED_MULTIPLIER

CFireDemonController::CFireDemonController(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::FIRE_DEMON_CONTROLLER;
	m_fCooldownMultiplier = 1.0f;
	m_fAttackSpeed = 1.0f;

	InitComponents();
	InitProjectiles();
	InitIndicator();

	ResetTimers();
	ResetMultipliers();

	m_bIsAttacking = false;
	m_bIsAttackingSpecial[0] = false;
	m_bIsAttackingSpecial[1] = false;
	m_bIsAttackingSpecial[2] = false;
	m_bMoving = false;
	m_bMouseControls = false;
	m_bCheckFloorRay = true;
	m_bIsNavigating = false;
	m_bPrevFrameClick = false;

	m_bHoverItem = false;

	m_fFireballCooldown = 7.0f;
	m_fAOECooldown = 10.0f;
	m_fFirewallCooldown = 15.0f;
	m_tBuffTimes = { 0,0,0 };
	m_tMaxBuffTimes = { 20,20,20 };
}

void CFireDemonController::Update(float fDeltaTime)
{
	// increase mana over time
	m_pcStats->SetMP(m_pcStats->GetMP() + fDeltaTime * 0.025f * m_pcStats->GetMaxMP());


	UpdateTimers(fDeltaTime);


	if (m_bIsAttacking)
	{
		if (m_fBasicAttacktime < BASIC_START_TIME)
		{
			ShootBasic();
			m_bIsAttacking = false;
			m_pcRigidbody->SetVelocity({ 0,0,0 });
		}
	}

	else if (m_bIsAttackingSpecial[0])
	{
		/*float fCurrentTime = (float)m_pcAnimator->GetCurrentAnimationTime();

		if (fCurrentTime > 1.1f)
			m_bIsAttackingSpecial[0] = false;

		else if (fCurrentTime > 0.6f && m_fFireBallAttackTime <= 0.0f)
			FireballAttack();*/

		m_pcRigidbody->SetVelocity({ 0,0,0 });
	}


	else if (m_bIsAttackingSpecial[1])
	{
		/*float fCurrentTime = (float)m_pcAnimator->GetCurrentAnimationTime();

		if (fCurrentTime > 0.85f)
			m_bIsAttackingSpecial[1] = false;*/

		m_pcRigidbody->SetVelocity({ 0,0,0 });
	}

	else if (m_bIsAttackingSpecial[2])
	{
		/*float fCurrentTime = (float)m_pcAnimator->GetCurrentAnimationTime();

		if (fCurrentTime > 0.7f)
			m_bIsAttackingSpecial[2] = false;*/

		m_pcRigidbody->SetVelocity({ 0,0,0 });
	}

	else
	{
		if (!m_bMouseControls)
		{
			if (!CInputManager::GetKeyDown(VK_SPACE))
			{
				KeyboardMovement();
			}

			else
			{
				BeginBasic(GetFloorPosition());
				m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0,0,0));
			}
		}

		else
		{
			/*
			if player is not attacking
				if mouse click down
					if clicked on enemy
						begin BASIC attack
						is now attacking BASIC
				else if mouse click hold
					go towards mouse click
			else
				if player attack basic
					if timer ready
						shoot basic projectile
						is not attacking anymore
			*/

			if (CInputManager::GetKeyDown(VK_RBUTTON))
			{
				BeginBasic(GetFloorPosition());
				m_bIsAttacking = true;
				m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0, 0, 0));
				m_bIsNavigating = false;
			}

			else if (CInputManager::GetKeyDown(VK_LBUTTON))
			{
				m_bIsNavigating = false;
				m_bPrevFrameClick = true;
			}

			else if (CInputManager::GetKey(VK_LBUTTON))
			{
				MoveToMouseDirection();

				m_bIsNavigating = false;
				m_bPrevFrameClick = true;
			}

			// TODO: Input Manager GetKeyUp
			else if (m_bPrevFrameClick)
			{
				m_bIsNavigating = true;
				m_tClickPos = GetFloorPosition();
				
				InstantiateIndicator(m_tClickPos);

				m_bPrevFrameClick = false;
			}

			else if(!m_bIsNavigating)
			{
				m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0, 0, 0));
				m_pEnemyTransform = nullptr;

				/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::IDLE)
				{
					m_pcAnimator->SetAnimation(ePlayerAnimation::IDLE);
					m_pcAnimator->SetAnimationTime(0.0);
					m_pcAnimator->SetSpeed(1.0f);
					m_pcAnimator->SetLoop(true);
				}*/
			}

			if (m_bIsNavigating)
			{
				/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::WALK)
				{
					m_pcAnimator->SetAnimation(ePlayerAnimation::WALK);
					m_pcAnimator->SetAnimationTime(0.0f);
					m_pcAnimator->SetSpeed(1.8f);
					m_pcAnimator->SetLoop(true);
				}*/

				CMath::TVECTOR3 tOrigin = m_pcTransform->GetPosition() + CMath::TVECTOR3(0, 2, 0);
				CMath::TVECTOR3 tDirection = m_tClickPos + CMath::TVECTOR3(0,2,0) - tOrigin;
				CEntityMask cMask;
				cMask.Add(eEntity::ENVIRONMENT);
				TRayHit tHit;

				if (CMath::Vector3Magnitude(m_pcTransform->GetPosition() - m_tClickPos) < 0.5f ||
					CPhysicsManager::Raycast(tOrigin, tDirection, tHit, 3.0f, &cMask))
				{
					m_bIsNavigating = false;
					m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0, 0, 0));
				}

				else
				{
					m_pcRigidbody->SetVelocity(CMath::Vector3Normalize(m_tClickPos - m_pcTransform->GetPosition()) * m_pcStats->GetMovementSpeed() * m_fSpeedMultiplier);
					m_pcTransform->TurnTo(CMath::Vector3Normalize(m_tClickPos - m_pcTransform->GetPosition()));
				}
			}
		}
	}

	bool bUsingSpecial = m_bIsAttackingSpecial[0] || m_bIsAttackingSpecial[1] || m_bIsAttackingSpecial[2];
	if (!bUsingSpecial)
	{
		if (m_bMouseControls)
		{

			if (CInputManager::GetKeyDown('Q'))
			{
				if (m_fFireBallAttackTime <= 0)
				{
					// The spell's cost should be used instead of 0
					if (m_pcStats->GetMP() >= m_pcBasicAttackRef->GetComponent<CProjectileComponent>()->GetCost())
					{
						CMath::TVECTOR3 tFloorPosition = GetFloorPosition();
						m_pcTransform->LookAt(tFloorPosition);

						/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::FIREBALL)
						{
							m_pcAnimator->SetAnimation(ePlayerAnimation::FIREBALL);
							m_pcAnimator->SetAnimationTime(0.0);
							m_pcAnimator->SetSpeed(1.0f);
							m_pcAnimator->SetLoop(false);
						}*/
						m_bIsAttackingSpecial[0] = true;
						m_bIsNavigating = false;
					}
				}
			}

			else if (CInputManager::GetKeyDown('W'))
			{
				if (m_fAoeAttackTime <= 0)
				{
					// The spell's cost should be used instead of 0
					if (m_pcStats->GetMP() >= m_pcAOERef->GetComponent<CProjectileComponent>()->GetCost())
					{
						CMath::TVECTOR3 tPlayerPosition = m_pcTransform->GetPosition();
						CMath::TVECTOR3 tFloorPosition = GetFloorPosition();

						if (CMath::Vector3Magnitude(tFloorPosition - tPlayerPosition) < 25.0f)
						{
							m_pcTransform->LookAt(tFloorPosition);

							/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::AOE)
							{
								m_pcAnimator->SetAnimation(ePlayerAnimation::AOE);
								m_pcAnimator->SetAnimationTime(0.0);
								m_pcAnimator->SetSpeed(1.0f);
								m_pcAnimator->SetLoop(false);
							}*/

							AOE(tFloorPosition - CMath::TVECTOR3(0, 10, 0));
							m_bIsAttackingSpecial[1] = true;
							m_bIsNavigating = false;
						}
					}
				}
			}

			else if (CInputManager::GetKeyDown('E'))
			{
				if (m_fFirewallAttackTime <= 0)
				{
					// The spell's cost should be used instead of 0
					if (m_pcStats->GetMP() >= m_pcFirewallRef->GetComponent<CProjectileComponent>()->GetCost())
					{
						CMath::TVECTOR3 tPlayerPosition = m_pcTransform->GetPosition();
						CMath::TVECTOR3 tFloorPosition = GetFloorPosition();

						if (CMath::Vector3Magnitude(tFloorPosition - tPlayerPosition) < 25.0f)
						{
							m_pcTransform->LookAt(tFloorPosition);

							/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::FIREWALL)
							{
								m_pcAnimator->SetAnimation(ePlayerAnimation::FIREWALL);
								m_pcAnimator->SetAnimationTime(0.0);
								m_pcAnimator->SetSpeed(1.0f);
								m_pcAnimator->SetLoop(false);
							}*/

							Firewall(tFloorPosition);
							m_bIsAttackingSpecial[2] = true;
							m_bIsNavigating = false;
						}
					}
				}
			}
		}

		else
		{
			if (CInputManager::GetKeyDown('1'))
			{
				if (m_fFireBallAttackTime <= 0)
				{
					// The spell's cost should be used instead of 0
					if (m_pcStats->GetMP() >= m_pcBasicAttackRef->GetComponent<CProjectileComponent>()->GetCost())
					{
						CMath::TVECTOR3 tFloorPosition = GetFloorPosition();
						m_pcTransform->LookAt(tFloorPosition);
						/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::FIREBALL)
						{
							m_pcAnimator->SetAnimation(ePlayerAnimation::FIREBALL);
							m_pcAnimator->SetAnimationTime(0.0);
							m_pcAnimator->SetSpeed(1.0f);
						}*/
						m_bIsAttackingSpecial[0] = true;
					}
				}
			}

			else if (CInputManager::GetKeyDown('2'))
			{
				if (m_fAoeAttackTime <= 0)
				{
					// The spell's cost should be used instead of 0
					if (m_pcStats->GetMP() >= m_pcAOERef->GetComponent<CProjectileComponent>()->GetCost())
					{
						CMath::TVECTOR3 tPlayerPosition = m_pcTransform->GetPosition();
						CMath::TVECTOR3 tFloorPosition = GetFloorPosition();

						if (CMath::Vector3Magnitude(tFloorPosition - tPlayerPosition) < 25.0f)
						{
							m_pcTransform->LookAt(tFloorPosition);

							/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::AOE)
							{
								m_pcAnimator->SetAnimation(ePlayerAnimation::AOE);
								m_pcAnimator->SetAnimationTime(0.0);
								m_pcAnimator->SetSpeed(1.0f);
								m_pcAnimator->SetLoop(false);
							}*/

							AOE(tFloorPosition - CMath::TVECTOR3(0, 10, 0));
							m_bIsAttackingSpecial[1] = true;
						}
					}
				}
			}

			else if (CInputManager::GetKeyDown('3'))
			{
				if (m_fFirewallAttackTime <= 0)
				{
					// The spell's cost should be used instead of 0
					if (m_pcStats->GetMP() >= m_pcFirewallRef->GetComponent<CProjectileComponent>()->GetCost())
					{
						CMath::TVECTOR3 tPlayerPosition = m_pcTransform->GetPosition();
						CMath::TVECTOR3 tFloorPosition = GetFloorPosition();

						if (CMath::Vector3Magnitude(tFloorPosition - tPlayerPosition) < 25.0f)
						{
							m_pcTransform->LookAt(tFloorPosition);

							/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::FIREWALL)
							{
								m_pcAnimator->SetAnimation(ePlayerAnimation::FIREWALL);
								m_pcAnimator->SetAnimationTime(0.0);
								m_pcAnimator->SetSpeed(1.0f);
								m_pcAnimator->SetLoop(false);
							}*/

							Firewall(tFloorPosition);
							m_bIsAttackingSpecial[2] = true;
						}
					}
				}
			}
		}
	}

	Inventory();


	if(m_fFlashingTime > 0.0f)
	{
		m_fFlashingTime -= CTime::GetDeltaF();

		if(m_fFlashingTime <= 0.0f)
		{
			m_pcMeshRenderer->SetPixelShader(ePixelShader::DEFAULT);
		}
	}
}

void CFireDemonController::ShootBasic()
{
	//int nRand = rand() % (eSFX::FIREBALL2 - eSFX::FIREBALL1 + 1) + eSFX::FIREBALL1;
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::FIREBALL2));

	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcBasicAttackRef);
	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage() * m_fDamageMultiplier); //Modify damage based on items/stats/buffs/debuffs
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward(*(CMath::TVECTOR3*)&pcOwnerTransform->GetMatrix().r[2]);

	pcProjTransform->SetMatrix(pcOwnerTransform->GetMatrix());
	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition() + CMath::Vector3Normalize(tForward) * 1.5f + CMath::TVECTOR3(0, 3, 0));
	pcProjTransform->SetScale({ .7f, .7f, .7f });

	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity(tForward * 30);

	CParticleEmitter* pcEmitter = pcNewProjectile->GetComponent<CParticleEmitter>();

	pcEmitter->SetEmitterDirection(tForward);
	pcEmitter->SetMaxInitialRotation(m_pcTransform->GetRotation().y);

	pcEmitter->RefreshEmitterSettings();


	pcNewProjectile->SetActiveState(true);

	CLightComponent* pcProjLight = pcNewProjectile->GetComponent<CLightComponent>();


	/*if (pcProjLight)
	{
		m_pcOwner->m_pcRoom->AddLight(pcProjLight);
	}*/
}

void CFireDemonController::FireballAttack()
{
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, rand() % (eSFX::FIREBALL2 - eSFX::FIREBALL1 + 1) + eSFX::FIREBALL1));

	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcFireballRef);
	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage() * 3 * m_fDamageMultiplier); //Modify damage based on items/stats/buffs/debuffs
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward(*(CMath::TVECTOR3*)&pcOwnerTransform->GetMatrix().r[2]);

	pcProjTransform->SetMatrix(pcOwnerTransform->GetMatrix());
	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition() + CMath::Vector3Normalize(tForward) * 1.5f + CMath::TVECTOR3(0, 3, 0));
	pcProjTransform->SetScale({ 3.0f, 3.0f, 3.0f });
	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity(tForward * 30);

	CParticleEmitter* pcEmitter = pcNewProjectile->GetComponent<CParticleEmitter>();

	pcEmitter->SetEmitterDirection(-tForward);

	pcEmitter->RefreshEmitterSettings();

	pcNewProjectile->SetActiveState(true);

	m_fFireBallAttackTime = m_fFireballCooldown;

	m_pcStats->SetMP(m_pcStats->GetMP() - pcNewProjectile->GetComponent<CProjectileComponent>()->GetCost());

	CLightComponent* pcProjLight = pcNewProjectile->GetComponent<CLightComponent>();

	/*if (pcProjLight)
	{
		m_pcOwner->m_pcRoom->AddLight(pcProjLight);
	}*/

}

void CFireDemonController::AOE(CMath::TVECTOR3 vPosition)
{
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::FIREBALL2));
	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcAOERef);
	CProjectileComponent* pcProjComp = pcNewProjectile->GetComponent<CProjectileComponent>();
	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity({ 0, 20, 0 });
	pcProjComp->SetDamage(m_pcStats->GetBaseDamage() * 2.0f * m_fDamageMultiplier);
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	pcNewProjectile->GetComponent<CTransform>()->SetPosition(vPosition);

	pcNewProjectile->SetActiveState(true);

	m_fAoeAttackTime = m_fAOECooldown;

	m_pcStats->SetMP(m_pcStats->GetMP() - pcProjComp->GetCost());

	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CLightComponent* pcProjLight = pcNewProjectile->GetComponent<CLightComponent>();

	/*if (pcProjLight)
	{
		m_pcOwner->m_pcRoom->AddLight(pcProjLight);
	}*/
}

void CFireDemonController::Firewall(CMath::TVECTOR3 vPosition)
{
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::FIREWALL));
	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcFirewallRef);
	CProjectileComponent* pcProjComp = pcNewProjectile->GetComponent<CProjectileComponent>();
	pcProjComp->SetDamage(m_pcStats->GetBaseDamage() * 2.0f * m_fDamageMultiplier);
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();

	pcProjTransform->SetRotation(m_pcOwner->GetComponent<CTransform>()->GetRotation());
	pcProjTransform->SetPosition(vPosition);

	pcNewProjectile->SetActiveState(true);

	m_fFirewallAttackTime = m_fFirewallCooldown;

	m_pcStats->SetMP(m_pcStats->GetMP() - pcProjComp->GetCost());

	CLightComponent* pcProjLight = pcNewProjectile->GetComponent<CLightComponent>();

	/*if (pcProjLight)
	{
		m_pcOwner->m_pcRoom->AddLight(pcProjLight);
	}*/

	CParticleEmitter* pcEmitter = pcNewProjectile->GetComponent<CParticleEmitter>();

	pcEmitter->RefreshEmitterSettings();
}

void CFireDemonController::InitProjectiles()
{
	InitBasicProjectile();
	InitFireball();
	InitAOEProj();
	InitFireWall();
}

float CFireDemonController::GetFlashTime()
{
	return m_fFlashingTime;
}

void CFireDemonController::ResetFlashTimer()
{
	m_fFlashingTime = .2f;
	m_pcMeshRenderer->SetPixelShader(ePixelShader::FLASH);
}

bool CFireDemonController::IsOverItem()
{
	return m_bHoverItem;
}

void CFireDemonController::OverItem(bool bConfirm)
{
	m_bHoverItem = bConfirm;
}

void CFireDemonController::InitComponents()
{
	m_pEnemyTransform = nullptr;
	m_pcRigidbody = this->m_pcOwner->GetComponent<CRigidbody>();
	m_pcTransform = this->m_pcOwner->GetComponent<CTransform>();
	m_pcStats = this->m_pcOwner->GetComponent<CStats>();
	m_pcAnimator = this->m_pcOwner->GetComponent<CAnimatorComponent>();
	m_pcInventory = this->m_pcOwner->GetComponent<CInventory>();
	m_pcAgent = this->m_pcOwner->GetComponent<CPathAgent>();
	m_pcEmitter = this->m_pcOwner->GetComponent<CParticleEmitter>();
	m_pcMeshRenderer = this->m_pcOwner->GetComponent<CMeshRenderer>();
}

void CFireDemonController::InitBasicProjectile()
{
	m_pcBasicAttackRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CTransform* m_pcTransformRef = (CTransform*)CEntityManager::AddComponentToEntity(m_pcBasicAttackRef, eComponent::TRANSFORM);

	CCapsuleCollider* pcProjCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(m_pcBasicAttackRef, eComponent::CAPSULE_COLLIDER);
	pcProjCollider->SetCenter({ 0,0,0 });
	pcProjCollider->SetRadius(1.0f);
	pcProjCollider->SetHeight(2.1f);
	pcProjCollider->SetTrigger(true);

	CLightComponent* pcProjLight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcBasicAttackRef, eComponent::LIGHT);

	CEntityManager::AddComponentToEntity(m_pcBasicAttackRef, eComponent::RIGIDBODY);

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcBasicAttackRef, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::SPHERE);
	pcMesh->SetIndexBuffer(eIndexBuffer::SPHERE);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::BASIC_ATTACK_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::NONE, 1);
	pcMesh->SetTexture(eSRV::NONE, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::CLAMP);

	CProjectileComponent* pcProjComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcBasicAttackRef, eComponent::PROJECTILE_COMPONENT);
	pcProjComponent->SetDamage(10.0f);
	pcProjComponent->SetLifeTime(1.0f);
	pcProjComponent->SetCasterEntity(eEntity::PLAYER);
	pcProjComponent->SetCost(1.0f);
	pcProjComponent->SetRotationSpeed(20.0f);


	pcProjLight->SetColor({ 229 / 255.0f, 137 / 255.0f, 39 / 255.0f, 1 });
	pcProjLight->SetPosition({ 0, 0, 0 });
	pcProjLight->SetRadius(15.0f);

	CParticleEmitter* pcEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(m_pcBasicAttackRef, eComponent::PARTICLE_EMITTER);

	pcEmitter->SetMaxParticles(20);
	pcEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
	pcEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
	pcEmitter->SetTexture(eSRV::BASIC_ATTACK_PARTICLE_EFFECT, 0);
	pcEmitter->SetTexture(eSRV::NONE, 1);
	pcEmitter->SetTexture(eSRV::NONE, 2);
	pcEmitter->SetInputLayout(eInputLayout::DEFAULT);
	pcEmitter->SetVertexShader(eVertexShader::DEFAULT);
	pcEmitter->SetPixelShader(ePixelShader::PARTICLE);
	pcEmitter->SetSampler(eSampler::WRAP);

	pcEmitter->SetEmitterShape(eEmitter::CONE);
	pcEmitter->SetEmissionAngle(0.0f);
	pcEmitter->SetMaxSpeed(18.0f);
	pcEmitter->SetMinSpeed(15.0f);
	pcEmitter->SetLife(0.5f);
	pcEmitter->SetOffset({ 0, 0, 0 });
	pcEmitter->SetLoop(true);
	pcEmitter->SetMaxRotationSpeed(1);
	pcEmitter->SetMinRotationSpeed(-1);

	pcEmitter->SetParticleStartScale(CMath::TVECTOR3(1.5f, 1.5f, .5f));
	pcEmitter->SetParticleEndScale(CMath::TVECTOR3(0.1f, 0.1f, .5f));

	pcEmitter->SetMaxExtent({ .3f,0, .3f });
	pcEmitter->SetMinExtent({ -.3f,0,-.3f });

	pcEmitter->SetParticlesPerEmission(3);
	pcEmitter->SetEmissionRate(.1f);

	m_pcBasicAttackRef->SetActiveState(false);
}

void CFireDemonController::InitFireball()
{
	m_pcFireballRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CEntityManager::AddComponentToEntity(m_pcFireballRef, eComponent::TRANSFORM);


	CCapsuleCollider* pcProjCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(m_pcFireballRef, eComponent::CAPSULE_COLLIDER);
	pcProjCollider->SetCenter({ 0,0,0 });
	pcProjCollider->SetRadius(0.5f);
	pcProjCollider->SetHeight(1.1f);
	pcProjCollider->SetTrigger(true);

	CLightComponent* pcProjLight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcFireballRef, eComponent::LIGHT);

	CEntityManager::AddComponentToEntity(m_pcFireballRef, eComponent::RIGIDBODY);

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcFireballRef, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::SPHERE);
	pcMesh->SetIndexBuffer(eIndexBuffer::SPHERE);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::FIREBALL_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::NONE, 1);
	pcMesh->SetTexture(eSRV::NONE, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::CLAMP);

	CProjectileComponent* pcProjComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcFireballRef, eComponent::PROJECTILE_COMPONENT);
	pcProjComponent->SetDamage(10.0f);
	pcProjComponent->SetLifeTime(1.0f);
	pcProjComponent->SetCasterEntity(eEntity::PLAYER);
	pcProjComponent->SetCost(1.0f);
	pcProjComponent->SetRotationSpeed(14.0f);

	pcProjLight->SetColor({ 229 / 255.0f, 137 / 255.0f, 39 / 255.0f, 1 });
	pcProjLight->SetPosition({ 0, 0, 0 });
	pcProjLight->SetRadius(15.0f);

	CParticleEmitter* pcEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(m_pcFireballRef, eComponent::PARTICLE_EMITTER);

	pcEmitter->SetMaxParticles(30);
	pcEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
	pcEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
	pcEmitter->SetTexture(eSRV::FIREBALL_PARTICLE_EFFECT, 0);
	pcEmitter->SetTexture(eSRV::NONE, 1);
	pcEmitter->SetTexture(eSRV::NONE, 2);
	pcEmitter->SetInputLayout(eInputLayout::DEFAULT);
	pcEmitter->SetVertexShader(eVertexShader::DEFAULT);
	pcEmitter->SetPixelShader(ePixelShader::PARTICLE);
	pcEmitter->SetSampler(eSampler::WRAP);

	pcEmitter->SetEmitterShape(eEmitter::CONE);
	pcEmitter->SetEmissionAngle(.0f);
	pcEmitter->SetMaxSpeed(-15.0f);
	pcEmitter->SetMinSpeed(-18.0f);
	pcEmitter->SetLife(1.0f);
	pcEmitter->SetOffset({ 0, 0, 0 });
	pcEmitter->SetLoop(true);
	pcEmitter->SetMaxRotationSpeed(2);
	pcEmitter->SetMinRotationSpeed(-2);

	pcEmitter->SetParticleStartScale(CMath::TVECTOR3(4, 4, .5f));
	pcEmitter->SetParticleEndScale(CMath::TVECTOR3(0.5f, 0.5f, .5f));

	pcEmitter->SetMaxExtent({ 0.5f, 0.0, 0.0f });
	pcEmitter->SetMinExtent({ -0.5f, 0.0, 0.0f });

	pcEmitter->SetParticlesPerEmission(3);
	pcEmitter->SetEmissionRate(.1f);

	m_pcFireballRef->SetActiveState(false);
}

void CFireDemonController::InitAOEProj()
{
	m_pcAOERef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CTransform* pcAOETransform = (CTransform*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::TRANSFORM);
	pcAOETransform->SetScale({ 3.0f, 8.0f, 3.0f });
	
	CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::RIGIDBODY);


	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::AOE_MESH);
	pcMesh->SetIndexBuffer(eIndexBuffer::AOE_MESH);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::AOE_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::NONE, 1);
	pcMesh->SetTexture(eSRV::NONE, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::AoE);
	pcMesh->SetSampler(eSampler::WRAP);

	CCapsuleCollider* pcAOECollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::CAPSULE_COLLIDER);
	pcAOECollider->SetRadius(1.8f);
	pcAOECollider->SetHeight(4.0f);
	pcAOECollider->SetTrigger(true);

	CProjectileComponent* pcAOEComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::PROJECTILE_COMPONENT);
	pcAOEComponent->SetDamage(1.0f);
	pcAOEComponent->SetLifeTime(0.48f);
	pcAOEComponent->SetCasterEntity(eEntity::PLAYER);
	pcAOEComponent->SetCost(2.0f);

	CLightComponent* pcAOELight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::LIGHT);
	pcAOELight->SetColor({ 1, 0, 0, 1 });
	pcAOELight->SetPosition({ 0, 4, 0 });
	pcAOELight->SetRadius(25.0f);

	m_pcAOERef->SetActiveState(false);
}

void CFireDemonController::InitFireWall()
{
	m_pcFirewallRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	m_pcFirewallRef->bIsTriggerStay = true;
	CTransform* pcFirewallTransform = (CTransform*)CEntityManager::AddComponentToEntity(m_pcFirewallRef, eComponent::TRANSFORM);
	pcFirewallTransform->SetScale({ 2.5f, 1.0f, 2.0f });

	//CMeshRenderer* pcFirewallMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcFirewallRef, eComponent::MESH_RENDERER);
	//pcFirewallMesh->SetVertexBuffer(eVertexBuffer::FIREWALL);
	//pcFirewallMesh->SetIndexBuffer(eIndexBuffer::FIREWALL);
	//pcFirewallMesh->SetTextureCount(3);
	//pcFirewallMesh->SetTexture(eSRV::FADE, 0);
	//pcFirewallMesh->SetTexture(eSRV::SMOKE_NOISE, 1);
	//pcFirewallMesh->SetTexture(eSRV::FIREWALL_NOISE, 2);
	//pcFirewallMesh->SetInputLayout(eInputLayout::DEFAULT);
	//pcFirewallMesh->SetVertexShader(eVertexShader::DEFAULT);
	//pcFirewallMesh->SetPixelShader(ePixelShader::TORCH_FIRE);
	//pcFirewallMesh->SetSampler(eSampler::WRAP);

	CBoxCollider* pcFirewallCollider = (CBoxCollider*)CEntityManager::AddComponentToEntity(m_pcFirewallRef, eComponent::BOX_COLLIDER);
	pcFirewallCollider->SetCenter({ 0,0,0 });
	pcFirewallCollider->SetExtent({ 3.5f, 1, 0.5f });
	pcFirewallCollider->SetTrigger(true);

	CProjectileComponent* pcFirewallComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcFirewallRef, eComponent::PROJECTILE_COMPONENT);
	pcFirewallComponent->SetDamage(5.0f);
	pcFirewallComponent->SetLifeTime(4.0f);
	pcFirewallComponent->SetCasterEntity(eEntity::PLAYER);
	pcFirewallComponent->SetCost(5.0f);

	CLightComponent* pcFirewallLight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcFirewallRef, eComponent::LIGHT);
	pcFirewallLight->SetColor({ 1, 0, 0, 1 });
	pcFirewallLight->SetPosition({ 0, 4, 0 });
	pcFirewallLight->SetRadius(25.0f);

	CParticleEmitter* pcEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(m_pcFirewallRef, eComponent::PARTICLE_EMITTER);

	pcEmitter->SetMaxParticles(100);
	pcEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
	pcEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
	pcEmitter->SetTexture(eSRV::FIREBALL_PARTICLE_EFFECT, 0);
	pcEmitter->SetTexture(eSRV::NONE, 1);
	pcEmitter->SetTexture(eSRV::NONE, 2);
	pcEmitter->SetInputLayout(eInputLayout::DEFAULT);
	pcEmitter->SetVertexShader(eVertexShader::DEFAULT);
	pcEmitter->SetPixelShader(ePixelShader::PARTICLE);
	pcEmitter->SetSampler(eSampler::WRAP);
	pcEmitter->SetEmitterDirection({0, 1, 0});

	pcEmitter->SetEmitterShape(eEmitter::PLANE);
	pcEmitter->SetEmissionAngle(0.0f);
	pcEmitter->SetMaxSpeed(10.0f);
	pcEmitter->SetMinSpeed(15.0f);
	pcEmitter->SetLife(.8f);
	pcEmitter->SetOffset({ 0, 0, 0 });
	pcEmitter->SetLoop(true);
	pcEmitter->SetMaxRotationSpeed(2);
	pcEmitter->SetMinRotationSpeed(-2);

	pcEmitter->SetParticleStartScale(CMath::TVECTOR3(3, 3, .5f));
	pcEmitter->SetParticleEndScale(CMath::TVECTOR3(0.5f, 0.5f, .5f));

	pcEmitter->SetMaxExtent({ 3.8f, 0, 0.0f });
	pcEmitter->SetMinExtent({ -3.8f, 0, 0.0f });

	pcEmitter->SetParticlesPerEmission(5);
	pcEmitter->SetEmissionRate(.1f);

	m_pcFirewallRef->SetActiveState(false);
}

void CFireDemonController::ResetTimers()
{
	m_fFireBallAttackTime = 0.0f;
	m_fBasicAttacktime = 0.0f;
	m_fAttackAnimTime = 0.0f;
	m_fAoeAttackTime = 0.0f;
	m_fFirewallAttackTime = 0.0f;
	m_fEnemyRadius = 0.0f;
	m_tBuffTimes = { 0,0,0 };
}

void CFireDemonController::ResetMultipliers()
{
	m_fDamageMultiplier = 1.0f;
	m_fSpeedMultiplier = 1.0f;
	m_fResistanceMultiplier = 1.0f;
}

void CFireDemonController::MoveToMouseDirection()
{
	CMath::TVECTOR3 tFloorPosition = GetFloorPosition();

	CMath::TVECTOR3 tNormalVel = CMath::Vector3Normalize(tFloorPosition - m_pcTransform->GetPosition());
	m_pcRigidbody->SetVelocity(tNormalVel * m_pcStats->GetMovementSpeed() * m_fSpeedMultiplier);

	CMath::TVECTOR3 tCurrentPosition = m_pcTransform->GetPosition();
	CMath::TVECTOR3 turnToVector = CMath::TVECTOR3(tFloorPosition.x, tCurrentPosition.y, tFloorPosition.z) - tCurrentPosition;
	m_pcTransform->TurnTo(CMath::Vector3Normalize(turnToVector));

	// Set animation to walking
	/*if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::WALK)
	{
		m_pcAnimator->SetAnimation(ePlayerAnimation::WALK);
		m_pcAnimator->SetAnimationTime(0.0);
		m_pcAnimator->SetSpeed(1.8f);
		m_pcAnimator->SetLoop(true);
	}*/
}

void CFireDemonController::InitIndicator()
{
	m_pcClickIndicator = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CTransform* pcTransform = (CTransform*)CEntityManager::AddComponentToEntity(m_pcClickIndicator, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(m_pcClickIndicator, eComponent::CLICK_INDICATOR);

	pcTransform->SetMatrix(CMath::MatrixXRotation(4.71239f) * CMath::MatrixScale(2.0f, 1, 2.0f));

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcClickIndicator, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::QUAD);
	pcMesh->SetIndexBuffer(eIndexBuffer::QUAD);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eSRV::CLICK_INDICATOR, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::FADE_OUT);
	pcMesh->SetSampler(eSampler::CLAMP);

	m_pcClickIndicator->SetActiveState(false);
}

void CFireDemonController::MoveToMousePosition()
{
	CMath::TVECTOR3 tFloorPosition = GetFloorPosition();

	CMath::TVECTOR3 tNormalVel = CMath::Vector3Normalize(tFloorPosition - m_pcTransform->GetPosition());
	m_pcRigidbody->SetVelocity(tNormalVel * m_pcStats->GetMovementSpeed() * m_fSpeedMultiplier);

	CMath::TVECTOR3 tCurrentPosition = m_pcTransform->GetPosition();
	CMath::TVECTOR3 turnToVector = CMath::TVECTOR3(tFloorPosition.x, tCurrentPosition.y, tFloorPosition.z) - tCurrentPosition;
	m_pcTransform->TurnTo(CMath::Vector3Normalize(turnToVector));

	// Set animation to walking
	if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::WALK)
	{
		m_pcAnimator->SetAnimation(ePlayerAnimation::WALK);
		m_pcAnimator->SetAnimationTime(0.0);
		m_pcAnimator->SetSpeed(1.8f);
		m_pcAnimator->SetLoop(true);
	}
}

float CFireDemonController::GetResistance()
{
	return m_fResistanceMultiplier;
}

CMath::TVECTOR3 CFireDemonController::GetAttackTime()
{
	return CMath::TVECTOR3(m_fFireBallAttackTime, m_fAoeAttackTime, m_fFirewallAttackTime);
}

CMath::TVECTOR3 CFireDemonController::GetBuffTimes()
{
	return m_tBuffTimes;
}

CMath::TVECTOR3 CFireDemonController::GetMaxBuffTime()
{
	return m_tMaxBuffTimes;
}

void CFireDemonController::UseMouseControls(bool bMouseControls)
{
	m_bMouseControls = bMouseControls;
}

CFireDemonController::~CFireDemonController()
{

}

void CFireDemonController::UpdateTimers(float fDelta)
{
	// This can get updated only if it needs to
	if (m_fFireBallAttackTime > 0)
	{
		m_fFireBallAttackTime -= fDelta + ((m_fFireballCooldown * m_fCooldownMultiplier - m_fFireballCooldown) * fDelta);
		if (m_fFireBallAttackTime <= 0)
		{
			m_fFireBallAttackTime = 0;
		}
	}
	// This can get updated only if it needs to
	if (m_fAoeAttackTime > 0)
	{
		m_fAoeAttackTime -= fDelta + ((m_fAOECooldown * m_fCooldownMultiplier - m_fAOECooldown) * fDelta);
		if (m_fAoeAttackTime <= 0)
		{
			m_fAoeAttackTime = 0;
		}
	}
	// This can get updated only if it needs to
	if (m_fBasicAttacktime > 0)
	{
		m_fBasicAttacktime -= fDelta * m_fAttackSpeed;
		if (m_fBasicAttacktime <= 0)
		{
			m_fBasicAttacktime = 0;
		}
	}
	// This can get updated only if it needs to
	if (m_fAttackAnimTime > 0)
	{
		m_fAttackAnimTime -= fDelta * m_fAttackSpeed;
		if (m_fAttackAnimTime <= 0)
		{
			m_fAttackAnimTime = 0;
		}
	}

	if (m_fFirewallAttackTime > 0)
	{
		m_fFirewallAttackTime -= fDelta + ((m_fFirewallCooldown * m_fCooldownMultiplier - m_fFirewallCooldown) * fDelta);
		if (m_fFirewallAttackTime <= 0)
		{
			m_fFirewallAttackTime = 0;
		}
	}
	
	if (m_tBuffTimes.x > 0)
	{
		m_tBuffTimes.x -= fDelta;
		if (m_tBuffTimes.x <= 0)
		{
			m_tBuffTimes.x = 0;
			m_fDamageMultiplier /= 1.5f;
		}
	}

	if (m_tBuffTimes.y > 0)
	{
		m_tBuffTimes.y -= fDelta;
		if (m_tBuffTimes.y <= 0)
		{
			m_tBuffTimes.y = 0;
			m_fSpeedMultiplier /= 1.25f;
		}
	}

	if (m_tBuffTimes.z > 0)
	{
		m_tBuffTimes.z -= fDelta;
		if (m_tBuffTimes.z <= 0)
		{
			m_tBuffTimes.z = 0;
			m_fResistanceMultiplier /= 1.25f;
		}
	}

}

void CFireDemonController::KeyboardMovement()
{
	CMath::TVECTOR3 tVel;
	if (CInputManager::GetKey('W'))
	{
		tVel.z = 0.5f;
		tVel.x = 0.5f;
	}
	else if (CInputManager::GetKey('S'))
	{
		tVel.z = -0.5f;
		tVel.x = -0.5f;
	}
	if (CInputManager::GetKey('D'))
	{
		tVel.x += 0.5f;
		tVel.z += -0.5f;
	}
	else if (CInputManager::GetKey('A'))
	{
		tVel.x += -0.5f;
		tVel.z += 0.5f;
	}

	if (CMath::Vector3Dot(tVel, tVel) > 0)
	{
		CMath::TVECTOR3 tNormalVel = CMath::Vector3Normalize(tVel);
		m_pcRigidbody->SetVelocity(tNormalVel * m_pcStats->GetMovementSpeed() * m_fSpeedMultiplier);
		m_pcTransform->TurnTo(tNormalVel);

		// Send audio message 
		CEventManager::SendAudioMessage(TAudioMessage::TAudioMessage(true, eAudio::SFX, eSFX::PLAYERMOVE));
		m_bMoving = true;

		// Set animation to walking
		if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::WALK)
		{
			m_pcAnimator->SetAnimation(ePlayerAnimation::WALK);
			m_pcAnimator->SetAnimationTime(0.0);
			m_pcAnimator->SetSpeed(1.8f);
			m_pcAnimator->SetLoop(true);
		}
	}
	else
	{
		if (m_bMoving)
		{
			m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0,0,0));
			CEventManager::SendAudioMessage(TAudioMessage::TAudioMessage(false, eAudio::SFX, eSFX::PLAYERMOVE));
			m_bMoving = false;
		}

		if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::IDLE)
		{
			m_pcAnimator->SetAnimation(ePlayerAnimation::IDLE);
			m_pcAnimator->SetAnimationTime(0.0);
			m_pcAnimator->SetSpeed(1.0f);
		}
	}
	// Updating light data
	//((CPlayerEntity*)m_pcOwner)->GetLight()->GetComponent<CTransform>()->SetPosition(m_pcTransform->GetPosition() + CMath::TVECTOR3(0, 2.5f, 0));
}

void CFireDemonController::MouseMovement()
{
	if (CInputManager::GetKeyDown(VK_LBUTTON))
	{
		IEntity* pcEnemy = GetSelectedEnemy();

		if (pcEnemy)
		{
			m_pEnemyTransform = pcEnemy->GetComponent<CTransform>();
			m_fEnemyRadius = pcEnemy->GetComponent<CCapsuleCollider>()->GetRadius() * m_pEnemyTransform->GetScale().x;

			m_pcAgent->SetTarget(m_pEnemyTransform->GetPosition());
			m_pcAgent->Start();

			m_bCheckFloorRay = false;
		}
	}

	if (m_pEnemyTransform)
	{
		CMath::TVECTOR3 tEnemyPosition = m_pEnemyTransform->GetPosition();
		m_pcAgent->SetTarget(tEnemyPosition);

		// Set animation to walking
		if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::WALK)
		{
			m_pcAnimator->SetAnimation(ePlayerAnimation::WALK);
			m_pcAnimator->SetAnimationTime(0.0);
			m_pcAnimator->SetSpeed(1.8f);
			m_pcAnimator->SetLoop(true);
		}

		CMath::TVECTOR3 tCurrentPosition = m_pcTransform->GetPosition();
		CMath::TVECTOR3 turnToVector = CMath::TVECTOR3(tEnemyPosition.x, tCurrentPosition.y, tEnemyPosition.z) - tCurrentPosition;
		m_pcTransform->TurnTo(CMath::Vector3Normalize(turnToVector));
	}

	if (!m_bCheckFloorRay && !CInputManager::GetKey(VK_LBUTTON))
	{
		m_bCheckFloorRay = true;
	}

	if (m_bCheckFloorRay)
	{
		if (CInputManager::GetKey(VK_LBUTTON))
		{
			if (m_pcAgent->IsNavigating())
			{
				m_pcAgent->Stop();
				m_pEnemyTransform = nullptr;
			}

			CMath::TVECTOR3 tFloorPosition = GetFloorPosition();
			CMath::TVECTOR3 tNormalVel = CMath::Vector3Normalize(tFloorPosition - m_pcTransform->GetPosition());
			m_pcRigidbody->SetVelocity(tNormalVel * m_pcStats->GetMovementSpeed() * m_fSpeedMultiplier);

			CMath::TVECTOR3 tCurrentPosition = m_pcTransform->GetPosition();
			CMath::TVECTOR3 turnToVector = CMath::TVECTOR3(tFloorPosition.x, tCurrentPosition.y, tFloorPosition.z) - tCurrentPosition;
			m_pcTransform->TurnTo(CMath::Vector3Normalize(turnToVector));

			// Set animation to walking
			if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::WALK)
			{
				m_pcAnimator->SetAnimation(ePlayerAnimation::WALK);
				m_pcAnimator->SetAnimationTime(0.0);
				m_pcAnimator->SetSpeed(1.8f);
				m_pcAnimator->SetLoop(true);
			}

			if(CInputManager::GetKeyDown(VK_LBUTTON))
			{
				CEnvironmentEntity* pClickInstance = (CEnvironmentEntity*)CEntityManager::CloneEntity(m_pcClickIndicator);
				pClickInstance->GetComponent<CTransform>()->SetPosition(tFloorPosition + CMath::TVECTOR3(0, .1F, 0));

				pClickInstance->SetActiveState(true);
			}

		}
		else if (!m_pcAgent->IsNavigating())
		{
			m_pcRigidbody->SetVelocity(CMath::TVECTOR3(0, 0, 0));
			m_pEnemyTransform = nullptr;

			if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::IDLE)
			{
				m_pcAnimator->SetAnimation(ePlayerAnimation::IDLE);
				m_pcAnimator->SetAnimationTime(0.0);
				m_pcAnimator->SetSpeed(1.0f);
			}

			m_pcClickIndicator->SetActiveState(false);
		}
	}
}

void CFireDemonController::GetMouseRay(CMath::TVECTOR3& tStart, CMath::TVECTOR3& tEnd)
{
	CMath::TVECTOR2 vMouse;
	CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
	CInputManager::GetMouseWorldSpace(vMouse, tStart, tEnd);
}

IEntity* CFireDemonController::GetSelectedEnemy()
{
	CMath::TVECTOR3 tStart, tEnd;
	GetMouseRay(tStart, tEnd);

	CMath::TVECTOR3 tDirection = CMath::Vector3Normalize(tEnd - tStart);
	float tMaxDistance = CMath::Vector3Magnitude(tEnd - tStart);

	CEntityMask mask;
	mask.Add(eEntity::ENEMY);

	TRayHit tHit;
	if (CPhysicsManager::Raycast(tStart, tDirection, tHit, tMaxDistance, &mask))
	{
		if (tHit.tEntity->m_nEntityType == eEntity::ENEMY)
		{
			return tHit.tEntity;
		}
	}

	return nullptr;
}

CMath::TVECTOR3 CFireDemonController::GetFloorPosition()
{
	CMath::TVECTOR3 tStart, tEnd;
	GetMouseRay(tStart, tEnd);

	CMath::TVECTOR3 tFloorPosition;
	CMath::TVECTOR3 tDirection = CMath::Vector3Normalize(tEnd - tStart);

	CPhysicsManager::RaycastMouseToFloor(tStart, tDirection, OUT tFloorPosition);

	return tFloorPosition;
}

void CFireDemonController::BeginBasic(CMath::TVECTOR3 tTarget)
{
	m_pcTransform->LookAt(tTarget);
	//BeginBasicAnimation();

	m_fBasicAttacktime = BASIC_TOTAL_TIME;
	m_bIsAttacking = true;
}

void CFireDemonController::BeginBasicAnimation()
{
	m_pcAnimator->SetAnimation(ePlayerAnimation::MELEE);
	m_pcAnimator->SetAnimationTime(0.0);
	m_pcAnimator->SetSpeed(2.0f * ATTACK_SPEED_MULTIPLIER * m_fAttackSpeed);
	m_pcAnimator->SetLoop(false);
}

void CFireDemonController::InstantiateIndicator(CMath::TVECTOR3 tPosition)
{
	CEnvironmentEntity* pClickInstance = (CEnvironmentEntity*)CEntityManager::CloneEntity(m_pcClickIndicator);
	pClickInstance->GetComponent<CTransform>()->SetPosition(tPosition + CMath::TVECTOR3(0, .1F, 0));

	pClickInstance->SetActiveState(true);
}

void CFireDemonController::Inventory()
{
	if (m_bMouseControls)
	{
		if (CInputManager::GetKeyDown('1'))
		{
			m_pcInventory->SetCurrentConsumable(eConsumableItems::HEALTH);
		}

		if (CInputManager::GetKeyDown('2'))
		{
			m_pcInventory->SetCurrentConsumable(eConsumableItems::MANA);
		}

		if (CInputManager::GetKeyDown('3'))
		{
			m_pcInventory->SetCurrentConsumable(eConsumableItems::DAMAGE);
		}

		if (CInputManager::GetKeyDown('4'))
		{
			m_pcInventory->SetCurrentConsumable(eConsumableItems::HASTE);
		}

		if (CInputManager::GetKeyDown('5'))
		{
			m_pcInventory->SetCurrentConsumable(eConsumableItems::RESIST);
		}
	}
	else
	{
		if (CInputManager::GetKeyDown('Q'))
		{
			m_pcInventory->Previous();
		}

		if (CInputManager::GetKeyDown('E'))
		{
			m_pcInventory->Next();
		}
	}

	if (CInputManager::GetKeyDown('F'))
	{
		if (m_pcInventory->GetCurrentConsumable() != 0)
		{
			CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::POTION));
		}

		switch (m_pcInventory->UseCurrentConsumable())
		{
			case eConsumableItems::HEALTH:
			{
				m_pcEmitter->SetMaxParticles(5);
				m_pcEmitter->SetTexture(eSRV::HEALTH_PARTICLE, 0);

				m_pcEmitter->SetEmitterShape(eEmitter::CONE);
				m_pcEmitter->SetEmissionAngle(0.0f);
				m_pcEmitter->SetMaxSpeed(7.0f);
				m_pcEmitter->SetMinSpeed(4.0f);
				m_pcEmitter->SetLife(1.0f);
				m_pcEmitter->SetOffset({ 0, 3, 0 });

				m_pcEmitter->SetLoop(false);
				m_pcEmitter->SetMaxRotationSpeed(2);
				m_pcEmitter->SetMinRotationSpeed(-2);
				m_pcEmitter->SetEmitterDirection({ 0,1,0 });

				m_pcEmitter->SetParticleStartScale(CMath::TVECTOR3(.5f, .5f, .5f));
				m_pcEmitter->SetParticleEndScale(CMath::TVECTOR3(1, 1, .5f));

				m_pcEmitter->SetMaxExtent({ 1.2f, 0, 1.2f });
				m_pcEmitter->SetMinExtent({ -1.2f, 0, -1.2f });

				m_pcEmitter->SetParticlesPerEmission(1);
				m_pcEmitter->SetEmissionRate(.5f);

				m_pcEmitter->RefreshEmitterSettings();

				m_pcStats->SetHP(m_pcStats->GetHP() + m_pcStats->GetMaxHP() * 0.25f);
				break;
			}
			case eConsumableItems::MANA:
			{
				//Non looping particles
				m_pcEmitter->SetMaxParticles(5);
				m_pcEmitter->SetTexture(eSRV::MANA_PARTICLE, 0);

				m_pcEmitter->SetEmitterShape(eEmitter::CONE);
				m_pcEmitter->SetEmissionAngle(0.0f);
				m_pcEmitter->SetMaxSpeed(7.0f);
				m_pcEmitter->SetMinSpeed(4.0f);
				m_pcEmitter->SetLife(1.0f);
				m_pcEmitter->SetOffset({ 0, 3, 0 });

				m_pcEmitter->SetLoop(false);
				m_pcEmitter->SetMaxRotationSpeed(2);
				m_pcEmitter->SetMinRotationSpeed(-2);
				m_pcEmitter->SetEmitterDirection({ 0,1,0 });

				m_pcEmitter->SetParticleStartScale(CMath::TVECTOR3(.5f, .5f, .5f));
				m_pcEmitter->SetParticleEndScale(CMath::TVECTOR3(1, 1, .5f));

				m_pcEmitter->SetMaxExtent({ 1.2f, 0, 1.2f });
				m_pcEmitter->SetMinExtent({ -1.2f, 0, -1.2f });

				m_pcEmitter->SetParticlesPerEmission(1);
				m_pcEmitter->SetEmissionRate(.5f);

				m_pcEmitter->RefreshEmitterSettings();

				m_pcStats->SetMP(m_pcStats->GetMaxMP());
				break;
			}
			case eConsumableItems::DAMAGE:
			{
				//looping particles
				m_pcEmitter->SetMaxParticles(5);
				m_pcEmitter->SetTexture(eSRV::DAMAGE_PARTICLE, 0);

				m_pcEmitter->SetEmitterShape(eEmitter::CONE);
				m_pcEmitter->SetEmissionAngle(0.0f);
				m_pcEmitter->SetMaxSpeed(7.0f);
				m_pcEmitter->SetMinSpeed(4.0f);
				m_pcEmitter->SetLife(1.0f);
				m_pcEmitter->SetOffset({ 0, 3, 0 });

				m_pcEmitter->SetLoop(false);
				m_pcEmitter->SetMaxRotationSpeed(2);
				m_pcEmitter->SetMinRotationSpeed(-2);
				m_pcEmitter->SetEmitterDirection({ 0,1,0 });

				m_pcEmitter->SetParticleStartScale(CMath::TVECTOR3(.5f, .5f, .5f));
				m_pcEmitter->SetParticleEndScale(CMath::TVECTOR3(1, 1, .5f));

				m_pcEmitter->SetMaxExtent({ 1.2f, 0, 1.2f });
				m_pcEmitter->SetMinExtent({ -1.2f, 0, -1.2f });

				m_pcEmitter->SetParticlesPerEmission(1);
				m_pcEmitter->SetEmissionRate(.5f);

				m_pcEmitter->RefreshEmitterSettings();

				if (m_tBuffTimes.x <= 0.0f)
				{
					m_fDamageMultiplier *= 1.5f;
				}
				m_tBuffTimes.x = m_tMaxBuffTimes.x;
				break;
			}
			case eConsumableItems::HASTE:
			{
				//looping particles
				m_pcEmitter->SetMaxParticles(5);
				m_pcEmitter->SetTexture(eSRV::PARTICLE_SPARK, 0);

				m_pcEmitter->SetEmitterShape(eEmitter::CONE);
				m_pcEmitter->SetEmissionAngle(0.0f);
				m_pcEmitter->SetMaxSpeed(7.0f);
				m_pcEmitter->SetMinSpeed(4.0f);
				m_pcEmitter->SetLife(1.0f);
				m_pcEmitter->SetOffset({ 0, 3, 0 });

				m_pcEmitter->SetLoop(false);
				m_pcEmitter->SetMaxRotationSpeed(2);
				m_pcEmitter->SetMinRotationSpeed(-2);
				m_pcEmitter->SetEmitterDirection({ 0,1,0 });

				m_pcEmitter->SetParticleStartScale(CMath::TVECTOR3(.5f, .5f, .5f));
				m_pcEmitter->SetParticleEndScale(CMath::TVECTOR3(1, 1, .5f));

				m_pcEmitter->SetMaxExtent({ 1.2f, 0, 1.2f });
				m_pcEmitter->SetMinExtent({ -1.2f, 0, -1.2f });

				m_pcEmitter->SetParticlesPerEmission(1);
				m_pcEmitter->SetEmissionRate(.5f);

				m_pcEmitter->RefreshEmitterSettings();

				if (m_tBuffTimes.y <= 0.0f)
				{
					m_fSpeedMultiplier *= 1.25f;
				}
				m_tBuffTimes.y = m_tMaxBuffTimes.y;
				break;
			}
			case eConsumableItems::RESIST:
			{
				//looping particles
				m_pcEmitter->SetMaxParticles(5);
				m_pcEmitter->SetTexture(eSRV::DEFENSE_PARTICLE, 0);

				m_pcEmitter->SetEmitterShape(eEmitter::CONE);
				m_pcEmitter->SetEmissionAngle(0.0f);
				m_pcEmitter->SetMaxSpeed(7.0f);
				m_pcEmitter->SetMinSpeed(4.0f);
				m_pcEmitter->SetLife(1.0f);
				m_pcEmitter->SetOffset({ 0, 3, 0 });

				m_pcEmitter->SetLoop(false);
				m_pcEmitter->SetMaxRotationSpeed(2);
				m_pcEmitter->SetMinRotationSpeed(-2);
				m_pcEmitter->SetEmitterDirection({ 0,1,0 });

				m_pcEmitter->SetParticleStartScale(CMath::TVECTOR3(.5f, .5f, .5f));
				m_pcEmitter->SetParticleEndScale(CMath::TVECTOR3(1, 1, .5f));

				m_pcEmitter->SetMaxExtent({ 1.2f, 0, 1.2f });
				m_pcEmitter->SetMinExtent({ -1.2f, 0, -1.2f });

				m_pcEmitter->SetParticlesPerEmission(1);
				m_pcEmitter->SetEmissionRate(.5f);

				m_pcEmitter->RefreshEmitterSettings();

				if (m_tBuffTimes.z <= 0.0f)
				{
					m_fResistanceMultiplier *= 1.25f;
				}
				m_tBuffTimes.z = m_tMaxBuffTimes.z;
				break;
			}
			default:
			{
				//CEventManager::SendDebugMessage(TDebugMessage("There is nothing to use"));
				break;
			}
		}
	}

	// Use this to change multiplier values as needed
	if (m_pcInventory->ChangePassive())
	{
		int last = -1;
		for (int i = 0; i < 2; i++)
		{
			if (m_pcInventory->GetLastPassive().mData[i] == 1)
			{
				last = i;
				break;
			}
		}
		switch ((int)m_pcInventory->GetLastPassive().mData[last])
		{
		case ePassiveItems::RING:
		{
			m_fDamageMultiplier /= 1.15f;
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			m_fResistanceMultiplier /= 1.5f;
			break;
		}
		case ePassiveItems::BELT:
		{
			// Health-on-Kill Deactivated
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			m_fDamageMultiplier /= 1.4f;
			m_fResistanceMultiplier *= 1.2f;
			break;
		}
		case ePassiveItems::BOOTS:
		{
			m_fSpeedMultiplier /= 1.25f;
			break;
		}
		case ePassiveItems::HASTE:
		{
			// Attack Speed
			m_fAttackSpeed /= 1.2f;
			break;
		}
		case ePassiveItems::HAT:
		{
			// Ability Cooldowns
			m_fCooldownMultiplier /= 1.25f;
			break;
		}
		default:
			break;
		}

		switch ((int)m_pcInventory->GetPassive().mData[last])
		{
		case ePassiveItems::RING:
		{
			m_fDamageMultiplier *= 1.15f;
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			m_fResistanceMultiplier *= 1.5f;
			break;
		}
		case ePassiveItems::BELT:
		{
			// Health-on-Kill Activated
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			m_fDamageMultiplier *= 1.4f;
			m_fResistanceMultiplier /= 1.2f;
			break;
		}
		case ePassiveItems::BOOTS:
		{
			m_fSpeedMultiplier *= 1.25f;
			break;
		}
		case ePassiveItems::HASTE:
		{
			// Attack Speed
			m_fAttackSpeed *= 1.2f;
			break;
		}
		case ePassiveItems::HAT:
		{
			// Ability Cooldowns
			m_fCooldownMultiplier *= 1.25f;
			break;
		}
		default:
			break;
		}
	}

	// Use this to change cooldown times as needed
	if (m_pcInventory->ChangeActive())
	{
		switch (m_pcInventory->GetLastActive())
		{
		case 0:
		{

		}
		default:
			break;
		}

		switch (m_pcInventory->GetActive())
		{
		case 0:
		{

		}
		default:
			break;
		}
	}

}

float CFireDemonController::GetFireballCoolDown()
{
	return m_fFireballCooldown;
}
float CFireDemonController::GetAOECooldown()
{
	return m_fAOECooldown;
}
float CFireDemonController::GetFirewallCooldown()
{
	return m_fFirewallCooldown;
}
