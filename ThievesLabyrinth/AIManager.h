#pragma once
#include "System.h"
#include "Math.h"
#include <vector>

class CComponentManager;
class IEnemyController;
class CPathAgent;
class CRoomEntity;

class CAIManager : public ISystem
{
	CComponentManager* m_pcComponentManager;
	std::vector<IEnemyController*> m_pcEnemyControllers;
	float m_fSeparationStrength;

	CPathAgent* m_pcPlayerAgent;
	CRoomEntity* currentRoom;

	bool m_bCalculateWeights;

	CMath::TVECTOR3 GetSeparationAccel(IEnemyController * pcEnemy);

	void UpdateEnemyBehavior(float fDeltaTime);
	void UpdateFlockingBehavior(float fDeltaTime);

public:
	CAIManager(CComponentManager* pcComponentManager);
	~CAIManager();

	void Reset();

	// DEBUG
	void FindPlayerPath();
	void CheckForAgent();

	void Update(float fDeltaTime);

};

