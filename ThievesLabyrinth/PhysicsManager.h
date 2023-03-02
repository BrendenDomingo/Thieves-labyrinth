#pragma once
#include "System.h"

#include "Math.h"

#include <vector>
#include <map>
#include <array>
#include <unordered_map>


#define ROOM_HEIGHT 100
#define ROOM_WIDTH 100
#define CELL_SIZE 10


class IEntity;
class CTransform;
class CRigidbody;
class ICollider;
class CBoxCollider;
class CCapsuleCollider;
class CComponentManager;
class CTransform;
class CRoomEntity;
class CPlayerEntity;

struct TRayHit
{
	IEntity* tEntity;
	CMath::TVECTOR3 tEntryPoint;
	CMath::TVECTOR3 tExitPoint;
};

class CEntityMask
{
public:
	int nValue;

	CEntityMask() : nValue(0) {}
	void Add(int nEntityType);
	void Remove(int nEnityType);
	void Invert();
};

class CPhysicsManager : public ISystem
{
private:

	struct TCollision
	{
		CMath::TVECTOR3 tAxis;
		float fOffset;
	};

	struct TCoordinate
	{
		int x, y;
		TCoordinate(int _x, int _y) : x(_x), y(_y) {}
	};

	struct TPlane
	{
		CMath::TVECTOR3 tNormal;
		float fDistance;
	};


	std::vector<CTransform*>*										m_pcTransforms;
	std::vector<CRigidbody*>*										m_pcRigidbodies;
	std::vector<ICollider*>*										m_pcColliders;
	static std::vector<ICollider*>									m_cStaticColliders;
	static std::vector<ICollider*>									m_cDynamicColliders;
	std::unordered_map<ICollider*, std::vector<ICollider*>>			m_cCollisions;
	std::unordered_map<ICollider*, std::vector<TCoordinate>>		m_StaticGridPositions;
	std::unordered_map<ICollider*, std::vector<TCoordinate>>		m_DynamicGridPositions;

	CComponentManager*												m_pcComponentManager;

	CRoomEntity*													m_pcCurrentRoom;
	CPlayerEntity*													m_pcPlayerEntity;

	CMath::TVECTOR3													m_tRoomPosition;

	std::vector<ICollider*>											m_Grid [ROOM_WIDTH / CELL_SIZE][ROOM_HEIGHT / CELL_SIZE];

	static TPlane													m_FloorPlane;

public:


	CPhysicsManager(CComponentManager* pcComponentManager);
	~CPhysicsManager();

	void Update(float fDeltaTime);

	static bool Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, float fMaxDistance);
	static bool Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit& tRayHit, CEntityMask* cMask = nullptr);
	static bool Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit& tRayHit, float fMaxDistance, CEntityMask* cMask = nullptr);
	static bool RaycastMouseToFloor(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, CMath::TVECTOR3& tFloorPosition);

	static bool RayOverlap(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, std::vector<TRayHit>& results, float fMaxDistance, CEntityMask* cMask = nullptr);

private:

#ifdef MULTI_THREADING

#define BITSETSIZE			4096
#define RIGIDBODYTHREADS	3

	std::mutex										m_cMainMutex;
	std::mutex										m_cMutexLock;

	std::condition_variable							m_cGunCondition;
	std::condition_variable							m_cMainWait;

	std::vector<std::thread>						threads;

	std::bitset<BITSETSIZE>							m_cBitMask;

	float											m_fDelta;
	short											m_nFinishedThreads;
	bool											m_bShutDownFlag;
#endif // MUTLI_THREADING	

	void UpdateComponentLists();
	void UpdateRigidBodies(double dDeltaTime);
	void UpdateCollisions();

	void PairwiseCollisionCheck(ICollider* pcA, ICollider* pcB, int nCollisionType, bool bIsTriggerCollision);
	int GetCollisionType(ICollider* pcA, ICollider* pcB);

	void UpdateBoxBoxCollision(CBoxCollider* pcA, CBoxCollider* pcB, bool bIsTrigger, bool& bIsColliding);
	void UpdateCapsuleCapsuleCollision(CCapsuleCollider* pcA, CCapsuleCollider* pcB, bool bIsTrigger, bool& bIsColliding);
	void UpdateBoxCapsuleCollision(CBoxCollider* pcA, CCapsuleCollider* pcB, bool bIsTrigger, bool bReverseOrder, bool& bIsColliding);

	bool IsCollidingBoxBox(CBoxCollider* pcA, CBoxCollider* pcB, TCollision& tCollision);
	bool IsCollidingCapsuleCapsule(CCapsuleCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollision);
	bool IsCollidingBoxCapsule(CBoxCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollision);

	bool IsIntersecting(CMath::TVECTOR3 tAxis, CBoxCollider* pA, CBoxCollider* pB, float& fMinOverlap, CMath::TVECTOR3& tMinAxis);

	void ResolveCollision(CBoxCollider* pcA, CBoxCollider* pcB, TCollision& tCollision);
	void ResolveCollision(CCapsuleCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollision);
	void ResolveCollision(CBoxCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollsion);

	bool IsNewCollision(ICollider* pcA, ICollider* pcB);

	bool IsCollisionExit(ICollider* pcA, ICollider* pcB, int& nIndexA, int& nIndexB);

	static float Clamp(float fValue, float fMin, float fMax);
	static float GetRatio(float fValue, float fMin, float fMax);

	static void ClosestPtSegmentSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3& tPs, CMath::TVECTOR3& tQt);
	static void ClosestPtLineSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tDirection, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3& tPs, CMath::TVECTOR3& tQt);
	static void ClosestPtPointOBB(CMath::TVECTOR3 tPoint, CBoxCollider* pcB, CMath::TVECTOR3& tResult);
	static void ClosestPtPointSegment(CMath::TVECTOR3 tPoint, CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3& tResult);

	int FindGridPositionX(CMath::TVECTOR3 tPosition, int min, int max);
	int FindGridPositionZ(CMath::TVECTOR3 tPosition, int min, int max);
	void GetMinMaxOverlap(CBoxCollider* collider, CMath::TVECTOR2& tOverlapX, CMath::TVECTOR2& tOverlapZ);
	void GetMinMaxOverlap(CCapsuleCollider* collider, CMath::TVECTOR2& tOverlapX, CMath::TVECTOR2& tOverlapZ);
	void SetStaticGridPositions(ICollider* collider, CMath::TVECTOR2 tOverlapX, CMath::TVECTOR2 tOverlapZ);
	void SetDynamicGridPositions(ICollider* collider, CMath::TVECTOR2 tOverlapX, CMath::TVECTOR2 tOverlapZ);

	static bool IsCollidingRayBox(CBoxCollider* pcBoxCollider, CMath::TVECTOR3 tRayOrigin, CMath::TVECTOR3 tRayDirection, float fMaxDistance, CMath::TVECTOR3& tCollisionPoint, CMath::TVECTOR3& tCollisionExit);
	static bool IsCollidingRayCapsule(CCapsuleCollider* pCapsuleCollider, CMath::TVECTOR3 tRayOrigin, CMath::TVECTOR3 tRayDirection, float fMaxDistance, CMath::TVECTOR3 & tCollisionEntry, CMath::TVECTOR3& tCollisionExit);
};

