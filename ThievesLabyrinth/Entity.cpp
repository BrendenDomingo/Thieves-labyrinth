#include "Entity.h"
#include "MageController.h"
#include "ChickenController.h"
#include "FireDemonController.h"

#include "EnumTypes.h"

#include "Component.h"
#include "Level.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Stats.h"
#include "SpikeTrapController.h"
#include "Transform.h"
#include "GameLogic.h"
#include "PathSurface.h"
#include "LightComponent.h"
#include "ParticleEffect.h"
#include "PathAgent.h"
#include "Time.h"
#include "Rigidbody.h"
#include "AnimatorComponent.h"
#include "Math.h"

#include <random>
#include <time.h>


class IComponent;
class ICollider;
class CRigidbody;
class CMeshRenderer;
class CFireDemonController;
class CCameraController;
class CAnimatorComponent;

#pragma region Base Clase

IEntity::IEntity(int nId)
{
	m_nEntityId = nId;
	m_bState = true;
	m_pcRoom = nullptr;
}

bool IEntity::IsActive()
{
	return m_bState;
}

void IEntity::SetActiveState(bool bState)
{
	m_bState = bState;

	for(IComponent* pcComponent : m_pcComponents)
	{
		pcComponent->SetActiveState(m_bState);
	}
}

void IEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
}

void IEntity::OnTriggerStay(IEntity * pcCollidingEntity)
{
}

void IEntity::OnTriggerExit(IEntity * pcCollidingEntity)
{
}

void IEntity::OnCollisionEnter(IEntity * pcCollidingEntity)
{
}

void IEntity::OnCollisionStay(IEntity * pcCollidingEntity)
{
}

void IEntity::OnCollisionExit(IEntity * pcCollidingEntity)
{
}

IEntity::~IEntity()
{
}

#pragma endregion

#pragma region Player Entity

CPlayerEntity::CPlayerEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::PLAYER;
}

void CPlayerEntity::Update()
{
	//Run controller update;
}

void CPlayerEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
	if (pcCollidingEntity == nullptr)
		return;

	if (pcCollidingEntity->m_nEntityType == eEntity::PROJECTILE && pcCollidingEntity->m_nEntityId != eEntity::PLAYER)
	{
		CProjectileComponent* pcProjectileComponent = pcCollidingEntity->GetComponent<CProjectileComponent>();
		if (pcProjectileComponent->GetCasterEntity() != eEntity::PLAYER)
		{
			CStats* pcStats = GetComponent<CStats>();

			pcStats->SetHP(pcStats->GetHP() - pcCollidingEntity->GetComponent<CProjectileComponent>()->GetDamage() / GetComponent<CFireDemonController>()->GetResistance());

			/*CParticleEmitter* pParticleEffect = GetComponent<CParticleEmitter>();

			pParticleEffect->SetMaxParticles(8);
			pParticleEffect->SetVertexBuffer(eVertexBuffer::QUAD);
			pParticleEffect->SetIndexBuffer(eIndexBuffer::QUAD);
			pParticleEffect->SetTexture(eSRV::TORCH_PARTICLE_EFFECT, 0);
			pParticleEffect->SetTexture(eSRV::NONE, 1);
			pParticleEffect->SetTexture(eSRV::NONE, 2);
			pParticleEffect->SetInputLayout(eInputLayout::DEFAULT);
			pParticleEffect->SetVertexShader(eVertexShader::DEFAULT);
			pParticleEffect->SetPixelShader(ePixelShader::PASS_2D);
			pParticleEffect->SetSampler(eSampler::WRAP);

			pParticleEffect->SetEmitterShape(eEmitter::CONE);

			CMath::TVECTOR3 vDirection = pcCollidingEntity->GetComponent<CRigidbody>()->GetVelocity();
			pParticleEffect->SetEmitterDirection(-CMath::Vector3Normalize(vDirection));
			pParticleEffect->SetEmissionAngle(20.0f);
			pParticleEffect->SetMaxSpeed(6.0f);
			pParticleEffect->SetMinSpeed(4.0f);
			pParticleEffect->SetLife(.5f);
			pParticleEffect->SetOffset({ 0, 4, 0 });
			pParticleEffect->SetLoop(false);
			pParticleEffect->SetMaxRotationSpeed(5);
			pParticleEffect->SetMinRotationSpeed(-5);

			pParticleEffect->SetParticleStartScale(CMath::TVECTOR3(1.2f, 1.2f, .5f));
			pParticleEffect->SetParticleEndScale(CMath::TVECTOR3(0.5f, 0.5f, .5f));
			pParticleEffect->SetParticlesPerEmission(5);
			pParticleEffect->SetEmissionRate(0.0f);

			pParticleEffect->RefreshEmitterSettings();*/

			GetComponent<CFireDemonController>()->ResetFlashTimer();
			CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, rand() % (eSFX::HIT4 - eSFX::HIT + 1) + eSFX::HIT));

			if (pcStats->GetHP() <= 0)
			{
				CGameLogic::CheckLose();
			}
		}
	}
	else if (pcCollidingEntity->m_nEntityType == eEntity::TRAP)
	{
		CSpikeTrapController* pcTrapController = pcCollidingEntity->GetComponent<CSpikeTrapController>();

		if (pcTrapController)
		{
			CStats* pcStats = GetComponent<CStats>();

			pcStats->SetHP((float)pcStats->GetHP() - ((CSpikeTrapController*)pcTrapController)->GetDamage() * GetComponent<CFireDemonController>()->GetResistance());

			CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, rand() % (eSFX::HIT4 - eSFX::HIT + 1) + eSFX::HIT));

			if (pcStats->GetHP() <= 0)
			{
				CGameLogic::CheckLose();
			}
		}
	}
	else if (pcCollidingEntity->m_nEntityType == eEntity::ITEM)
	{
		// Figure out what item drop it is
		CItemEntity* pcItem = (CItemEntity*)pcCollidingEntity;
		switch (pcItem->m_nItemClass)
		{
		case eItemClass::CONSUMABLE:
		{
			GetComponent<CInventory>()->AddConsumableItem(pcItem->m_nItemType);
			CLevel::m_pcCurrentRoom->RemoveLight(pcItem->GetComponent<CLightComponent>());
			CEntityManager::AddEntityToDeletionQueue(pcCollidingEntity);
			// If we haven't picked up this specific item before, add it to the collection count
			if (!pcItem->m_bAcquired)
			{
				CGameLogic::m_nItemsCollected++;
				pcItem->m_bAcquired = true;
			}
			break;
		}
		case eItemClass::ACTIVE:
		{
			GetComponent<CInventory>()->SetActive(pcItem->m_nItemType);
			int lastItem = GetComponent<CInventory>()->GetLastActive();
			if (lastItem == eActiveItems::NONE)
			{
				CEntityManager::AddEntityToDeletionQueue(pcCollidingEntity);
			}
			else
			{
				pcItem->m_nItemType = lastItem;
				switch (lastItem)
				{
				case 0:
				{

				}
				default:
					pcItem->GetComponent<CLightComponent>()->SetColor(CMath::TVECTOR4(1, 1, 1, 1));
					break;
				}
			}
			break;
		}
		case eItemClass::PASSIVE:
		{
			CInventory* pStorage = GetComponent<CInventory>();
			int nSlot = -1;
			if (pStorage->GetPassive().x == ePassiveItems::NONE)
			{
				nSlot = 0;
			}
			else if (pStorage->GetPassive().y == ePassiveItems::NONE)
			{
				nSlot = 1;
			}
			if (nSlot >= 0 && nSlot < 2)
			{
				pStorage->SetPassive(pcItem->m_nItemType, nSlot);
				CEntityManager::AddEntityToDeletionQueue(pcCollidingEntity);
				// If we haven't picked up this specific item before, add it to the collection count
				if (!pcItem->m_bAcquired)
				{
					CGameLogic::m_nItemsCollected++;
					pcItem->m_bAcquired = true;
				}
			}
			else
			{
				this->GetComponent<CFireDemonController>()->OverItem(true);
			}
			break;
		}
		default:
			break;
		}
	}
}

void CPlayerEntity::OnTriggerExit(IEntity * pcCollidingEntity)
{
	if (pcCollidingEntity == nullptr)
	{
		return;
	}

	if (pcCollidingEntity->m_nEntityType == eEntity::DOOR)
	{
		CMath::TVECTOR3 PlayerPosition = GetComponent<CTransform>()->GetPosition();
		CMath::TVECTOR3 ColliderPosition = pcCollidingEntity->GetComponent<CTransform>()->GetPosition();

		CLightComponent* pcLight = GetComponent<CLightComponent>();

		if(pcLight)
		{
			for (size_t i = 0; i < m_pcRoom->m_pcNeighbors.size(); i++)
			{
				m_pcRoom->m_pcNeighbors[i]->RemoveLight(pcLight);
			}
			m_pcRoom->RemoveLight(pcLight);
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[0]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Right Door (Camera Right) 
			if (PlayerPosition.x > ColliderPosition.x)
			{
				int NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size() - 1;
				for (int i = NeighborsSize; i >= 0; i--)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Top == true)
					{
						NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size();
						for (int j = 0; j < NeighborsSize; j++)
						{
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();
						}
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();					
						
						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);	
						m_pcRoom = CLevel::m_pcCurrentRoom;
						AddPlayerLight(pcLight);

						return;
					}
				}
			}

			if (PlayerPosition.x < ColliderPosition.x)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);

				m_pcRoom = CLevel::m_pcCurrentRoom;
			
				AddPlayerLight(pcLight);

				return;
			}
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[3]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Bottom Door (Camera Bottom)
			if (PlayerPosition.z < ColliderPosition.z)
			{
				int NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size();
				for (int i = 0; i < NeighborsSize; i++)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Right == true)
					{
						NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size();
						for (int j = 0; j < NeighborsSize; j++)
						{
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();
						}

						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();						

						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);						
						m_pcRoom = CLevel::m_pcCurrentRoom;
					
						AddPlayerLight(pcLight);

						return;
					}
				}
			}

			if (PlayerPosition.z > ColliderPosition.z)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);

				m_pcRoom = CLevel::m_pcCurrentRoom;			
				AddPlayerLight(pcLight);
				return;
			}
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[2]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Top Door (Camera Top)
			if (PlayerPosition.z > ColliderPosition.z)
			{
				int NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size() - 1;
				for (int i = NeighborsSize; i >= 0; i--)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Left == true)
					{
						NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size();
						for (int j = 0; j < NeighborsSize; j++)
						{
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();
						}

						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();						

						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);						

						this->m_pcRoom = CLevel::m_pcCurrentRoom;
					
						AddPlayerLight(pcLight);

						return;
					}
				}
			}

			if (PlayerPosition.z < ColliderPosition.z)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);

				m_pcRoom = CLevel::m_pcCurrentRoom;
				
				AddPlayerLight(pcLight);
				return;
			}
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[1]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Left Door (Camera Left)
			if (PlayerPosition.x < ColliderPosition.x)
			{
				int NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size();
				for (int i = 0; i < NeighborsSize; i++)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Bottom == true)
					{
						NeighborsSize = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size();
						for (int j = 0; j < NeighborsSize; j++)
						{
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();
						}
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();		

						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);
						m_pcRoom = CLevel::m_pcCurrentRoom;
						return;
					}
				}
			}

			if (PlayerPosition.x > ColliderPosition.x)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);
				m_pcRoom = CLevel::m_pcCurrentRoom;

				AddPlayerLight(pcLight);
				return;
			}
		}
	}

	else if (pcCollidingEntity->m_nEntityType == eEntity::ITEM)
	{
		CItemEntity* pcItem = (CItemEntity*)pcCollidingEntity;
		if (pcItem->m_nItemClass == eItemClass::PASSIVE)
		{
			this->GetComponent<CFireDemonController>()->OverItem(false);
		}
	}
}

void CPlayerEntity::OnTriggerStay(IEntity* pcCollidingEntity)
{
	if (pcCollidingEntity->m_nEntityType == eEntity::ITEM)
	{
		CItemEntity* pcItem = (CItemEntity*)pcCollidingEntity;
		int nSlot = -1;
		if (CInputManager::GetKeyDown('Z'))
		{
			nSlot = 0;
		}
		else if (CInputManager::GetKeyDown('X'))
		{
			nSlot = 1;
		}
		if (nSlot > -1 && nSlot < 2)
		{
			CInventory* pcStorage = GetComponent<CInventory>();
			if (pcItem->m_nItemClass == eItemClass::PASSIVE)
			{
				pcStorage->SetPassive(pcItem->m_nItemType, nSlot);
				int lastItem = (int)pcStorage->GetLastPassive().mData[nSlot];
				pcItem->m_nItemType = lastItem;
				switch (lastItem)
				{
				case ePassiveItems::RING:
				{
					pcItem->GetComponent<CLightComponent>()->SetColor({ 1, 0, 0, 1 });
					break;
				}
				case ePassiveItems::THIMBLE:
				{
					pcItem->GetComponent<CLightComponent>()->SetColor({ 1, 1, 0, 1 });
					break;
				}
				case ePassiveItems::BELT:
				{
					pcItem->GetComponent<CLightComponent>()->SetColor({ 255 / 255.0f, 138 / 255.0f, 38 / 255.0f, 1 });
					break;
				}
				case ePassiveItems::GAUNTLET:
				{
					pcItem->GetComponent<CLightComponent>()->SetColor({ 0, 1, 1, 1 });
					break;
				}
				case ePassiveItems::BOOTS:
				{
					pcItem->GetComponent<CLightComponent>()->SetColor({ 0, 1, 0, 1 });
					break;
				}
				case ePassiveItems::HASTE:
				{
					pcItem->GetComponent<CLightComponent>()->SetColor({ 1, 0, 1, 1 });
					break;
				}
				case ePassiveItems::HAT:
				{
					pcItem->GetComponent<CLightComponent>()->SetColor({ 0, 0, 1, 1 });
					break;
				}
				default:
					pcItem->GetComponent<CLightComponent>()->SetColor({ 1, 1, 1, 1 });
					break;
				}
			}
		}
	}
}

CPlayerEntity::~CPlayerEntity()
{

}

void CPlayerEntity::AddPlayerLight(CLightComponent * pcLight)
{
	if (pcLight)
	{
		for (size_t i = 0; i < m_pcRoom->m_pcNeighbors.size(); i++)
		{
			m_pcRoom->m_pcNeighbors[i]->AddLight(pcLight);
		}
		m_pcRoom->AddLight(pcLight);
	}
}

#pragma endregion

#pragma region Enemy Entity



void CEnemyEntity::DeathAnimation()
{
	CAnimatorComponent* pcAnimator = GetComponent<CAnimatorComponent>();
	if (pcAnimator)
	{
		pcAnimator->SetAnimation(eEnemyAnimation::DEATH);
		pcAnimator->SetLoop(false);
		pcAnimator->SetAnimationTime(0.0);
		pcAnimator->SetSpeed(1.0f);
		pcAnimator->SetLoop(false);
	}
}

CEnemyEntity::CEnemyEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::ENEMY;
}

void CEnemyEntity::Update()
{
	//Run controller update;
}

void CEnemyEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
	if (!pcCollidingEntity)
		return;

	if (pcCollidingEntity->m_nEntityType == eEntity::PROJECTILE)
	{
		CProjectileComponent* pcProjectileComponent = pcCollidingEntity->GetComponent<CProjectileComponent>();
		CProjectileEntity* pcProj = (CProjectileEntity*)pcCollidingEntity;

		if (pcProjectileComponent->GetCasterEntity() != eEntity::ENEMY && !pcProj->bIsTriggerStay)
		{
			CKnightController* controller = GetComponent<CKnightController>();
			if(controller)
			{
				if(controller->IsBlocking())
				{
					CEventManager::SendAudioMessage({ true, eAudio::SFX, eSFX::BLOCK });

					if(controller->IsDeflecting())
					{
						CRigidbody* pcRigidbody = pcProj->GetComponent<CRigidbody>();

						if(pcRigidbody)
						{
							pcProjectileComponent->SetLifeTime(3.0F);
							pcProjectileComponent->SetCasterEntity(eEntity::ENEMY);
							pcRigidbody->SetVelocity(-pcRigidbody->GetVelocity());

							CParticleEmitter* cEmitter = pcProj->GetComponent<CParticleEmitter>();

							if(cEmitter)
							{
								cEmitter->SetTexture(eSRV::PARTICLE_SPARK, 0);
								cEmitter->RefreshEmitterSettings();

								pcProj->GetComponent<CMeshRenderer>()->SetTexture(eSRV::ENEMY_KNIGHT_DIFFUSE, 0);
							}
						}
					}

					return;
				}
			}

			CStats* pcStats = GetComponent<CStats>();
			float health = pcStats->GetHP();
			if(health > 0)
			{
				health -= pcCollidingEntity->GetComponent<CProjectileComponent>()->GetDamage();
				pcStats->SetHP(health);
				CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, rand() % ( eSFX::HIT4 - eSFX::HIT + 1 ) + eSFX::HIT));

				GetComponent<IEnemyController>()->ResetFlashTimer();

				if(health <= 0)
				{
					DeathAnimation();

					m_pcRoom->m_nEnemyCount--;
					CLevel::UpdateDoors(this->m_pcRoom);
					CLevel::nTotalEnemyCount--;
					CGameLogic::CheckWin();

					if (pcCollidingEntity->GetComponent<CProjectileComponent>()->GetCasterEntity() == eEntity::PLAYER)
					{
						CGameLogic::m_nSlain++;
						if (CEntityManager::GetEntity(0)->GetComponent<CInventory>()->GetPassive().x == ePassiveItems::BELT ||
							CEntityManager::GetEntity(0)->GetComponent<CInventory>()->GetPassive().y == ePassiveItems::BELT)
						{
							CEntityManager::GetEntity(0)->GetComponent<CStats>()->SetHP(CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetHP() +
							CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetMaxHP() * 0.05f);
						}
						if (rand() % 100 < 75)
						{
							// ITEM DROP!!
							CItemEntity* pcDrop = (CItemEntity*)CEntityManager::CreateEntity(eEntity::ITEM);
							pcDrop->CreateRandomItem((IEntity*)this);
							
						}
					}
				}
			}
		}
	}
	else if (pcCollidingEntity->m_nEntityType == eEntity::TRAP)
	{
		void* pcTrapController = pcCollidingEntity->GetComponent<CSpikeTrapController>();

		if (pcTrapController)
		{
			CStats* pcStats = GetComponent<CStats>();
			float health = pcStats->GetHP();
			if (health > 0)
			{
				pcStats->SetHP(health - ((CSpikeTrapController*)pcTrapController)->GetDamage());
				CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, rand() % (eSFX::HIT4 - eSFX::HIT + 1) + eSFX::HIT));

				GetComponent<IEnemyController>()->ResetFlashTimer();
				if (pcStats->GetHP() <= 0)
				{
					DeathAnimation();
					m_pcRoom->m_nEnemyCount--;
					CLevel::UpdateDoors(this->m_pcRoom);
					CLevel::nTotalEnemyCount--;
					CGameLogic::CheckWin();
				}
			}
		}
	}
}

void CEnemyEntity::OnTriggerStay(IEntity * pcCollidingEntity)
{
	if (pcCollidingEntity->m_nEntityType == eEntity::PROJECTILE)
	{
		CProjectileComponent* pcProjectileComponent = pcCollidingEntity->GetComponent<CProjectileComponent>();
		CProjectileEntity* pcProj = (CProjectileEntity*)pcCollidingEntity;
		if (pcProjectileComponent->GetCasterEntity() != eEntity::ENEMY && pcProj->bIsTriggerStay)
		{
			CStats* pcStats = GetComponent<CStats>();
			float health = pcStats->GetHP();
			if (health > 0)
			{
				GetComponent<IEnemyController>()->ResetFlashTimer();

				health -= pcCollidingEntity->GetComponent<CProjectileComponent>()->GetDamage() * CTime::GetDeltaF();
				pcStats->SetHP(health);
			//	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, rand() % (eSFX::HIT4 - eSFX::HIT + 1) + eSFX::HIT));

				if (health <= 0)
				{
					DeathAnimation();

					m_pcRoom->m_nEnemyCount--;
					CLevel::UpdateDoors(this->m_pcRoom);
					CLevel::nTotalEnemyCount--;
					CGameLogic::CheckWin();
					if (pcProjectileComponent->GetCasterEntity() == eEntity::PLAYER)
					{
						CGameLogic::m_nSlain++;
						if (CEntityManager::GetEntity(0)->GetComponent<CInventory>()->GetPassive().x == ePassiveItems::BELT ||
							CEntityManager::GetEntity(0)->GetComponent<CInventory>()->GetPassive().y == ePassiveItems::BELT)
						{
							CEntityManager::GetEntity(0)->GetComponent<CStats>()->SetHP(CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetHP() +
								CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetMaxHP() * 0.05f);
						}
					}
				}
			}
		}
	}

}

CEnemyEntity::~CEnemyEntity()
{

}

#pragma endregion

#pragma region Projectile Entity

CProjectileEntity::CProjectileEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::PROJECTILE;
	bIsTriggerStay = false;
}

void CProjectileEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
	if(GetComponent<CProjectileComponent>()->GetCasterEntity() != pcCollidingEntity->m_nEntityType && pcCollidingEntity->m_nEntityType != eEntity::PROJECTILE 
		&& pcCollidingEntity->m_nEntityType != eEntity::ITEM && !bIsTriggerStay)
	{
		CParticleEmitter* pcEmitter = GetComponent<CParticleEmitter>();
		CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, rand() % (eSFX::STONE_IMPACT_5 - eSFX::STONE_IMPACT_1 + 1) + eSFX::STONE_IMPACT_1));

		if(pcEmitter)
		{
			 CKnightController* pcController = pcCollidingEntity->GetComponent<CKnightController>();

			if(pcController)
			{
				if(pcController->IsBlocking())
				{
					GetComponent<CProjectileComponent>()->Destroy(eSRV::PARTICLE_SPARK);
					return;
				}
			}

			GetComponent<CProjectileComponent>()->Destroy(eSRV::NONE);
		}
		else if(GetComponent<CRigidbody>())
		{
			CEntityManager::AddEntityToDeletionQueue(this);
		}
	}
}

CProjectileEntity::~CProjectileEntity()
{

}

#pragma endregion

#pragma region Camera Entity

CCameraEntity::CCameraEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::CAMERA;
}

void CCameraEntity::Update()
{
	//Run controller update;
}

CCameraEntity::~CCameraEntity()
{
}

#pragma endregion

#pragma region Room Entity

#define MAX_LIGHTS 16

CRoomEntity::CRoomEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::ROOM;
	Top = false;
	Bottom = false;
	Right = false;
	Left = false;
}

void CRoomEntity::SpawnEntities()
{
	if (Entities == true)
	{
		for (size_t i = 0; i < m_pcEnemies.size(); i++)
		{
			m_pcEnemies[i]->SetActiveState(false);
			m_pcEnemies[i]->GetComponent<CMeshRenderer>()->SetActiveState(true);
			if(m_pcEnemies[i]->GetComponent<CAnimatorComponent>())
				m_pcEnemies[i]->GetComponent<CAnimatorComponent>()->SetActiveState(true);
		}
		return;
	}

	SetActiveState(true);

	CTransform* pRoomTransform = GetComponent<CTransform>();

	int Wallsize = (int)m_pcWalls.size();
	for (int i = 0; i < Wallsize; i++)
	{
		m_pcWalls[i]->m_pcRoom = this;
		m_pcWalls[i]->GetComponent<CTransform>()->SetPosition(m_pcWalls[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcWalls[i]->SetActiveState(true);		
	}

	int TrapSize = (int)m_pcTraps.size();
	for (int i = 0; i < TrapSize; i++)
	{
		m_pcTraps[i]->m_pcRoom = this;
		m_pcTraps[i]->GetComponent<CTransform>()->SetPosition(m_pcTraps[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcTraps[i]->SetActiveState(true);
	}

	int PillarSize = (int)m_pcPillars.size();
	for (int i = 0; i < PillarSize; i++)
	{
		m_pcPillars[i]->m_pcRoom = this;
		m_pcPillars[i]->GetComponent<CTransform>()->SetPosition(m_pcPillars[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcPillars[i]->SetActiveState(true);
	}

	int LightsSize = (int)m_pcLights.size();
	for (int i = 0; i <LightsSize; i++)
	{
		m_pcLights[i]->m_pcRoom = this;
		m_pcLights[i]->GetComponent<CTransform>()->SetPosition(m_pcLights[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcLights[i]->SetActiveState(true);
	}
	
	int ClutterSize = (int)m_pcClutter.size();
	for (int i = 0; i < ClutterSize; i++)
	{
		m_pcClutter[i]->m_pcRoom = this;
		m_pcClutter[i]->GetComponent<CTransform>()->SetPosition(m_pcClutter[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcClutter[i]->SetActiveState(true);
	}

	CPathSurface* pcPathSurface = GetComponent<CPathSurface>();
	pcPathSurface->SetOrigin(pRoomTransform->GetPosition());

	int SpawnSize = (int)m_pcSpawns.size();
	for (int i = 0; i < SpawnSize; i++)
	{
		m_pcSpawns[i]->m_pcRoom = this;
		m_pcSpawns[i]->GetComponent<CTransform>()->SetPosition(m_pcSpawns[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcSpawns[i]->SetActiveState(true);
	}

	int EnemieSize = (int)m_pcEnemies.size();
	for (int i = 0; i < EnemieSize; i++)
	{
		m_pcEnemies[i]->m_pcRoom = this;

		if (this != CLevel::m_pcCurrentRoom)
		{
			m_pcEnemies[i]->SetActiveState(false);
			m_pcEnemies[i]->GetComponent<CMeshRenderer>()->SetActiveState(true);
		}

		if (this != CLevel::m_pcStartRoom)
		{
			m_pcEnemies[i]->SetActiveState(true);
		}

		if (this == CLevel::m_pcStartRoom)
		{
			m_pcEnemies[i]->GetComponent<IEnemyController>()->GetPlayerReference();
			return;
		}

		m_pcEnemies[i]->GetComponent<CTransform>()->SetPosition(m_pcEnemies[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcEnemies[i]->SetActiveState(false);
		m_pcEnemies[i]->GetComponent<CMeshRenderer>()->SetActiveState(true);

		CPathAgent* pcAgent = (CPathAgent*)m_pcEnemies[i]->GetComponent<CPathAgent>();
		if(pcAgent) pcAgent->SetSurface(this->GetComponent<CPathSurface>());

		m_pcEnemies[i]->GetComponent<IEnemyController>()->GetPlayerReference();
	}

	Entities = true;
	SetActiveState(true);
}

void CRoomEntity::DeSpawnEntities()
{
	if (Entities == false)
	{
		return;
	}

	CTransform* pRoomTransform = GetComponent<CTransform>();

	for (int i = 0; i < m_nEnemyCount; i++)
	{
		m_pcEnemies[i]->GetComponent<CTransform>()->SetPosition(m_pcEnemies[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcEnemies[i]->SetActiveState(false);
	}

	int TrapsSize = (int)m_pcTraps.size();
	for (int i = 0; i < TrapsSize; i++)
	{
		m_pcTraps[i]->m_pcRoom = this;
		m_pcTraps[i]->GetComponent<CTransform>()->SetPosition(m_pcTraps[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcTraps[i]->SetActiveState(false);
	}

	int Wallsize = (int)m_pcWalls.size();
	for (int i = 0; i < Wallsize; i++)
	{
		m_pcWalls[i]->GetComponent<CTransform>()->SetPosition(m_pcWalls[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcWalls[i]->SetActiveState(false);
	}

	int PillarsSize = (int)m_pcPillars.size();
	for (int i = 0; i < PillarsSize; i++)
	{
		m_pcPillars[i]->GetComponent<CTransform>()->SetPosition(m_pcPillars[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcPillars[i]->SetActiveState(false);
	}

	int LightsSize = (int)m_pcLights.size();
	for (int i = 0; i < LightsSize; i++)
	{
		m_pcLights[i]->m_pcRoom = this;
		m_pcLights[i]->GetComponent<CTransform>()->SetPosition(m_pcLights[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcLights[i]->SetActiveState(false);
	}

	int SpawnsSize = (int)m_pcSpawns.size();
	for (int i = 0; i <SpawnsSize; i++)
	{
		m_pcSpawns[i]->GetComponent<CTransform>()->SetPosition(m_pcSpawns[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcSpawns[i]->SetActiveState(false);
	}

	int ClutterSize = (int)m_pcClutter.size();
	for (int i = 0; i < ClutterSize; i++)
	{
		m_pcClutter[i]->m_pcRoom = this;
		m_pcClutter[i]->GetComponent<CTransform>()->SetPosition(m_pcClutter[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());

		m_pcClutter[i]->SetActiveState(false);
	}

	Entities = false;
	SetActiveState(false);
}

void CRoomEntity::SpawnDoors()
{
	if (Doors == true)
	{
		return;
	}

	CTransform* pRoomTransform = GetComponent<CTransform>();

	int DoorSize = (int)m_pcDoors.size();
	for (int i = 0; i < DoorSize; i++)
	{
		m_pcDoors[i]->m_pcRoom = this;
		m_pcDoors[i]->GetComponent<CTransform>()->SetPosition(m_pcDoors[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		
		m_pcDoors[i]->SetActiveState(true);
		
	}

	Doors = true;
}

void CRoomEntity::DeSpawnDoors()
{
	if (Doors == false)
	{
		return;
	}

	CTransform* pRoomTransform = GetComponent<CTransform>();

	int DoorSize = (int)m_pcDoors.size();
	for (int i = 0; i < DoorSize; i++)
	{
		
		m_pcDoors[i]->GetComponent<CTransform>()->SetPosition(m_pcDoors[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcDoors[i]->GetComponent<CBoxCollider>()->SetTrigger(false);
		m_pcDoors[i]->SetActiveState(false);
	}

	Doors = false;
}

void CRoomEntity::TurnEnemiesOn()
{
	for (int i = 0; i < m_nEnemyCount; i++)
	{
		m_pcEnemies[i]->SetActiveState(true);
	}
}

void CRoomEntity::AddLight(CLightComponent * pNewLight)
{
	if(m_pLights.size() < MAX_LIGHTS)
	{
		m_pLights.push_back(pNewLight);
	}
}

void CRoomEntity::RemoveLight(CLightComponent * pLight)
{
	int LightsSize = (int)m_pLights.size();
	for (int i = 0; i < LightsSize; i++)
	{
		if (m_pLights[i] == pLight)
		{
			m_pLights.erase(m_pLights.begin() + i);
			break;
		}
	}
}

void CRoomEntity::AddItem(CItemEntity * pItem)
{
	m_pcItems.push_back(pItem);
}

void CRoomEntity::RemoveItem(CItemEntity * pItem)
{
	for (int i = 0; i < m_pcItems.size(); i++)
	{
		if (m_pcItems[i] == pItem)
		{
			m_pcItems.erase(m_pcItems.begin() + i);
			break;
		}
	}
}

std::vector<CLightComponent*> CRoomEntity::GetLights()
{
	return m_pLights;
}

std::vector<CItemEntity*> CRoomEntity::GetItems()
{
	return m_pcItems;
}

CRoomEntity::~CRoomEntity()
{
	
}

#pragma endregion

#pragma region Environment Entity

CEnvironmentEntity::CEnvironmentEntity(int nID) : IEntity(nID)
{
	m_nEntityType = eEntity::ENVIRONMENT;
}

CEnvironmentEntity::~CEnvironmentEntity()
{

}

#pragma endregion

#pragma region DoorEntity

CDoorEntity::CDoorEntity(int nID) : IEntity(nID)
{
	m_nEntityType = eEntity::DOOR;
}

CDoorEntity::~CDoorEntity()
{
}

#pragma endregion

#pragma region Trap Entity

CTrapEntity::CTrapEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::TRAP;
}

CTrapEntity::~CTrapEntity()
{

}

#pragma endregion

#pragma region Spawn Entity

CSpawnEntity::CSpawnEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::SPAWN;
}

CSpawnEntity::~CSpawnEntity()
{

}

#pragma endregion

#pragma region Item Entity

CItemEntity::CItemEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::ITEM;
	m_bAcquired = false;
}

CItemEntity::~CItemEntity()
{

}

void CItemEntity::CreateRandomItem(IEntity * pEntity)
{
	int nItemClass = eItemClass::CONSUMABLE;
	if (rand() % 20 < 5)
	{
		nItemClass = eItemClass::PASSIVE;
	}
	int nItemType = 0;
	switch (nItemClass)
	{
	case eItemClass::CONSUMABLE:
	{
		nItemType = (rand() % (eConsumableItems::COUNT - eConsumableItems::HEALTH)) + eConsumableItems::HEALTH;
		break;
	}
	case eItemClass::PASSIVE:
	{
		nItemType = (rand() % (ePassiveItems::COIN - ePassiveItems::RING)) + ePassiveItems::RING;
		break;
	}
	default:
		break;
	}
	CreateItem(nItemClass, nItemType, pEntity);
}

void CItemEntity::CreateItem(int nItemClass, int nItemType, IEntity * pEntity)
{
	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(this, eComponent::MESH_RENDERER);
	pcMesh->SetTextureCount(3);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::CLAMP);
	pcMesh->SetGlowColor({ 1.0f, 0.9f, 0.0f, 1.0f });

	CTransform* pcTransform = (CTransform*)CEntityManager::AddComponentToEntity(this, eComponent::TRANSFORM);
	pcTransform->SetMatrix(CMath::MatrixYRotation(TORAD(-135)));
	CCapsuleCollider* pcCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(this, eComponent::CAPSULE_COLLIDER);

	CMath::TVECTOR3 position = pEntity->GetComponent<CTransform>()->GetPosition();
	position.y = 0;
	pcTransform->SetPosition(position);

	pcCollider->SetTrigger(true);
	pcCollider->SetCenter(CMath::TVECTOR3(0, 2.0f, 0));
	pcCollider->SetRadius(2.0f);
	pcCollider->SetHeight(4.01f);
	this->m_pcRoom = pEntity->m_pcRoom;

	CLightComponent* pcLight = (CLightComponent*)CEntityManager::AddComponentToEntity(this, eComponent::LIGHT);
	this->m_nItemClass = nItemClass;
	switch (nItemClass)
	{
	case eItemClass::CONSUMABLE:
	{
		pcLight->SetPosition({ 0, 1, 0 });
		pcMesh->SetTexture(eSRV::POTION, 0);
		pcMesh->SetTexture(eSRV::NONE, 1);
		pcMesh->SetTexture(eSRV::NONE, 2);
		pcMesh->SetVertexBuffer(eVertexBuffer::POTION);
		pcMesh->SetIndexBuffer(eIndexBuffer::POTION);
		this->m_nItemType = nItemType;
		switch (nItemType)
		{
		case eConsumableItems::HEALTH:
		{
			pcLight->SetColor({ 1, 0, 0, 1 });
			break;
		}
		case eConsumableItems::MANA:
		{
			pcLight->SetColor({ 0, 0, 1, 1 });
			break;
		}
		case eConsumableItems::DAMAGE:
		{
			pcLight->SetColor({ 1, 0.5f, 0, 1 });
			break;
		}
		case eConsumableItems::HASTE:
		{
			pcLight->SetColor({ 1, 0, 1, 1 });
			break;
		}
		case eConsumableItems::RESIST:
		{
			pcLight->SetColor({ 0, 1, 0, 1 });
			break;
		}
		default:
			pcLight->SetColor({ 1, 1, 1, 1 });
			break;
		}
		break;
	}
	case eItemClass::PASSIVE:
	{
		pcLight->SetPosition({ 0, 2.5f, 0 });

		pcMesh->SetTexture(eSRV::LOOT_CHEST_DIFFUSE, 0);
		pcMesh->SetTexture(eSRV::LOOT_CHEST_NORMAL, 1);
		pcMesh->SetTexture(eSRV::LOOT_CHEST_SPECULAR, 2);
		pcMesh->SetVertexBuffer(eVertexBuffer::LOOTCHEST);
		pcMesh->SetIndexBuffer(eIndexBuffer::LOOTCHEST);
		this->m_nItemType = nItemType;
		switch (nItemType)
		{
		case ePassiveItems::RING:
		{
			pcLight->SetColor({ 1, 0, 0, 1 });
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			pcLight->SetColor({ 1, 1, 0, 1 });
			break;
		}
		case ePassiveItems::BELT:
		{
			pcLight->SetColor({ 139 / 255.0f, 69 / 255.0f, 69 / 255.0f, 1 });
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			pcLight->SetColor({ 0, 1, 1, 1 });
			break;
		}
		case ePassiveItems::BOOTS:
		{
			pcLight->SetColor({ 0, 1, 0, 1 });
			break;
		}
		case ePassiveItems::HASTE:
		{
			pcLight->SetColor({ 1, 0, 1, 1 });
			break;
		}
		case ePassiveItems::HAT:
		{
			pcLight->SetColor({ 0, 0, 1, 1 });
			break;
		}
		default:
			pcLight->SetColor({ 1, 1, 1, 1 });
			break;
		}
		break;
	}
	default:
		break;
	}
	pcLight->SetRadius(8.0f);

	this->SetActiveState(true);
	this->m_pcRoom->AddItem(this);

	CParticleEmitter* pcEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(this, eComponent::PARTICLE_EMITTER);

	pcEmitter->SetMaxParticles(15);
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
	pcEmitter->SetMaxSpeed(7.0f);
	pcEmitter->SetMinSpeed(4.0f);
	pcEmitter->SetLife(1.0f);
	pcEmitter->SetOffset({ 0, 0, 0 });
	pcEmitter->SetLoop(true);
	pcEmitter->SetMaxRotationSpeed(2);
	pcEmitter->SetMinRotationSpeed(-2);
	pcEmitter->SetEmitterDirection({ 0,1,0 });

	pcEmitter->SetParticleStartScale(CMath::TVECTOR3(.5f, .5f, .5f));
	pcEmitter->SetParticleEndScale(CMath::TVECTOR3(1, 1, .5f));

	pcEmitter->SetMaxExtent({ .7f, 0, .7f });
	pcEmitter->SetMinExtent({ -.7f, 0, -.7f });

	pcEmitter->SetParticlesPerEmission(1);
	pcEmitter->SetEmissionRate(.5f);

	pcEmitter->RefreshEmitterSettings();
}

#pragma endregion

CParticleEntity::CParticleEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::PARTICLE;
}
