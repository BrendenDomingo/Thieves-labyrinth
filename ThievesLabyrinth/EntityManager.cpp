#include "EntityManager.h"
#include "Entity.h"

#include "MageController.h"
#include "KnightController.h"

#include "Rigidbody.h"
#include "AnimatorComponent.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Transform.h"
#include "PathSurface.h"
#include "ParticleEffect.h"
#include "LightComponent.h"
#include "IndicatorController.h"
#include "SpikeTrapController.h"
#include "Entity.h"

#include "EnumTypes.h"
#include "Stats.h"

CComponentManager*				CEntityManager::m_pcComponentManager;

CPlayerEntity*					CEntityManager::m_pcPlayer;

std::vector<CEnemyEntity*>		 CEntityManager::m_pcEnemies;
std::vector<CProjectileEntity*>	 CEntityManager::m_pcProjectiles;
CCameraEntity*					 CEntityManager::m_pcCamera;
std::vector<CRoomEntity*>		 CEntityManager::m_pcRooms;
std::vector<CEnvironmentEntity*> CEntityManager::m_pcEnvironmentEntities;
std::vector<CDoorEntity*>		 CEntityManager::m_pcDoorEntities;
std::vector<CTrapEntity*>		 CEntityManager::m_pcTrapEntities;
std::vector<CSpawnEntity*>		 CEntityManager::m_pcSpawnEntities;
std::vector<CItemEntity*>		 CEntityManager::m_pcItemEntities;
std::vector<IEntity*>			 CEntityManager::m_pcDeletionQueue;
std::vector<CParticleEntity*>	 CEntityManager::m_pcParticleEntities;

int								 CEntityManager::m_nIdCount;


CEntityManager::CEntityManager(CComponentManager* pcComponentManager)
{
	m_nIdCount = 1;
	m_pcComponentManager = pcComponentManager;
}

IEntity* CEntityManager::CreateEntity(int nType, int nOption)
{
	//USE ALLOCATOR
	IEntity* pcResult;

	switch(nType)
	{
		case eEntity::PLAYER:
		{
			m_pcPlayer = new CPlayerEntity(0);
			pcResult = m_pcPlayer;

			break;
		}

		case eEntity::ENVIRONMENT:
		{
			pcResult = new CEnvironmentEntity(m_nIdCount++);
			m_pcEnvironmentEntities.push_back((CEnvironmentEntity*)pcResult);
			break;
		}

		case eEntity::ENEMY:
		{
			pcResult = new CEnemyEntity(m_nIdCount++);
			m_pcEnemies.push_back((CEnemyEntity*)pcResult);
		/*	switch(nOption)
			{
				case eEnemyType::KNIGHT:
				{
					break;
				}
				case eEnemyType::BERSERKER:
				{
					break;
				}
				case eEnemyType::MAGE:
				{
					break;
				}
				case eEnemyType::ARCHER:
				{
					break;
				}
			
				case eEnemyType::CHICKEN:
				{
					break;
				}

			default:
				break;
			}*/
			break;

		}

		case eEntity::PROJECTILE:
		{	
			pcResult = new CProjectileEntity(m_nIdCount++);
			m_pcProjectiles.push_back((CProjectileEntity*)pcResult);
			break;
		}

		case eEntity::CAMERA:
		{
			m_pcCamera = new CCameraEntity(m_nIdCount++);
			pcResult = m_pcCamera;

			break;
		}

		case eEntity::ROOM:
		{
			pcResult = new CRoomEntity(m_nIdCount++);
			m_pcRooms.push_back((CRoomEntity*)pcResult);

			break;
		}

		case eEntity::DOOR:
		{
			pcResult = new CDoorEntity(m_nIdCount++);
			m_pcDoorEntities.push_back((CDoorEntity*)pcResult);

			break;
		}

		case eEntity::TRAP:
		{
			pcResult = new CTrapEntity(m_nIdCount++);
			m_pcTrapEntities.push_back((CTrapEntity*)pcResult);

			break;
		}

		case eEntity::SPAWN:
		{
			pcResult = new CSpawnEntity(m_nIdCount++);
			m_pcSpawnEntities.push_back((CSpawnEntity*)pcResult);

			break;
		}

		case eEntity::ITEM:
		{
			pcResult = new CItemEntity(m_nIdCount++);
			m_pcItemEntities.push_back((CItemEntity*)pcResult);

			break;
		}

		case eEntity::PARTICLE:
		{
			pcResult = new CParticleEntity(m_nIdCount++);
			m_pcParticleEntities.push_back((CParticleEntity*)pcResult);

			break;
		}

		default:
		{
			pcResult = nullptr;
			break;
		}
	}

	return pcResult;
}

IComponent* CEntityManager::AddComponentToEntity(IEntity* pcEntity, int nComponentType)
{
	IComponent* pcNewComponent = m_pcComponentManager->CreateComponent(pcEntity, nComponentType);
	if(pcNewComponent != nullptr)
	{
		pcEntity->m_pcComponents.push_back(pcNewComponent);
	}

	return pcNewComponent;
}

int CEntityManager::RemoveComponentFromEntity(IEntity * pcEntity, int nComponentType)
{
	for(unsigned int i = 0; i < pcEntity->m_pcComponents.size(); i++)
	{
		if(pcEntity->m_pcComponents[i]->m_nComponentType == nComponentType)
		{
			m_pcComponentManager->DeleteComponent(pcEntity->m_pcComponents[i]->m_nComponentType, pcEntity->m_pcComponents[i]);

			pcEntity->m_pcComponents.erase(pcEntity->m_pcComponents.begin() + i);

			
			return 0;
		}
	}

	return - 1;
}

void CEntityManager::AddEntityToDeletionQueue(IEntity * pcEntity)
{
	
	auto entityFound = std::find(m_pcDeletionQueue.begin(), m_pcDeletionQueue.end(), pcEntity);
	if (entityFound == std::end(m_pcDeletionQueue))
	{
		m_pcDeletionQueue.push_back(pcEntity);
	}
}

int CEntityManager::DeleteEntity(IEntity* pcEntity)
{
	for(IComponent* component : pcEntity->m_pcComponents)
	{
		m_pcComponentManager->DeleteComponent(component->m_nComponentType, component);
	}
	pcEntity->m_pcComponents.clear();

	switch(pcEntity->m_nEntityType)
	{
		case eEntity::PLAYER:
		{
			delete m_pcPlayer;
			m_pcPlayer = nullptr;

			break;
		}

		case eEntity::ENEMY:
		{
			int EnemiesSize = (int)m_pcEnemies.size();
			for(int i = 0; i < EnemiesSize; i++)
			{
				if(m_pcEnemies[i] == pcEntity)
				{
					m_pcEnemies.erase(m_pcEnemies.begin() + i);
					break;
				}
			}

			delete (CEnemyEntity*)pcEntity;
			break;
		}

		case eEntity::PROJECTILE:
		{
			int ProjectilesSize = (int)m_pcProjectiles.size();
			for(int i = 0; i < ProjectilesSize; i++)
			{
				if(m_pcProjectiles[i] == pcEntity)
				{
					m_pcProjectiles.erase(m_pcProjectiles.begin() + i);
					break;
				}
			}

			delete (CProjectileEntity*)pcEntity;
			break;
		}

		case eEntity::CAMERA:
		{
			
			delete m_pcCamera;

			m_pcCamera = nullptr;
			break;
		}

		case eEntity::ROOM:
		{
			int RoomSize = (int)m_pcRooms.size();
			for (int i = 0; i < RoomSize; i++)
			{
				if (m_pcRooms[i] == pcEntity)
				{
					m_pcRooms.erase(m_pcRooms.begin() + i);
					break;
				}
			}

			delete (CRoomEntity*)pcEntity;
			break;
		}

		case eEntity::ENVIRONMENT:
		{
			int EnvironmentSize = (int)m_pcEnvironmentEntities.size();
			for (int i = 0; i < EnvironmentSize; i++)
			{
				if (m_pcEnvironmentEntities[i] == pcEntity)
				{
					m_pcEnvironmentEntities.erase(m_pcEnvironmentEntities.begin() + i);
					break;
				}
			}
		
			delete (CEnvironmentEntity*)pcEntity;
			break;
		}

		case eEntity::DOOR:
		{
			int DoorSize = (int)m_pcDoorEntities.size();
			for (int i = 0; i < DoorSize; i++)
			{
				if (m_pcDoorEntities[i] == pcEntity)
				{
					m_pcDoorEntities.erase(m_pcDoorEntities.begin() + i);

					break;
				}
			}

			delete (CDoorEntity*)pcEntity;
			break;
		}

		case eEntity::TRAP:
		{
			int TrapSize = (int)m_pcTrapEntities.size();
			for (int i = 0; i < TrapSize; i++)
			{
				if (m_pcTrapEntities[i] == pcEntity)
				{
					m_pcTrapEntities.erase(m_pcTrapEntities.begin() + i);
					break;
				}
			}

			delete (CTrapEntity*)pcEntity;
			break;
		}

		case eEntity::SPAWN:
		{
			int SpawnSize = (int)m_pcSpawnEntities.size();
			for (int i = 0; i < SpawnSize; i++)
			{
				if (m_pcSpawnEntities[i] == pcEntity)
				{
					m_pcSpawnEntities.erase(m_pcSpawnEntities.begin() + i);
					break;
				}
			}

			delete (CSpawnEntity*)pcEntity;
			break;
		}

		case eEntity::ITEM:
		{
			int ItemsSize = (int)m_pcItemEntities.size();
			for (int i = 0; i < ItemsSize; i++)
			{
				if (m_pcItemEntities[i] == pcEntity)
				{
					m_pcItemEntities.erase(m_pcItemEntities.begin() + i);
					break;
				}
			}

			pcEntity->m_pcRoom->RemoveItem((CItemEntity*)pcEntity);

			delete (CItemEntity*)pcEntity;
			break;
		}

		case eEntity::PARTICLE:
		{
			int ParticleSize = (int)m_pcParticleEntities.size();
			for (int i = 0; i < ParticleSize; i++)
			{
				if (m_pcParticleEntities[i] == pcEntity)
				{
					m_pcParticleEntities.erase(m_pcParticleEntities.begin() + i);
					break;
				}
			}

			delete (CParticleEntity*)pcEntity;
			break;
		}

		default:
		{
			return -1;

			break;
		}
	}
	pcEntity = nullptr;

	return 0;
}

int CEntityManager::GetEntityCount(int nType)
{
	switch(nType)
	{
		case eEntity::PLAYER:
		{
			return m_pcPlayer != nullptr ? 1 : 0;

			break;
		}

		case eEntity::ENEMY:
		{
			return (int)m_pcEnemies.size();
			break;
		}

		case eEntity::PROJECTILE:
		{
			return (int)m_pcProjectiles.size();
			break;
		}

		case eEntity::CAMERA:
		{
			return m_pcCamera != nullptr ? 1 : 0;
			break;
		}

		case eEntity::ROOM:
		{
			return (int)m_pcRooms.size();
			break;
		}

		case eEntity::DOOR:
		{
			return (int)m_pcDoorEntities.size();
			break;
		}

		case eEntity::TRAP:
		{
			return (int)m_pcTrapEntities.size();
			break;
		}

		case eEntity::SPAWN:
		{
			return (int)m_pcSpawnEntities.size();
			break;
		}

		case eEntity::ITEM:
		{
			return (int)m_pcItemEntities.size();
			break;
		}

		case eEntity::PARTICLE:
		{
			return (int)m_pcParticleEntities.size();
			break;
		}

		default:
		{
			return -1;
			break;
		}

	}
}

IEntity* CEntityManager::GetEntity(int nID)
{
	if(m_pcPlayer->m_nEntityId == nID)
	{
		return m_pcPlayer;
	}

	if(m_pcCamera->m_nEntityId == nID)
	{
		return m_pcCamera;
	}

	int EnemiesSize = (int)m_pcEnemies.size();
	for(int i = 0; i < EnemiesSize; i++)
	{
		if(m_pcEnemies[i]->m_nEntityId == nID)
		{
			return m_pcEnemies[i];
		}
	}

	int ProjectileSize = (int)m_pcProjectiles.size();
	for(int i = 0; i < ProjectileSize; i++)
	{
		if(m_pcProjectiles[i]->m_nEntityId == nID)
		{
			return m_pcProjectiles[i];
		}

	}

	int RoomsSize = (int)m_pcRooms.size();
	for(int i = 0; i < RoomsSize; i++)
	{
		if(m_pcRooms[i]->m_nEntityId == nID)
		{
			return m_pcRooms[i];
		}
	}

	int DoorSize = (int)m_pcDoorEntities.size();
	for (int i = 0; i < DoorSize; i++)
	{
		if (m_pcDoorEntities[i]->m_nEntityId == nID)
		{
			return m_pcDoorEntities[i];
		}
	}

	int EnvironmentSize = (int)m_pcEnvironmentEntities.size();
	for (int i = 0; i < EnvironmentSize; i++)
	{
		if (m_pcEnvironmentEntities[i]->m_nEntityId == nID)
		{
			return m_pcEnvironmentEntities[i];
		}
	}

	int TrapSize = (int)m_pcTrapEntities.size();
	for (int i = 0; i < TrapSize; i++)
	{
		if (m_pcTrapEntities[i]->m_nEntityId == nID)
		{
			return m_pcTrapEntities[i];
		}
	}

	int SpawnSize = (int)m_pcSpawnEntities.size();
	for (int i = 0; i < SpawnSize; i++)
	{
		if (m_pcSpawnEntities[i]->m_nEntityId == nID)
		{
			return m_pcSpawnEntities[i];
		}
	}

	int ItemsSize = (int)m_pcItemEntities.size();
	for (int i = 0; i < ItemsSize; i++)
	{
		if (m_pcItemEntities[i]->m_nEntityId == nID)
		{
			return m_pcItemEntities[i];
		}
	}

	int ParticleSize = (int)m_pcParticleEntities.size();
	for (int i = 0; i < ParticleSize; i++)
	{
		if (m_pcParticleEntities[i]->m_nEntityId == nID)
		{
			return m_pcParticleEntities[i];
		}
	}


	return nullptr;
}

void CEntityManager::DeleteAllEntities()
{
	if (m_pcPlayer != nullptr)
	{
		DeleteEntity(m_pcPlayer);
	}

	if (m_pcCamera != nullptr)
	{
		DeleteEntity(m_pcCamera);
	}

	int EnemiesSize = (int)m_pcEnemies.size() - 1;
	for (int i = EnemiesSize; i >= 0; i--)
	{
		DeleteEntity(m_pcEnemies[i]);
	}

	int ProjectilesSize = (int)m_pcProjectiles.size() - 1;
	for (int i = ProjectilesSize; i >= 0; i--)
	{
		DeleteEntity(m_pcProjectiles[i]);
	}

	int EnvironmentSize = (int)m_pcEnvironmentEntities.size() - 1;
	for (int i = EnvironmentSize; i >= 0; i--)
	{
		DeleteEntity(m_pcEnvironmentEntities[i]);
	}

	int ItemSize = (int)m_pcItemEntities.size() - 1;
	for (int i = ItemSize; i >= 0; i--)
	{
		DeleteEntity(m_pcItemEntities[i]);
	}

	int RoomSize = (int)m_pcRooms.size() - 1;
	for (int i = RoomSize; i >= 0; i--)
	{
		DeleteEntity(m_pcRooms[i]);
	}

	int DoorSize = (int)m_pcDoorEntities.size() - 1;
	for (int i = DoorSize; i >= 0; i--)
	{
		DeleteEntity(m_pcDoorEntities[i]);
	}

	int TrapSize = (int)m_pcTrapEntities.size() - 1;
	for (int i = TrapSize; i >= 0; i--)
	{
		DeleteEntity(m_pcTrapEntities[i]);
	}

	int SpawnSize = (int)m_pcSpawnEntities.size() - 1;
	for (int i = SpawnSize; i >= 0; i--)
	{
		DeleteEntity(m_pcSpawnEntities[i]);
	}

	int ParticleSize = (int)m_pcParticleEntities.size() - 1;
	for (int i = ParticleSize; i >= 0; i--)
	{
		DeleteEntity(m_pcParticleEntities[i]);
	}

	m_nIdCount = 1;
	m_pcDeletionQueue.clear();
}

IEntity* CEntityManager::CloneEntity(IEntity * pcEntityToCopy)
{
	IEntity* pNewEntity = CEntityManager::CreateEntity(pcEntityToCopy->m_nEntityType);
	
	for(IComponent* pcComponent : pcEntityToCopy->m_pcComponents)
	{
		IComponent* pcNewComponent = CEntityManager::AddComponentToEntity(pNewEntity, pcComponent->m_nComponentType);
		
		switch(pcComponent->m_nComponentType)
		{
			case eComponent::TRANSFORM:
			{
				CTransform* temp = (CTransform*)pcNewComponent;
				*temp = *(CTransform*)pcComponent;
				break;
			}

			case eComponent::BOX_COLLIDER:
			{
				CBoxCollider* temp = (CBoxCollider*)pcNewComponent;
				*temp = *(CBoxCollider*)pcComponent;

				break;
			}

			case eComponent::CAPSULE_COLLIDER:
			{
				CCapsuleCollider* temp = (CCapsuleCollider*)pcNewComponent;
				*temp = *(CCapsuleCollider*)pcComponent;

				break;
			}

			case eComponent::RIGIDBODY:
			{
				CRigidbody* temp = (CRigidbody*)pcNewComponent;
				*temp = *(CRigidbody*)pcComponent;

				break;
			}

			case eComponent::MESH_RENDERER:
			{
				CMeshRenderer* temp = (CMeshRenderer*)pcNewComponent;
				*temp = *(CMeshRenderer*)pcComponent;

				break;
			}

			case eComponent::STATS:
			{
				CStats* temp = (CStats*)pcNewComponent;
				*temp = *(CStats*)pcComponent;

				break;
			}

			case eComponent::ANIMATOR:
			{
				CAnimatorComponent* temp = (CAnimatorComponent*)pcNewComponent;
				*temp = *(CAnimatorComponent*)pcComponent;

				break;
			}

			case eComponent::MAGE_CONTROLLER:
			{
				CMageController* temp = (CMageController*)pcNewComponent;
				*temp = *(CMageController*)pcComponent;

				break;
			}

			case eComponent::CHICKEN_CONTROLLER:
			{
				CChickenController* temp = (CChickenController*)pcNewComponent;
				*temp = *(CChickenController*)pcComponent;

				break;
			}

			case eComponent::VIKING_CONTROLLER:
			{
				CVikingController* temp = (CVikingController*)pcNewComponent;
				*temp = *(CVikingController*)pcComponent;

				break;
			}

			case eComponent::KNIGHT_CONTROLLER:
			{
				CKnightController* temp = (CKnightController*)pcNewComponent;
				*temp = *(CKnightController*)pcComponent;

				break;
			}

			case eComponent::PROJECTILE_COMPONENT:
			{
				CProjectileComponent* temp = (CProjectileComponent*)pcNewComponent;
				*temp = *(CProjectileComponent*)pcComponent;

				break;
			}

			case eComponent::LIGHT:
			{
				CLightComponent* temp = (CLightComponent*)pcNewComponent;
				*temp = *(CLightComponent*)pcComponent;

				break;
			}

			case eComponent::PATH_SURFACE:
			{
				CPathSurface* temp = (CPathSurface*)pcNewComponent;
				*temp = *(CPathSurface*)pcComponent;

				break;
			}

			
			case eComponent::PARTICLE_EMITTER:
			{
				CParticleEmitter* temp = (CParticleEmitter*)pcNewComponent;
				*temp = *(CParticleEmitter*)pcComponent;
				temp->Disable();
				break;
			}

			case eComponent::CLICK_INDICATOR:
			{
				CIndicatorController* temp = (CIndicatorController*)pcNewComponent;
				*temp = *(CIndicatorController*)pcComponent;

				break;
			}

			case eComponent::SPIKE_TRAP_COMPONENT:
			{
				CSpikeTrapController* temp = (CSpikeTrapController*)pcNewComponent;
				*temp = *(CSpikeTrapController*)pcComponent;

				break;
			}
		}
	}

	if (pNewEntity->m_nEntityType == eEntity::PARTICLE)
	{
		CParticleEntity* pcNewParticle = (CParticleEntity*)pNewEntity;
		CParticleEntity* pcParticleToCopy = (CParticleEntity*)pcEntityToCopy;
		pcNewParticle->m_fLifetime = pcParticleToCopy->m_fLifetime;

		return pcNewParticle;
	}

	if (pNewEntity->m_nEntityType == eEntity::ROOM)
	{
		CRoomEntity* pcNewRoom = (CRoomEntity*)pNewEntity;
		CRoomEntity* pcRoomToCopy = (CRoomEntity*)pcEntityToCopy;
		
		pcNewRoom->m_nEnemyCount = pcRoomToCopy->m_nEnemyCount;

		for (CEnemyEntity* pcEnemyEntity : pcRoomToCopy->m_pcEnemies)
		{
			pcNewRoom->m_pcEnemies.push_back((CEnemyEntity*)CloneEntity(pcEnemyEntity));
		}

		for (CEnvironmentEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcWalls)
		{
			pcNewRoom->m_pcWalls.push_back((CEnvironmentEntity*)CloneEntity(pcEnvironmentEntity));
		}

		for (CEnvironmentEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcPillars)
		{
			pcNewRoom->m_pcPillars.push_back((CEnvironmentEntity*)CloneEntity(pcEnvironmentEntity));
		}

		for (CDoorEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcDoors)
		{
			pcNewRoom->m_pcDoors.push_back((CDoorEntity*)CloneEntity(pcEnvironmentEntity));
		}

		for(CSpawnEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcSpawns)
		{
			pcNewRoom->m_pcSpawns.push_back((CSpawnEntity*)CloneEntity(pcEnvironmentEntity));
		}

		for (CEnvironmentEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcClutter)
		{
			pcNewRoom->m_pcClutter.push_back((CEnvironmentEntity*)CloneEntity(pcEnvironmentEntity));
		}

		for (CTrapEntity* pcTrapEntity : pcRoomToCopy->m_pcTraps)
		{
			pcNewRoom->m_pcTraps.push_back((CTrapEntity*)CloneEntity(pcTrapEntity));
		}

		for (CEnvironmentEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcLights)
		{
			pcNewRoom->m_pcLights.push_back((CEnvironmentEntity*)CloneEntity(pcEnvironmentEntity));

			for (int i = 0; i < pcNewRoom->m_pcLights.size(); i++)
			{
				if (pcEnvironmentEntity->top == true)
				{
					pcNewRoom->m_pcLights[i]->top = true;
				}

				if (pcEnvironmentEntity->bottom == true)
				{
					pcNewRoom->m_pcLights[i]->bottom = true;
				}

				if (pcEnvironmentEntity->left == true)
				{
					pcNewRoom->m_pcLights[i]->left = true;
				}

				if (pcEnvironmentEntity->right == true)
				{
					pcNewRoom->m_pcLights[i]->right = true;
				}
			}	
		}

		if (pcRoomToCopy->IsActive() == true)
		{
			pcNewRoom->SetActiveState(true);
		}
		else
		{
			pcNewRoom->SetActiveState(false);
		}

		return pcNewRoom;
	}
	
	if (pNewEntity->m_nEntityType == eEntity::PROJECTILE)
	{
		((CProjectileEntity*)(pNewEntity))->bIsTriggerStay = ((CProjectileEntity*)(pcEntityToCopy))->bIsTriggerStay;
	}

	pNewEntity->m_pcRoom = pcEntityToCopy->m_pcRoom;

	if (pcEntityToCopy->IsActive() == true)
	{
		pNewEntity->SetActiveState(true);
	}
	else
	{
		pNewEntity->SetActiveState(false);
	}

	
	return pNewEntity;
}

void CEntityManager::NotifyCollisionMessage(int nCollider, int nCollidingWith, int nCollisionType)
{
	IEntity* pcEntityA = GetEntity(nCollider);
	IEntity* pcEntityB = GetEntity(nCollidingWith);

	switch(nCollisionType)
	{
		case eCollisionState::COLLISION_ENTER:
		{
			if(pcEntityA && pcEntityB)
			{
				pcEntityA->OnCollisionEnter(pcEntityB);
			}

			if(pcEntityA && pcEntityB)
			{
				pcEntityB->OnCollisionEnter(pcEntityA);
			}
			break;
		}

		case eCollisionState::COLLISION_STAY:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnCollisionStay(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnCollisionStay(pcEntityA);

			break;
		}

		case eCollisionState::COLLISION_EXIT:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnCollisionExit(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnCollisionExit(pcEntityA);

			break;
		}

		case eCollisionState::TRIGGER_ENTER:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnTriggerEnter(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnTriggerEnter(pcEntityA);

			break;
		}

		case eCollisionState::TRIGGER_STAY:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnTriggerStay(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnTriggerStay(pcEntityA);

			break;
		}

		case eCollisionState::TRIGGER_EXIT:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnTriggerExit(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnTriggerExit(pcEntityA);

			break;
		}

		default:
			break;
		}
	}

void CEntityManager::ProcessDeletionQueue()
{
	for (size_t i = 0; i < m_pcDeletionQueue.size(); i++)
	{
		if (m_pcDeletionQueue[i])
		{
			DeleteEntity(m_pcDeletionQueue[i]);
			//m_pcDeletionQueue.erase(m_pcDeletionQueue.begin());
		}
	}
	/*for (IEntity* pcEntity : m_pcDeletionQueue)
	{
		if (pcEntity)
		{
			DeleteEntity(pcEntity);
			m_pcDeletionQueue.erase(m_pcDeletionQueue.begin());
		}
	}
*/
	m_pcDeletionQueue.clear();
}

CCameraEntity * CEntityManager::GetCameraEntity()
{
	return m_pcCamera;
}

CEntityManager::~CEntityManager()
{
	DeleteAllEntities();
}
