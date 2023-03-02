#pragma once
#define OPTION 1

#include "EventManager.h"
#include "EnumTypes.h"
#include "Pool.h"
#include <vector>

class IEntity;
class CComponentManager;

class CPlayerEntity;
class CEnemyEntity;
class CProjectileEntity;
class CEnemyEntity;
class CCameraEntity;
class CRoomEntity;
class CEnvironmentEntity;
class CDoorEntity;
class CTrapEntity;
class IComponent;
class CSpawnEntity;
class CPathSurface;
class CItemEntity;
class CParticle;
class CParticleEntity;

class CEntityManager 
{
	static CComponentManager*				m_pcComponentManager;

	static CPlayerEntity*					m_pcPlayer;
	static CCameraEntity*					m_pcCamera;


	static std::vector<CEnemyEntity*>		 m_pcEnemies;
	static std::vector<CProjectileEntity*>	 m_pcProjectiles;
	static std::vector<CRoomEntity*>		 m_pcRooms;
	static std::vector<CEnvironmentEntity*>  m_pcEnvironmentEntities;
	static std::vector<CDoorEntity*>		 m_pcDoorEntities;
	static std::vector<CTrapEntity*>		 m_pcTrapEntities;
	static std::vector<CSpawnEntity*>		 m_pcSpawnEntities;
	static std::vector<CItemEntity*>		 m_pcItemEntities;
	static std::vector<CParticleEntity*>	 m_pcParticleEntities;

	static std::vector<IEntity*>			m_pcDeletionQueue;

	static int								m_nIdCount;

	static int DeleteEntity(IEntity* pcEntity);

public:
	CEntityManager(CComponentManager* pcComponentManager);

	static IEntity* CreateEntity(int nType, int nOption = -1);

	static IComponent* AddComponentToEntity(IEntity* pcEntity, int nComponentType);

	int RemoveComponentFromEntity(IEntity* pcEntity, int nComponentType);

	static void AddEntityToDeletionQueue(IEntity* pcEntity);

	static int GetEntityCount(int nType);

	static IEntity* GetEntity(int nID);

	static void DeleteAllEntities();

	static IEntity* CloneEntity(IEntity* pcEntityToCopy);

	void NotifyCollisionMessage(int nCollider, int nCollidingWith, int nCollisionType);

	static void ProcessDeletionQueue();

	static CCameraEntity* GetCameraEntity();

	~CEntityManager();
};

