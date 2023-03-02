#pragma once
#include "EnumTypes.h"

#include "MageController.h"
#include "ChickenController.h"
#include "VikingController.h"
#include "ArcherController.h"
#include "KnightController.h"

#include <vector>

class IComponent;


class CBoxCollider;
class CCapsuleCollider;
class CRigidbody;
class CMeshRenderer;
class CFireDemonController;
class CCameraController;
class CAnimatorComponent;
class CStats;
class CProjectileComponent;
class CRoomEntity;
class CInventory;
class CTransform;
class CLightComponent;
class CSpikeTrapController;
class IEnemyController;
class CSpawnEntity;
class CPathSurface;
class CPathAgent;
class CItemEntity;
class CParticleEmitter;
class CIndicatorController;
class CTrapEntity;

class IEntity
{
	bool m_bState;

public:
	IEntity(int nId);
	
	int m_nEntityId;
	int m_nEntityType;

	CRoomEntity* m_pcRoom;

	std::vector<IComponent*> m_pcComponents;

	template <typename T>
	T* GetComponent()
	{
		for (IComponent* pnComponent : m_pcComponents)
		{
			switch (pnComponent->m_nComponentType)
			{
				case eComponent::TRANSFORM:
				{
					if (std::is_same<T, CTransform>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::BOX_COLLIDER:
				{
				if (std::is_same<T, CBoxCollider>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

			case eComponent::CAPSULE_COLLIDER:
			{
				if (std::is_same<T, CCapsuleCollider>::value)
				{
					return (T*)pnComponent;
				}

				break;
			}

				case eComponent::RIGIDBODY:
				{
					if (std::is_same<T, CRigidbody>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::MESH_RENDERER:
				{
					if (std::is_same<T, CMeshRenderer>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::FIRE_DEMON_CONTROLLER:
				{
					if (std::is_same<T, CFireDemonController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::CAMERA_CONTROLLER:
				{
					if (std::is_same<T, CCameraController>::value)
					{
						return (T*)pnComponent;
					}
					break;
				}
			
				case eComponent::ANIMATOR:
				{
					if(std::is_same<T, CAnimatorComponent>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::MAGE_CONTROLLER:
				{
					if(std::is_same<T, CMageController>::value)
					{
						return (T*)pnComponent;
					}

					else if (std::is_base_of<T, CMageController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::CHICKEN_CONTROLLER:
				{
					if (std::is_same<T, CChickenController>::value)
					{
						return (T*)pnComponent;
					}

					else if (std::is_base_of<T, CChickenController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::VIKING_CONTROLLER:
				{
					if (std::is_same<T, CVikingController>::value)
					{
						return (T*)pnComponent;
					}

					else if (std::is_base_of<T, CVikingController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::ARCHER_CONTROLLER:
				{
					if (std::is_same<T, CArcherController>::value)
					{
						return (T*)pnComponent;
					}
					else if (std::is_base_of<T, CArcherController>::value)
					{
						return (T*)pnComponent;
					}
					break;
				}

				case eComponent::KNIGHT_CONTROLLER:
				{
					if (std::is_same<T, CKnightController>::value)
					{
						return (T*)pnComponent;
					}

					else if (std::is_base_of<T, CKnightController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::STATS:
				{
					if(std::is_same<T, CStats>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::PROJECTILE_COMPONENT:
				{
					if(std::is_same<T, CProjectileComponent>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::INVENTORY:
				{
					if (std::is_same<T, CInventory>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::LIGHT:
				{
					if(std::is_same<T, CLightComponent>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::SPIKE_TRAP_COMPONENT:
				{
					if (std::is_same<T, CSpikeTrapController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::PATH_SURFACE:
				{
					if (std::is_same<T, CPathSurface>::value)
					{
						return (T*)pnComponent;
			}

					break;
		}

				case eComponent::PATH_AGENT:
				{
					if (std::is_same<T, CPathAgent>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::PARTICLE_EMITTER:
				{
					if (std::is_same<T, CParticleEmitter>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::CLICK_INDICATOR:
				{
					if(std::is_same<T, CIndicatorController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}
			}
		}

		return nullptr;
	}


	bool IsActive();

	void SetActiveState(bool bState);

	virtual void OnTriggerEnter(IEntity* pcCollidingEntity);
	virtual void OnTriggerStay(IEntity* pcCollidingEntity);
	virtual void OnTriggerExit(IEntity* pcCollidingEntity);

	virtual void OnCollisionEnter(IEntity* pcCollidingEntity);
	virtual void OnCollisionStay(IEntity* pcCollidingEntity);
	virtual void OnCollisionExit(IEntity* pcCollidingEntity);

	virtual~IEntity() = 0;
};

class CPlayerEntity : public IEntity
{
public:
	CPlayerEntity(int nId = -1);

	void Update();

	void OnTriggerEnter(IEntity* pcCollidingEntity);

	void OnTriggerExit(IEntity* pcCollidingEntity);

	void OnTriggerStay(IEntity* pcCollidingEntity);

	~CPlayerEntity();

	void AddPlayerLight(CLightComponent* cLight);
};

class CEnemyEntity : public IEntity
{
	void DeathAnimation();

public:
	CEnemyEntity(int nId = -1);

	void Update();

	void OnTriggerEnter(IEntity* pcCollidingEntity);
	void OnTriggerStay(IEntity* pcCollidingEntity);
	//void OnTriggerExit(IEntity* pcCollidingEntity);

	//void OnCollisionEnter(IEntity* pcCollidingEntity);
	//void OnCollisionStay(IEntity* pcCollidingEntity);
	//void OnCollisionExit(IEntity* pcCollidingEntity);

	~CEnemyEntity();
};

class CProjectileEntity : public IEntity
{
public:
	bool bIsTriggerStay;

	CProjectileEntity(int nId = -1);

	void OnTriggerEnter(IEntity* pcCollidingEntity);

	~CProjectileEntity();
};

class CCameraEntity : public IEntity
{
public:
	CCameraEntity(int nId = -1);

	void Update();

	~CCameraEntity();
};

class CEnvironmentEntity : public IEntity
{
public:
	CEnvironmentEntity(int nID = -1);

	bool top = false;
	bool bottom = false;
	bool left = false;
	bool right = false;

	~CEnvironmentEntity();
};

class CDoorEntity : public IEntity
{
public:
	CDoorEntity(int nID = -1);

	~CDoorEntity();
};

class CRoomEntity : public IEntity
{
	std::vector<CLightComponent*>	m_pLights;
	std::vector<CItemEntity*> m_pcItems;
	
public:

	int									m_nEnemyCount;
	std::vector<CEnemyEntity*>			m_pcEnemies;
	std::vector<CEnvironmentEntity*>	m_pcWalls;
	std::vector<CDoorEntity*>			m_pcDoors;
	std::vector<CEnvironmentEntity*>	m_pcPillars;
	std::vector<CEnvironmentEntity*>	m_pcLights;
	std::vector<CSpawnEntity*>			m_pcSpawns;
	std::vector<CEnvironmentEntity*>	m_pcClutter;

	std::vector<CTrapEntity*>			m_pcTraps;

	std::vector<CRoomEntity*>			m_pcNeighbors;

	bool Top;
	bool Bottom;
	bool Left;
	bool Right;

	bool Entities;
	bool Doors;
	bool Random;
	bool Visited;

	CRoomEntity(int nId = -1);

	void SpawnEntities();

	void DeSpawnEntities();

	void SpawnDoors();

	void DeSpawnDoors();

	void TurnEnemiesOn();

	

	void AddLight(CLightComponent* pNewLight);

	void RemoveLight(CLightComponent* pLight);

	void AddItem(CItemEntity* pItem);

	void RemoveItem(CItemEntity* pItem);

	std::vector<CLightComponent*>	GetLights();

	std::vector<CItemEntity*>		GetItems();

	~CRoomEntity();

};

class CTrapEntity : public IEntity
{
public:
	CTrapEntity(int nId);

	~CTrapEntity();
};

class CSpawnEntity : public IEntity
{
public:
	bool Taken = false;

	CSpawnEntity(int nId);
	~CSpawnEntity();
};

class CItemEntity : public IEntity
{
public:
	int m_nItemType, m_nItemClass;
	bool m_bAcquired;
	CItemEntity(int nId);
	~CItemEntity();

	void CreateRandomItem(IEntity* pEntity);
	void CreateItem(int nItemClass, int nItemType, IEntity* pEntity);
};

class CParticleEntity : public IEntity
{
public:
	float m_fLifetime;
	float m_fRotationSpeed;

	CParticleEntity(int nId);
};
