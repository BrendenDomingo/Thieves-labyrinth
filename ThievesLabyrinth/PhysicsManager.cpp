#include "PhysicsManager.h"
#include "EventManager.h"

//#include "Time.h"
#include "Level.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Rigidbody.h"
#include "Entity.h"
#include "Transform.h"
#include "Stats.h"

//#include <iostream>
#include <limits>

#define OUT
#define COLLISION_OFFSET 0.02f

std::vector<ICollider*> CPhysicsManager::m_cDynamicColliders;
std::vector<ICollider*> CPhysicsManager::m_cStaticColliders;
CPhysicsManager::TPlane CPhysicsManager::m_FloorPlane;

CPhysicsManager::CPhysicsManager(CComponentManager* pComponentManager)
{
	m_pcComponentManager = pComponentManager;
	m_pcCurrentRoom = nullptr;
	m_pcPlayerEntity = nullptr;
	m_pcTransforms = m_pcComponentManager->GetTransforms();
	m_pcRigidbodies = m_pcComponentManager->GetRigidbodies();
	m_pcColliders = m_pcComponentManager->GetColliders();
	m_FloorPlane.tNormal = CMath::TVECTOR3(0, 1, 0);
	m_FloorPlane.fDistance = 0.0f;

#ifdef MULTI_THREADING

	m_nFinishedThreads = 0;
	m_fDelta = 0.0;
	m_bShutDownFlag = false;

	m_cBitMask.reset();
	for (int i = 0; i < RIGIDBODYTHREADS; i++)
		threads.emplace_back(&CPhysicsManager::UpdateRigidBodies, this, 0.0);

#endif // MULTI_THREADING

}
CPhysicsManager::~CPhysicsManager()
{

#ifdef MULTI_THREADING

	m_cMainMutex.lock();
	m_bShutDownFlag = true;
	m_cGunCondition.notify_all();
	m_cMainMutex.unlock();

	for (auto& thread : threads) thread.join();

#endif // 


}

void CPhysicsManager::Update(float fDeltaTime)
{
	UpdateComponentLists();

#ifdef MULTI_THREADING

	m_fDelta = fDeltaTime;
	std::unique_lock<std::mutex> cMainULock(m_cMainMutex);
	m_cGunCondition.notify_all();
	m_cMainWait.wait(cMainULock, [&](){ return m_nFinishedThreads == RIGIDBODYTHREADS; });
	cMainULock.unlock();
	m_nFinishedThreads = 0;

#else
	UpdateRigidBodies(fDeltaTime);
#endif

	UpdateCollisions();
}


void CPhysicsManager::UpdateComponentLists()
{
	if (m_pcCurrentRoom != CLevel::m_pcCurrentRoom)
	{
		m_pcCurrentRoom = CLevel::m_pcCurrentRoom;
		if (m_pcCurrentRoom != nullptr)
		{
			m_tRoomPosition = m_pcCurrentRoom->GetComponent<CTransform>()->GetPosition();
		}
		m_StaticGridPositions.clear();
	}

	if (!m_pcPlayerEntity)
	{
		TEntityMessage message = TEntityMessage(0);
		m_pcPlayerEntity = (CPlayerEntity*)CEventManager::SendEntityMessage(message);
	}

	size_t nDynamic = m_cDynamicColliders.size(), nStatic = m_cStaticColliders.size(), nDynamicGrid = m_DynamicGridPositions.size();
	m_DynamicGridPositions.clear();
	m_cDynamicColliders.clear();
	m_cStaticColliders.clear();
	m_DynamicGridPositions.reserve(nDynamicGrid);
	m_cDynamicColliders.reserve(nDynamic);
	m_cStaticColliders.reserve(nStatic);

	for (ICollider* collider : *m_pcColliders)
	{
		if (!collider->IsActive())
		{
			if (m_cCollisions.count(collider) > 0 && m_cCollisions[collider].size() > 0)
			{
				for (int i = (int)m_cCollisions[collider].size() - 1; i >= 0; i--)
				{
					ICollider* other = m_cCollisions[collider][i];
					bool bIsTriggerCollision = collider->IsTrigger() || other->IsTrigger();

					for (int j = 0; j < (int)m_cCollisions[other].size(); j++)
					{
						if (m_cCollisions[other][j] == collider)
						{
							m_cCollisions[other].erase(m_cCollisions[other].begin() + j);
							m_cCollisions[collider].erase(m_cCollisions[collider].begin() + i);
						}
					}
				}

				m_cCollisions.erase(collider);
			}
			continue;
		}

		if (collider->m_pcOwner->m_pcRoom != m_pcCurrentRoom)
			continue;

		CRigidbody* pcRigidbody = collider->m_pcOwner->GetComponent<CRigidbody>();
		bool bIsDynamic = pcRigidbody != nullptr;

		if (!bIsDynamic)
		{
			m_cStaticColliders.push_back(collider);

			if (m_StaticGridPositions.count(collider) > 0)
				continue;

			if (collider->m_nComponentType == eComponent::BOX_COLLIDER)
			{
				CBoxCollider* pBoxCollider = (CBoxCollider*)collider;

				CMath::TVECTOR2 tOverlapX, tOverlapZ;
				GetMinMaxOverlap(pBoxCollider, tOverlapX, tOverlapZ);
				SetStaticGridPositions(collider, tOverlapX, tOverlapZ);
			}

			else if (collider->m_nComponentType == eComponent::CAPSULE_COLLIDER)
			{
				CCapsuleCollider* pCapCollider = (CCapsuleCollider*)collider;

				CMath::TVECTOR2 tOverlapX, tOverlapZ;
				GetMinMaxOverlap(pCapCollider, tOverlapX, tOverlapZ);
				SetStaticGridPositions(collider, tOverlapX, tOverlapZ);
			}
		}

		else
		{
			m_cDynamicColliders.push_back(collider);

			if (collider->m_nComponentType == eComponent::BOX_COLLIDER)
			{
				CBoxCollider* pBoxCollider = (CBoxCollider*)collider;

				CMath::TVECTOR2 tOverlapX, tOverlapZ;
				GetMinMaxOverlap(pBoxCollider, tOverlapX, tOverlapZ);
				SetDynamicGridPositions(collider, tOverlapX, tOverlapZ);
			}

			else if (collider->m_nComponentType == eComponent::CAPSULE_COLLIDER)
			{
				CCapsuleCollider* pCapCollider = (CCapsuleCollider*)collider;

				CMath::TVECTOR2 tOverlapX, tOverlapY;
				GetMinMaxOverlap(pCapCollider, tOverlapX, tOverlapY);
				SetDynamicGridPositions(collider, tOverlapX, tOverlapY);
			}
		}
	}
}
void CPhysicsManager::UpdateRigidBodies(double dDeltaTime)
{
#ifdef MULTI_THREADING

	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);

	m_cGunCondition.wait(cGunLock);
	cGunLock.unlock();

	short i;
	while (!m_bShutDownFlag)
	{
		i = -1;
		for (CRigidbody* rigidbody : *m_pcRigidbodies)
		{
			i++;
			if (!rigidbody->IsActive())
				continue;

			m_cMainMutex.lock();
			if (!(m_cBitMask[i]))
			{
				m_cBitMask[i] = 1;
				m_cMainMutex.unlock();						

				CTransform* pcTransform = rigidbody->m_pcOwner->GetComponent<CTransform>();
				pcTransform->SetPosition(pcTransform->GetPosition() + (rigidbody->GetVelocity() * m_fDelta));				

				continue;
			}
			m_cMainMutex.unlock();
		}

		cGunLock.lock();
		m_cMainMutex.lock();
		if (++m_nFinishedThreads == RIGIDBODYTHREADS)
			m_cBitMask.reset();
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();
		m_cGunCondition.wait(cGunLock);
		cGunLock.unlock();
	}
#else

	for (int i = 0; i < m_pcRigidbodies->size(); i++)
	{
		CTransform* pcTransform = (*m_pcRigidbodies)[i]->m_pcOwner->GetComponent<CTransform>();

		if (pcTransform && pcTransform->IsActive())
		{
			CMath::TVECTOR3 tVelocity = (*m_pcRigidbodies)[i]->GetVelocity();

			CMath::TVECTOR3 tNewPosition = pcTransform->GetPosition();
			tNewPosition.x += tVelocity.x * (float)dDeltaTime;
			tNewPosition.y += tVelocity.y * (float)dDeltaTime;
			tNewPosition.z += tVelocity.z * (float)dDeltaTime;

			pcTransform->SetPosition(tNewPosition);
		}
}

#endif // !MULTI_THREADING

}
void CPhysicsManager::UpdateCollisions()
{
	for (int i = 0; i < m_cDynamicColliders.size(); i++)
	{
		// Get the collider to be checked against the rest of colliders
		ICollider* pcA = m_cDynamicColliders[i];

		int nGridXPosition = FindGridPositionX(pcA->m_pcOwner->GetComponent<CTransform>()->GetPosition(), 0, ROOM_WIDTH / CELL_SIZE - 1);
		int nGridYPosition = FindGridPositionZ(pcA->m_pcOwner->GetComponent<CTransform>()->GetPosition(), 0, ROOM_HEIGHT / CELL_SIZE - 1);

		// Check against other dynamic objects that haven't been checked
		for (int j = i + 1; j < m_cDynamicColliders.size(); j++)
		{
			ICollider* pcB = m_cDynamicColliders[j];

			//Check if pcB is located in any of the neighboring cells of pcA
			for (TCoordinate coor : m_DynamicGridPositions[pcA])
			{
				for (TCoordinate coor2 : m_DynamicGridPositions[pcB])
				{
					if (coor.x == coor2.x && coor.y == coor2.y)
					{
						int nCollisionType = GetCollisionType(pcA, pcB);

						bool bIsTriggerCollision = pcA->IsTrigger() || pcB->IsTrigger();

						PairwiseCollisionCheck(pcA, pcB, nCollisionType, bIsTriggerCollision);

						break;
					}
				}
			}
		}

		// Check against all static objects
		for (int j = 0; j < m_cStaticColliders.size(); j++)
		{
			ICollider* pcB = m_cStaticColliders[j];

			//Check if pcB is located in any of the neighboring cells of pcA
			for (TCoordinate coor : m_DynamicGridPositions[pcA])
			{
				bool found = false;
				for (TCoordinate coor2 : m_StaticGridPositions[pcB])
				{
					// if it's found, then check for collision
					if (coor.x == coor2.x && coor.y == coor2.y)
					{
						int nCollisionType = GetCollisionType(pcA, pcB);

						bool bIsTriggerCollision = pcA->IsTrigger() || pcB->IsTrigger();

						PairwiseCollisionCheck(pcA, pcB, nCollisionType, bIsTriggerCollision);

						found = true;

						break;
					}
				}

				if (found)
					break;
			}
		}
	}
}

bool CPhysicsManager::Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, float fMaxDistance)
{
	for (ICollider* collider : m_cDynamicColliders)
	{
		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
			{
				return true;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (IsCollidingRayBox((CBoxCollider*)collider, tOrigin, CMath::Vector3Normalize(tDirection), fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
			{
				return true;
			}

			break;
		}

		default:
			break;
		}
	}

	for (ICollider* collider : m_cStaticColliders)
	{
		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
			{
				return true;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (IsCollidingRayBox((CBoxCollider*)collider, tOrigin, CMath::Vector3Normalize(tDirection), fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
			{
				return true;
			}

			break;
		}
		default:
			break;
		}
	}

	return false;
}
bool CPhysicsManager::Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit & tRayHit, CEntityMask* cMask)
{
	float fMinDistance = std::numeric_limits<float>::infinity();

	if (CMath::Vector3Dot(tDirection, tDirection) != 1.0f)
		tDirection = CMath::Vector3Normalize(tDirection);

	for (ICollider* collider : m_cDynamicColliders)
	{
		if (cMask)
		{
			if (!(cMask->nValue & 1 << collider->m_pcOwner->m_nEntityType))
				continue;
		}

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	for (ICollider* collider : m_cStaticColliders)
	{
		if (cMask)
		{
			if (!(cMask->nValue & 1 << collider->m_pcOwner->m_nEntityType))
				continue;
		}

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	bool bHasRayCollided = fMinDistance < std::numeric_limits<float>::infinity();

	return bHasRayCollided;
}
bool CPhysicsManager::Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit & tRayHit, float fMaxDistance, CEntityMask* cMask)
{
	float fMinDistance = std::numeric_limits<float>::infinity();

	if (CMath::Vector3Dot(tDirection, tDirection) != 1.0f)
		tDirection = CMath::Vector3Normalize(tDirection);

	for (ICollider* collider : m_cDynamicColliders)
	{
		if (collider->m_pcOwner == nullptr)
			continue;

		if (cMask)
		{
			if (!(cMask->nValue & 1 << collider->m_pcOwner->m_nEntityType))
				continue;
		}

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	for (ICollider* collider : m_cStaticColliders)
	{
		if (cMask)
		{
			if (!(cMask->nValue & 1 << collider->m_pcOwner->m_nEntityType))
				continue;
		}

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tEntryPoint = tCollisionPoint;
				tRayHit.tExitPoint = tCollisionExit;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	bool bHasRayCollided = fMinDistance < std::numeric_limits<float>::infinity();

	return bHasRayCollided;
}
bool CPhysicsManager::RaycastMouseToFloor(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, CMath::TVECTOR3 & tFloorPosition)
{
	/*
	An intersecting point in the plane would be:
	point = ray.origin + ray.direction * t;

	A point that lies in the plane satisfies the equation:
	dot(point, plane.normal) - plane.distance = 0

	Replacing the point with the intersecting point equation:
	dot((ray.origin + ray.direction * t), plane.normal) - plane.distance = 0;

	Solving for t:
	dot((ray.origin + ray.direction * t), plane.normal) = plane.distance;
	dot(ray.origin, plane.normal) + dot(ray.direction * t, plane.normal) = plane.distance;
	dot(ray.direction * t, plane.normal) = plane.distance - dot(ray.origin, plane.normal);
	t * dot(ray.direction, plane.normal) = plane.distance - dot(ray.origin, plane.normal);

	t = (plane.distance - dot(ray.origin, plane.normal) / dot(ray.direction, plane.normal);

	Then, return the result of the intersecting point equation given t.
	*/

	if (CMath::Vector3Dot(tDirection, tDirection) != 1.0f)
		tDirection = CMath::Vector3Normalize(tDirection);

	float fDotDirectionNorm = CMath::Vector3Dot(tDirection, m_FloorPlane.tNormal);

	if (fDotDirectionNorm >= 0)
	{
		CEventManager::SendDebugMessage(TDebugMessage(std::string("Why is the camera below the ground??")));
		return false;
	}

	float fDotOriginNorm = CMath::Vector3Dot(tOrigin, m_FloorPlane.tNormal);

	float fDistanceAlongRay = (m_FloorPlane.fDistance - fDotOriginNorm) / fDotDirectionNorm;

	if (fDistanceAlongRay < 0)
		return false;

	tFloorPosition = tOrigin + tDirection * fDistanceAlongRay;
	return true;
}

bool CPhysicsManager::RayOverlap(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, std::vector<TRayHit>& results, float fMaxDistance, CEntityMask* cMask)
{
	if (CMath::Vector3Dot(tDirection, tDirection) != 1.0f)
		tDirection = CMath::Vector3Normalize(tDirection);

	for (ICollider* collider : m_cDynamicColliders)
	{
		if (cMask)
		{
			if (!(cMask->nValue & 1 << collider->m_pcOwner->m_nEntityType))
				continue;
		}

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			
			TRayHit tRayHit;
			tRayHit.tEntity = collider->m_pcOwner;
			tRayHit.tEntryPoint = tCollisionPoint;
			tRayHit.tExitPoint = tCollisionExit;
			results.push_back(tRayHit);

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);


			TRayHit tRayHit;
			tRayHit.tEntity = collider->m_pcOwner;
			tRayHit.tEntryPoint = tCollisionPoint;
			tRayHit.tExitPoint = tCollisionExit;
			results.push_back(tRayHit);

			break;
		}
		default:
			break;
		}
	}

	for (ICollider* collider : m_cStaticColliders)
	{
		if (cMask)
		{
			if (!(cMask->nValue & 1 << collider->m_pcOwner->m_nEntityType))
				continue;
		}

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			
			TRayHit tRayHit;
			tRayHit.tEntity = collider->m_pcOwner;
			tRayHit.tEntryPoint = tCollisionPoint;
			tRayHit.tExitPoint = tCollisionExit;
			results.push_back(tRayHit);

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint, tCollisionExit;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint, OUT tCollisionExit))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			TRayHit tRayHit;
			tRayHit.tEntity = collider->m_pcOwner;
			tRayHit.tEntryPoint = tCollisionPoint;
			tRayHit.tExitPoint = tCollisionExit;
			results.push_back(tRayHit);

			break;
		}
		default:
			break;
		}
	}

	return results.size() > 0;
}

void CPhysicsManager::PairwiseCollisionCheck(ICollider* pcA, ICollider* pcB, int nCollisionType, bool bIsTriggerCollision)
{
	bool bIsColliding = false;
	if (nCollisionType == eCollisionTypes::BOX_BOX)
	{
		CBoxCollider* pcBoxA = (CBoxCollider*)pcA;
		CBoxCollider* pcBoxB = (CBoxCollider*)pcB;
		UpdateBoxBoxCollision(pcBoxA, pcBoxB, bIsTriggerCollision, OUT bIsColliding);
	}

	else if (nCollisionType == eCollisionTypes::CAPSULE_CAPSULE)
	{
		CCapsuleCollider* pcCapsuleA = (CCapsuleCollider*)pcA;
		CCapsuleCollider* pcCapsuleB = (CCapsuleCollider*)pcB;
		UpdateCapsuleCapsuleCollision(pcCapsuleA, pcCapsuleB, bIsTriggerCollision, OUT bIsColliding);
	}

	else if (nCollisionType == eCollisionTypes::BOX_CAPSULE)
	{
		if (pcA->m_nComponentType == eComponent::BOX_COLLIDER)
		{
			CBoxCollider* pcBoxColliderA = (CBoxCollider*)pcA;
			CCapsuleCollider* pcCapsuleColliderB = (CCapsuleCollider*)pcB;
			UpdateBoxCapsuleCollision(pcBoxColliderA, pcCapsuleColliderB, bIsTriggerCollision, false, OUT bIsColliding);
		}

		else
		{
			CCapsuleCollider* pcCapsuleColliderA = (CCapsuleCollider*)pcA;
			CBoxCollider* pcBoxColliderB = (CBoxCollider*)pcB;
			UpdateBoxCapsuleCollision(pcBoxColliderB, pcCapsuleColliderA, bIsTriggerCollision, true, OUT bIsColliding);
		}
	}

	if (bIsColliding)
	{
		if (IsNewCollision(pcA, pcB))
		{
			m_cCollisions[pcA].push_back(pcB);
			m_cCollisions[pcB].push_back(pcA);

			int nCollisionType = bIsTriggerCollision ? eCollisionState::TRIGGER_ENTER : eCollisionState::COLLISION_ENTER;
			TCollisionMessage message(pcA->m_pcOwner->m_nEntityId, pcB->m_pcOwner->m_nEntityId, nCollisionType);
			CEventManager::SendCollisionMessage(message);
		}

		else
		{
			int nCollisionType = bIsTriggerCollision ? eCollisionState::TRIGGER_STAY : eCollisionState::COLLISION_STAY;
			TCollisionMessage message(pcA->m_pcOwner->m_nEntityId, pcB->m_pcOwner->m_nEntityId, nCollisionType);
			CEventManager::SendCollisionMessage(message);
		}
	}

	else
	{
		int nIndexA, nIndexB;
		if (IsCollisionExit(pcA, pcB, OUT nIndexA, OUT nIndexB))
		{
			auto iterA = m_cCollisions[pcA].begin() + nIndexA;
			m_cCollisions[pcA].erase(iterA);

			auto iterB = m_cCollisions[pcB].begin() + nIndexB;
			m_cCollisions[pcB].erase(iterB);

			int nCollisionType = bIsTriggerCollision ? eCollisionState::TRIGGER_EXIT : eCollisionState::COLLISION_EXIT;
			TCollisionMessage message(pcA->m_pcOwner->m_nEntityId, pcB->m_pcOwner->m_nEntityId, nCollisionType);
			CEventManager::SendCollisionMessage(message);
		}
	}
}
int CPhysicsManager::GetCollisionType(ICollider* pcA, ICollider* pcB)
{
	int nComponentType = -1;
	switch (pcA->m_nComponentType)
	{
	case eComponent::BOX_COLLIDER:
	{
		switch (pcB->m_nComponentType)
		{

		case eComponent::BOX_COLLIDER:
		{
			nComponentType = eCollisionTypes::BOX_BOX;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			nComponentType = eCollisionTypes::BOX_CAPSULE;
			break;
		}

		default:
			break;
		}

		break;
	}

	case eComponent::CAPSULE_COLLIDER:
	{
		switch (pcB->m_nComponentType)
		{

		case eComponent::BOX_COLLIDER:
		{
			nComponentType = eCollisionTypes::BOX_CAPSULE;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			nComponentType = eCollisionTypes::CAPSULE_CAPSULE;
			break;
		}
		default:
			break;
		}

		break;
	}

	default:
		break;
	}

	return nComponentType;
}

void CPhysicsManager::UpdateBoxBoxCollision(CBoxCollider * pcA, CBoxCollider * pcB, bool bIsTriggerCollision, bool& bIsColliding)
{
	bIsColliding = false;

	TCollision tCollision;
	if (IsCollidingBoxBox(pcA, pcB, OUT tCollision))
	{
		if (!bIsTriggerCollision)
		{
			ResolveCollision(pcA, pcB, tCollision);
		}
		bIsColliding = true;
	}
}
void CPhysicsManager::UpdateCapsuleCapsuleCollision(CCapsuleCollider * pcA, CCapsuleCollider * pcB, bool bIsTriggerCollision, bool & bIsColliding)
{
	bIsColliding = false;

	TCollision tCollision;
	if (IsCollidingCapsuleCapsule(pcA, pcB, OUT tCollision))
	{
		if (!bIsTriggerCollision)
		{
			ResolveCollision(pcA, pcB, tCollision);
		}
		bIsColliding = true;
	}
}
void CPhysicsManager::UpdateBoxCapsuleCollision(CBoxCollider * pcA, CCapsuleCollider * pcB, bool bIsTriggerCollision, bool bReverseOrder, bool & bIsColliding)
{
	bIsColliding = false;

	TCollision tCollision;
	if (IsCollidingBoxCapsule(pcA, pcB, OUT tCollision))
	{
		if (!bIsTriggerCollision)
		{
			ResolveCollision(pcA, pcB, tCollision);
		}

		bIsColliding = true;
	}
}

bool CPhysicsManager::IsCollidingBoxBox(CBoxCollider* pA, CBoxCollider* pB, TCollision& tCollision)
{
	CMath::TVECTOR3 tLocalAxesA[3];
	CMath::TVECTOR3 tLocalAxesB[3];

	CTransform* tTransformA = pA->m_pcOwner->GetComponent<CTransform>();
	CTransform* tTransformB = pB->m_pcOwner->GetComponent<CTransform>();

	CMath::TMATRIX tTransformMatrixA = tTransformA->GetMatrix();
	CMath::TMATRIX tTransformMatrixB = tTransformB->GetMatrix();

	tLocalAxesA[0] = CMath::TVECTOR3(tTransformMatrixA.r[0].x, tTransformMatrixA.r[0].y, tTransformMatrixA.r[0].z);
	tLocalAxesB[0] = CMath::TVECTOR3(tTransformMatrixB.r[0].x, tTransformMatrixB.r[0].y, tTransformMatrixB.r[0].z);

	tLocalAxesA[1] = CMath::TVECTOR3(tTransformMatrixA.r[1].x, tTransformMatrixA.r[1].y, tTransformMatrixA.r[1].z);
	tLocalAxesB[1] = CMath::TVECTOR3(tTransformMatrixB.r[1].x, tTransformMatrixB.r[1].y, tTransformMatrixB.r[1].z);

	tLocalAxesA[2] = CMath::TVECTOR3(tTransformMatrixA.r[2].x, tTransformMatrixA.r[2].y, tTransformMatrixA.r[2].z);
	tLocalAxesB[2] = CMath::TVECTOR3(tTransformMatrixB.r[2].x, tTransformMatrixB.r[2].y, tTransformMatrixB.r[2].z);

	// Values used to resolve collision
	float fMinOverlap = std::numeric_limits<float>::infinity();
	CMath::TVECTOR3 tMinAxis;

	// If there is a gap between the OOBBs in any of their local axes, they're not colliding
	for (CMath::TVECTOR3 axis : tLocalAxesA)
	{
		if (!IsIntersecting(axis, pA, pB, fMinOverlap, tMinAxis))
			return false;
	}

	for (CMath::TVECTOR3 axis : tLocalAxesB)
	{
		if (!IsIntersecting(axis, pA, pB, fMinOverlap, tMinAxis))
			return false;
	}

	// If the direction of collision and the resolution axis point in the same direction, flip the resolution axis
	// Otherwise the object would move further inside the other object
	if (CMath::Vector3Dot((tTransformA->GetPosition() + pA->GetLocalCenter()) - (tTransformB->GetPosition() + pB->GetLocalCenter()), tMinAxis) < 0)
		tMinAxis *= -1;

	// Offset the resolution amount so they are still colliding by a minimum amount after the resolution
	// This way it is still possible to check if a collision is still in progress for the event manager
	tCollision.fOffset = fMinOverlap - COLLISION_OFFSET;
	tCollision.tAxis = tMinAxis;

	return true;
}
bool CPhysicsManager::IsCollidingCapsuleCapsule(CCapsuleCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
	CTransform* cTransformA = pcA->m_pcOwner->GetComponent<CTransform>();
	CTransform* cTransformB = pcB->m_pcOwner->GetComponent<CTransform>();

	CMath::TVECTOR3 tCenterPositionA = cTransformA->GetPosition() + pcA->GetCenter();
	CMath::TVECTOR3 tCenterPositionB = cTransformB->GetPosition() + pcB->GetCenter();

	CMath::TMATRIX tMatrixA = cTransformA->GetMatrix();
	CMath::TMATRIX tMatrixB = cTransformB->GetMatrix();

	CMath::TVECTOR3 tYAxisA = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrixA.r[1].x, tMatrixA.r[1].y, tMatrixA.r[1].z));
	CMath::TVECTOR3 tYAxisB = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrixB.r[1].x, tMatrixB.r[1].y, tMatrixB.r[1].z));

	CMath::TVECTOR3 tP0 = tCenterPositionA + tYAxisA * 0.5f * pcA->GetHeight();
	CMath::TVECTOR3 tP1 = tCenterPositionA - tYAxisA * 0.5f * pcA->GetHeight();

	CMath::TVECTOR3 tQ0 = tCenterPositionB + tYAxisB * 0.5f * pcB->GetHeight();
	CMath::TVECTOR3 tQ1 = tCenterPositionB - tYAxisB * 0.5f * pcB->GetHeight();

	CMath::TVECTOR3 tClosestPointA;
	CMath::TVECTOR3 tClosestPointB;
	ClosestPtSegmentSegment(tP0, tP1, tQ0, tQ1, OUT tClosestPointA, OUT tClosestPointB);

	CMath::TVECTOR3 tScaleA = cTransformA->GetScale();
	CMath::TVECTOR3 tScaleB = cTransformB->GetScale();

	float fRadiusA = pcA->GetRadius() * tScaleA.x;
	float fRadiusB = pcB->GetRadius() * tScaleB.x;

	CMath::TVECTOR3 tCollisionVector = tClosestPointA - tClosestPointB;
	float fDistance = CMath::Vector3Magnitude(tCollisionVector);
	float fRadiusSum = fRadiusA + fRadiusB;
	if (fDistance <= fRadiusSum)
	{
		tCollision.tAxis = CMath::Vector3Normalize(tCollisionVector);
		tCollision.fOffset = fRadiusSum - fDistance - COLLISION_OFFSET;
		return true;
	}

	return false;
}
bool CPhysicsManager::IsCollidingBoxCapsule(CBoxCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
	CTransform* tBoxTransform = pcA->m_pcOwner->GetComponent<CTransform>();
	CTransform* tCapTransform = pcB->m_pcOwner->GetComponent<CTransform>();

	CMath::TVECTOR3 tBoxCenter = pcA->GetLocalCenter() + tBoxTransform->GetPosition();

	CMath::TVECTOR3 tCapScale = tCapTransform->GetScale();
	float fCapRadius = pcB->GetRadius() * tCapScale.x;

	CMath::TVECTOR3 tBottom, tTop;
	pcB->GetMiddleSegment(tBottom, tTop);

	CMath::TVECTOR3 tSphereCenter;
	ClosestPtPointSegment(tBoxCenter, tBottom, tTop, tSphereCenter);

	CMath::TVECTOR3 tClosestAABBPoint;
	ClosestPtPointOBB(tSphereCenter, pcA, tClosestAABBPoint);

	float fDistance = CMath::Vector3Magnitude(tClosestAABBPoint - tSphereCenter);

	bool isColliding = fDistance <= fCapRadius;

	if (isColliding)
	{
		tCollision.fOffset = fCapRadius - fDistance - COLLISION_OFFSET;
		tCollision.fOffset = fmaxf(tCollision.fOffset, 0.0f);
		tCollision.tAxis = CMath::Vector3Normalize(tClosestAABBPoint - tSphereCenter);
	}

	return isColliding;
}
bool CPhysicsManager::IsIntersecting(CMath::TVECTOR3 tAxis, CBoxCollider* pA, CBoxCollider* pB, float& fMinOverlap, CMath::TVECTOR3& tMinAxis)
{
	// Get only the top four vertices, since the Y axis is not being checked
	CMath::TVECTOR3 tVerticesA[8];
	CMath::TVECTOR3 tVerticesB[8];

	CTransform* pcTransformA = pA->m_pcOwner->GetComponent<CTransform>();
	CTransform* pcTransformB = pB->m_pcOwner->GetComponent<CTransform>();

	CMath::TMATRIX tMatrixA = pcTransformA->GetMatrix();
	CMath::TMATRIX tMatrixB = pcTransformB->GetMatrix();

	CMath::TVECTOR3 tCenterA = pA->GetLocalCenter();
	CMath::TVECTOR3 tCenterB = pB->GetLocalCenter();

	CMath::TVECTOR3 tExtentA = pA->GetExtent();
	CMath::TVECTOR3 tExtentB = pB->GetExtent();

	tVerticesA[0] = CMath::Point3Transform(tExtentA, tMatrixA) + tCenterA;
	tVerticesA[1] = CMath::Point3Transform(CMath::TVECTOR3{  tExtentA.x,   tExtentA.y,  -tExtentA.z }, tMatrixA) + tCenterA;
	tVerticesA[2] = CMath::Point3Transform(CMath::TVECTOR3{  tExtentA.x,  -tExtentA.y,   tExtentA.z }, tMatrixA) + tCenterA;
	tVerticesA[3] = CMath::Point3Transform(CMath::TVECTOR3{  tExtentA.x,  -tExtentA.y,  -tExtentA.z }, tMatrixA) + tCenterA;
	tVerticesA[4] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentA.x,   tExtentA.y,   tExtentA.z }, tMatrixA) + tCenterA;
	tVerticesA[5] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentA.x,   tExtentA.y,  -tExtentA.z }, tMatrixA) + tCenterA;
	tVerticesA[6] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentA.x,  -tExtentA.y,   tExtentA.z }, tMatrixA) + tCenterA;
	tVerticesA[7] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentA.x,  -tExtentA.y,  -tExtentA.z }, tMatrixA) + tCenterA;

	tVerticesB[0] = CMath::Point3Transform(tExtentB, tMatrixB) + tCenterB;
	tVerticesB[1] = CMath::Point3Transform(CMath::TVECTOR3{ tExtentB.x,   tExtentB.y,  -tExtentB.z }, tMatrixB) + tCenterB;
	tVerticesB[2] = CMath::Point3Transform(CMath::TVECTOR3{ tExtentB.x,  -tExtentB.y,   tExtentB.z }, tMatrixB) + tCenterB;
	tVerticesB[3] = CMath::Point3Transform(CMath::TVECTOR3{ tExtentB.x,  -tExtentB.y,  -tExtentB.z }, tMatrixB) + tCenterB;
	tVerticesB[4] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentB.x,   tExtentB.y,   tExtentB.z }, tMatrixB) + tCenterB;
	tVerticesB[5] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentB.x,   tExtentB.y,  -tExtentB.z }, tMatrixB) + tCenterB;
	tVerticesB[6] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentB.x,  -tExtentB.y,   tExtentB.z }, tMatrixB) + tCenterB;
	tVerticesB[7] = CMath::Point3Transform(CMath::TVECTOR3{ -tExtentB.x,  -tExtentB.y,  -tExtentB.z }, tMatrixB) + tCenterB;

	// Get the min and max values between the projection of each vertex on the specified axis to check for a collision
	float IntervalA[2];
	float IntervalB[2];

	IntervalA[0] = IntervalA[1] = CMath::Vector3Dot(tAxis, tVerticesA[0]);
	IntervalB[0] = IntervalB[1] = CMath::Vector3Dot(tAxis, tVerticesB[0]);

	for (int i = 1; i < 8; i++)
	{
		float projectionA = CMath::Vector3Dot(tAxis, tVerticesA[i]);
		float projectionB = CMath::Vector3Dot(tAxis, tVerticesB[i]);

		IntervalA[0] = fminf(IntervalA[0], projectionA);
		IntervalA[1] = fmaxf(IntervalA[1], projectionA);

		IntervalB[0] = fminf(IntervalB[0], projectionB);
		IntervalB[1] = fmaxf(IntervalB[1], projectionB);
	}


	if (IntervalA[1] > IntervalB[0] && IntervalB[1] > IntervalA[0])
	{
		float fOverlap = fminf(IntervalA[1], IntervalB[1]) - std::fmaxf(IntervalA[0], IntervalB[0]);
		if (fOverlap < fMinOverlap)
		{
			fMinOverlap = fOverlap;
			tMinAxis = tAxis;
		}
		return true;
	}

	return false;
}

void CPhysicsManager::ResolveCollision(CBoxCollider * pcA, CBoxCollider * pcB, TCollision & tCollision)
{
	CTransform* pcTransforms[2];
	CRigidbody* pcRigidbodies[2];
	pcTransforms[0] = pcA->m_pcOwner->GetComponent<CTransform>();
	pcTransforms[1] = pcB->m_pcOwner->GetComponent<CTransform>();
	pcRigidbodies[0] = pcA->m_pcOwner->GetComponent<CRigidbody>();
	pcRigidbodies[1] = pcB->m_pcOwner->GetComponent<CRigidbody>();

	// No need to move second object
	if (!pcRigidbodies[1] || CMath::Vector3Magnitude(pcRigidbodies[1]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[0]->GetPosition();
		tPosition.x += tCollision.tAxis.x * tCollision.fOffset;
		//tPosition.y += tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z += tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[0]->SetPosition(tPosition);
	}

	// No need to move first object
	else if (!pcRigidbodies[0] || CMath::Vector3Magnitude(pcRigidbodies[0]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[1]->GetPosition();
		tPosition.x -= tCollision.tAxis.x * tCollision.fOffset;
		//tPosition.y -= tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z -= tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[1]->SetPosition(tPosition);
	}

	// Both objects are moving, so they need to be updated
	else
	{
		CMath::TVECTOR3 tVelocities[2] = { pcRigidbodies[0]->GetVelocity(), pcRigidbodies[1]->GetVelocity() };
		float fOverlap = tCollision.fOffset;

		float fSumX = fabsf(tVelocities[0].x) + fabsf(tVelocities[1].x);
		//float fSumY = fabsf(tVelocities[0].y) + fabsf(tVelocities[1].y);
		float fSumZ = fabsf(tVelocities[0].z) + fabsf(tVelocities[1].z);

		CMath::TVECTOR3 tPositions[2] = { pcTransforms[0]->GetPosition(), pcTransforms[1]->GetPosition() };

		if (fSumX > 0)
		{
			if ((tVelocities[0].x > 0 && tVelocities[1].x > 0) || (tVelocities[0].x < 0 && tVelocities[1].x < 0))
			{
				if (fabsf(tVelocities[0].x) > fabsf(tVelocities[1].x))
					tVelocities[0].x *= -1.0f;
				else
					tVelocities[1].x *= -1.0f;
			}

			tPositions[0].x += tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[0].x));
			tPositions[1].x -= tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[1].x));
		}

		/*if (fSumY > 0)
		{
			if ((tVelocities[0].y > 0 && tVelocities[1].y > 0) || (tVelocities[0].y < 0 && tVelocities[1].y < 0))
			{
				if (fabsf(tVelocities[0].y) > fabsf(tVelocities[1].y))
					tVelocities[0].y *= -1.0f;
				else
					tVelocities[1].y *= -1.0f;
			}

			tPositions[0].y += tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[0].y));
			tPositions[1].y -= tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[1].y));
		}*/

		if (fSumZ > 0)
		{
			if ((tVelocities[0].z > 0 && tVelocities[1].z > 0) || (tVelocities[0].z < 0 && tVelocities[1].z < 0))
			{
				if (fabsf(tVelocities[0].z) > fabsf(tVelocities[1].z))
					tVelocities[0].z *= -1.0f;
				else
					tVelocities[1].z *= -1.0f;
			}

			tPositions[0].z += tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[0].z));
			tPositions[1].z -= tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[1].z));
		}

		pcTransforms[0]->SetPosition(tPositions[0]);
		pcTransforms[1]->SetPosition(tPositions[1]);
	}
}
void CPhysicsManager::ResolveCollision(CCapsuleCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
	CTransform* pcTransforms[2];
	CRigidbody* pcRigidbodies[2];
	pcTransforms[0] = pcA->m_pcOwner->GetComponent<CTransform>();
	pcTransforms[1] = pcB->m_pcOwner->GetComponent<CTransform>();
	pcRigidbodies[0] = pcA->m_pcOwner->GetComponent<CRigidbody>();
	pcRigidbodies[1] = pcB->m_pcOwner->GetComponent<CRigidbody>();

	// No need to move second object
	if (!pcRigidbodies[1] || CMath::Vector3Magnitude(pcRigidbodies[1]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[0]->GetPosition();
		tPosition.x += tCollision.tAxis.x * tCollision.fOffset;
		//tPosition.y += tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z += tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[0]->SetPosition(tPosition);
	}

	// No need to move first object
	else if (!pcRigidbodies[0] || CMath::Vector3Magnitude(pcRigidbodies[0]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[1]->GetPosition();
		tPosition.x -= tCollision.tAxis.x * tCollision.fOffset;
		//tPosition.y -= tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z -= tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[1]->SetPosition(tPosition);
	}

	// Both objects are moving, so they need to be updated
	else
	{
		CMath::TVECTOR3 tVelocities[2] = { pcRigidbodies[0]->GetVelocity(), pcRigidbodies[1]->GetVelocity() };
		float fOverlap = tCollision.fOffset;

		float fSumX = fabsf(tVelocities[0].x) + fabsf(tVelocities[1].x);
		//float fSumY = fabsf(tVelocities[0].y) + fabsf(tVelocities[1].y);
		float fSumZ = fabsf(tVelocities[0].z) + fabsf(tVelocities[1].z);

		CMath::TVECTOR3 tPositions[2] = { pcTransforms[0]->GetPosition(), pcTransforms[1]->GetPosition() };

		if (fSumX > 0)
		{
			if ((tVelocities[0].x > 0 && tVelocities[1].x > 0) || (tVelocities[0].x < 0 && tVelocities[1].x < 0))
			{
				if (fabsf(tVelocities[0].x) > fabsf(tVelocities[1].x))
					tVelocities[0].x *= -1.0f;
				else
					tVelocities[1].x *= -1.0f;
			}

			tPositions[0].x += tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[0].x));
			tPositions[1].x -= tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[1].x));
		}

		/*if (fSumY > 0)
		{
			if ((tVelocities[0].y > 0 && tVelocities[1].y > 0) || (tVelocities[0].y < 0 && tVelocities[1].y < 0))
			{
				if (fabsf(tVelocities[0].y) > fabsf(tVelocities[1].y))
					tVelocities[0].y *= -1.0f;
				else
					tVelocities[1].y *= -1.0f;
			}

			tPositions[0].y += tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[0].y));
			tPositions[1].y -= tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[1].y));
		}*/

		if (fSumZ > 0)
		{
			if ((tVelocities[0].z > 0 && tVelocities[1].z > 0) || (tVelocities[0].z < 0 && tVelocities[1].z < 0))
			{
				if (fabsf(tVelocities[0].z) > fabsf(tVelocities[1].z))
					tVelocities[0].z *= -1.0f;
				else
					tVelocities[1].z *= -1.0f;
			}

			tPositions[0].z += tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[0].z));
			tPositions[1].z -= tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[1].z));
		}

		pcTransforms[0]->SetPosition(tPositions[0]);
		pcTransforms[1]->SetPosition(tPositions[1]);
	}
}
void CPhysicsManager::ResolveCollision(CBoxCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
	CTransform* pcTransforms[2];
	CRigidbody* pcRigidbodies[2];
	pcTransforms[0] = pcA->m_pcOwner->GetComponent<CTransform>();
	pcTransforms[1] = pcB->m_pcOwner->GetComponent<CTransform>();
	pcRigidbodies[0] = pcA->m_pcOwner->GetComponent<CRigidbody>();
	pcRigidbodies[1] = pcB->m_pcOwner->GetComponent<CRigidbody>();

	// No need to move second object
	if (!pcRigidbodies[1] || CMath::Vector3Magnitude(pcRigidbodies[1]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[0]->GetPosition();
		tPosition.x += tCollision.tAxis.x * tCollision.fOffset;
		//tPosition.y += tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z += tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[0]->SetPosition(tPosition);
	}

	// No need to move first object
	else if (!pcRigidbodies[0] || CMath::Vector3Magnitude(pcRigidbodies[0]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[1]->GetPosition();
		tPosition.x -= tCollision.tAxis.x * tCollision.fOffset;
		//tPosition.y -= tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z -= tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[1]->SetPosition(tPosition);
	}

	// Both objects are moving, so they need to be updated
	else
	{
		CMath::TVECTOR3 tVelocities[2] = { pcRigidbodies[0]->GetVelocity(), pcRigidbodies[1]->GetVelocity() };
		float fOverlap = tCollision.fOffset;

		float fSumX = fabsf(tVelocities[0].x) + fabsf(tVelocities[1].x);
		//float fSumY = fabsf(tVelocities[0].y) + fabsf(tVelocities[1].y);
		float fSumZ = fabsf(tVelocities[0].z) + fabsf(tVelocities[1].z);

		CMath::TVECTOR3 tPositions[2] = { pcTransforms[0]->GetPosition(), pcTransforms[1]->GetPosition() };

		if (fSumX > 0)
		{
			if ((tVelocities[0].x > 0 && tVelocities[1].x > 0) || (tVelocities[0].x < 0 && tVelocities[1].x < 0))
			{
				if (fabsf(tVelocities[0].x) > fabsf(tVelocities[1].x))
					tVelocities[0].x *= -1.0f;
				else
					tVelocities[1].x *= -1.0f;
			}

			tPositions[0].x += tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[0].x));
			tPositions[1].x -= tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[1].x));
		}

		/*if (fSumY > 0)
		{
			if ((tVelocities[0].y > 0 && tVelocities[1].y > 0) || (tVelocities[0].y < 0 && tVelocities[1].y < 0))
			{
				if (fabsf(tVelocities[0].y) > fabsf(tVelocities[1].y))
					tVelocities[0].y *= -1.0f;
				else
					tVelocities[1].y *= -1.0f;
			}

			tPositions[0].y += tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[0].y));
			tPositions[1].y -= tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[1].y));
		}*/

		if (fSumZ > 0)
		{
			if ((tVelocities[0].z > 0 && tVelocities[1].z > 0) || (tVelocities[0].z < 0 && tVelocities[1].z < 0))
			{
				if (fabsf(tVelocities[0].z) > fabsf(tVelocities[1].z))
					tVelocities[0].z *= -1.0f;
				else
					tVelocities[1].z *= -1.0f;
			}

			tPositions[0].z += tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[0].z));
			tPositions[1].z -= tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[1].z));
		}

		pcTransforms[0]->SetPosition(tPositions[0]);
		pcTransforms[1]->SetPosition(tPositions[1]);
	}
}

bool CPhysicsManager::IsNewCollision(ICollider * pcA, ICollider * pcB)
{
	// If there are no registered collisions in either collider, the collision is new
	if (m_cCollisions.count(pcA) == 0 || m_cCollisions.count(pcB) == 0)
		return true;

	for (ICollider* collider : m_cCollisions[pcA])
	{
		// Collider B was found in collision vector of A, so the collision existed before
		if (collider == pcB)
		{
			return false;
		}
	}

	// Collider B wasn't found, so the collision is new
	return true;
}
bool CPhysicsManager::IsCollisionExit(ICollider * pcA, ICollider * pcB, int& nIndexA, int& nIndexB)
{
	// If there are no registered collisions in either collider, there was no collision before
	if (m_cCollisions.count(pcA) == 0 || m_cCollisions.count(pcB) == 0)
		return false;

	int nSizeA = (int)m_cCollisions[pcA].size();
	int nSizeB = (int)m_cCollisions[pcB].size();

	// If there are no empty lists
	if (nSizeA != 0 && nSizeB != 0)
	{
		int nLargestSize = nSizeA > nSizeB ? nSizeA : nSizeB;

		bool bFoundA = false;
		bool bFoundB = false;

		for (int i = 0; i < nLargestSize; i++)
		{
			// A full list of collisions has been searched and the collider wasn't found, so there was no collision before
			if ((i == nSizeA && !bFoundA) || (i == nSizeB && !bFoundB))
				return false;

			// Collision was found but is not colliding now, so the collision just ended
			if (!bFoundA && m_cCollisions[pcA][i] == pcB)
			{
				bFoundA = true;
				nIndexA = i;
			}

			// Collision was found but is not colliding now, so the collision just ended
			if (!bFoundB && m_cCollisions[pcB][i] == pcA)
			{
				bFoundB = true;
				nIndexB = i;
			}
		}

		// Collision was found
		if (bFoundA && bFoundB)
			return true;
	}

	// Collision wasn't found
	return false;
}

float CPhysicsManager::Clamp(float fValue, float fMin, float fMax)
{
	if (fValue < fMin) return fMin;
	if (fValue > fMax) return fMax;
	return fValue;
}
float CPhysicsManager::GetRatio(float fValue, float fMin, float fMax)
{
	return (fValue - fMin) / (fMax - fMin);
}

void CPhysicsManager::ClosestPtSegmentSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3 & tPs, CMath::TVECTOR3 & tQt)
{
	// General equation for the closest points in a line is: 
	// P(s) = P0 + s * u where P0 = starting point of line 1, u = direction vector of line 1, s = scalar
	// Q(t) = Q0 + t * v where Q0 = starting point of line 2, v = direction vector of line 2, t = scalar
	// s = (be - cd) / (ac - b^2)
	// t = (ae-bd) / (ac - b^2)

	CMath::TVECTOR3 u = tP1 - tP0;
	CMath::TVECTOR3 v = tQ1 - tQ0;

	float a = CMath::Vector3Dot(u, u);
	float b = CMath::Vector3Dot(u, v);
	float c = CMath::Vector3Dot(v, v);
	float d = CMath::Vector3Dot(u, CMath::TVECTOR3(tP0 - tQ0));
	float e = CMath::Vector3Dot(v, CMath::TVECTOR3(tP0 - tQ0));

	float s = 0;
	float t = 0;
	float denominator = (a*c - b * b);

	// If denominator is not zero, calculate closest point scalar s on Line 1 to Line 2
	// Clamp s to make sure it's within the segment limits (original formula assumes infinite line)
	if (denominator != 0.0f)
	{
		s = Clamp((b*e - c * d) / denominator, 0.0f, 1.0f);
	}

	// Calculate closest point scalar t on Line 2 to P(s)
	// t = Dot((P0 + u*s) - Q0, v) / Dot(v,v) = (b*s + e) / c
	if (c != 0)
	{
		t = (b*s + e) / c;
	}

	// If t is within 0 and 1 (Q(t) is within the segment) then finish
	// Else, clamp t and recompute to find s
	// s = Dot((Q0 + v*t) - P0, u) / Dot(u,u) = (b*t - d) / a
	if (t < 0.0f)
	{
		t = 0.0f;
		s = Clamp(-d / a, 0.0f, 1.0f);
	}

	else if (t > 1.0f)
	{
		t = 1.0f;
		s = Clamp((b - d) / a, 0.0f, 1.0f);
	}

	tPs = tP0 + u * s;
	tQt = tQ0 + v * t;
}
void CPhysicsManager::ClosestPtLineSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tDirection, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3 & tPs, CMath::TVECTOR3 & tQt)
{
	CMath::TVECTOR3 u = CMath::TVECTOR3(tDirection);
	CMath::TVECTOR3 v = tQ1 - tQ0;

	float a = CMath::Vector3Dot(u, u);
	float b = CMath::Vector3Dot(u, v);
	float c = CMath::Vector3Dot(v, v);
	float d = CMath::Vector3Dot(u, CMath::TVECTOR3(tP0 - tQ0));
	float e = CMath::Vector3Dot(v, CMath::TVECTOR3(tP0 - tQ0));

	float s = 0;
	float t = 0;
	float denominator = (a*c - b * b);

	if (denominator != 0.0f)
	{
		s = (b*e - c * d) / denominator;
	}

	t = Clamp((b*s + e) / c, 0.0f, 1.0f);

	tPs = tP0 + u * s;
	tQt = tQ0 + v * t;
}
void CPhysicsManager::ClosestPtPointOBB(CMath::TVECTOR3 tPoint, CBoxCollider * pcB, CMath::TVECTOR3 & tResult)
{
	CTransform* tTransform = pcB->m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tCenter = tTransform->GetPosition() + pcB->GetLocalCenter();

	CMath::TVECTOR3 tMinAxis = tPoint - tCenter;

	tResult = tCenter;

	const int nAxisCount = 3;
	CMath::TVECTOR3 tLocalAxes[nAxisCount];

	CMath::TMATRIX tTransformMatrix = tTransform->GetMatrix();

	tLocalAxes[0] = CMath::Vector3Normalize(CMath::TVECTOR3(tTransformMatrix.r[0].x, tTransformMatrix.r[0].y, tTransformMatrix.r[0].z));
	tLocalAxes[1] = CMath::Vector3Normalize(CMath::TVECTOR3(tTransformMatrix.r[1].x, tTransformMatrix.r[1].y, tTransformMatrix.r[1].z));
	tLocalAxes[2] = CMath::Vector3Normalize(CMath::TVECTOR3(tTransformMatrix.r[2].x, tTransformMatrix.r[2].y, tTransformMatrix.r[2].z));

	CMath::TMATRIX tRotationMatrix = tTransformMatrix;
	tRotationMatrix.r[3].x = 0;
	tRotationMatrix.r[3].y = 0;
	tRotationMatrix.r[3].z = 0;
	tRotationMatrix.r[3].w = 1;

	CMath::TVECTOR3 tExtent = pcB->GetExtent() * tTransform->GetScale();

	for (int i = 0; i < nAxisCount; i++)
	{
		float fDistance = CMath::Vector3Dot(tMinAxis, tLocalAxes[i]);

			if (fDistance > tExtent.mData[i])
				fDistance = tExtent.mData[i];
			if (fDistance < -tExtent.mData[i])
				fDistance = -tExtent.mData[i];

		tResult = tResult + tLocalAxes[i] * fDistance;
	}
}
void CPhysicsManager::ClosestPtPointSegment(CMath::TVECTOR3 tPoint, CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3 & tResult)
{
	CMath::TVECTOR3 tSegment = tP1 - tP0;
	float fScalar = CMath::Vector3Dot(tPoint - tP0, tSegment) / CMath::Vector3Dot(tSegment, tSegment);

	fScalar = Clamp(fScalar, 0.0f, 1.0f);

	tResult = tP0 + tSegment * fScalar;
}

int CPhysicsManager::FindGridPositionX(CMath::TVECTOR3 tPosition, int nMin, int nMax)
{
	const int nCellWidth = CELL_SIZE;
	const int nCellCount = ROOM_WIDTH / nCellWidth;

	int nMid = (int)roundf(((float)(nMin + nMax) * 0.5f));

	if (tPosition.x < m_tRoomPosition.x + (nMid - (float)nCellCount * 0.5f) * nCellWidth)
	{
		if (nMid == nMin)
			return nMid;

		if (nMid == nMax)
			return nMid - 1;

		return FindGridPositionX(tPosition, nMin, nMid);
	}

	else
	{
		if (nMid == nMin)
			return nMid + 1;

		if (nMid == nMax)
			return nMid;
		return FindGridPositionX(tPosition, nMid, nMax);
	}

}
int CPhysicsManager::FindGridPositionZ(CMath::TVECTOR3 tPosition, int nMin, int nMax)
{
	const int nCellHeight = CELL_SIZE;
	const int nCellCount = ROOM_WIDTH / nCellHeight;

	int nMid = (int)roundf(((float)(nMin + nMax) / 2.0f));

	if (tPosition.z < m_tRoomPosition.z + (nMid - (float)nCellCount / 2.0f) * nCellHeight)
	{
		if (nMid == nMin)
			return nMid;

		if (nMid == nMax)
			return nMid - 1;

		return FindGridPositionZ(tPosition, nMin, nMid);
	}

	else
	{
		if (nMid == nMin)
			return nMid + 1;

		if (nMid == nMax)
			return nMid;
		return FindGridPositionZ(tPosition, nMid, nMax);
	}
}

void CPhysicsManager::GetMinMaxOverlap(CBoxCollider* collider, CMath::TVECTOR2& tOverlapX, CMath::TVECTOR2& tOverlapY)
{
	CTransform* pcTransform = collider->m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tPosition = pcTransform->GetPosition() + collider->GetLocalCenter();

	const int COLUMNS = ROOM_WIDTH / CELL_SIZE;
	const int ROWS = ROOM_HEIGHT / CELL_SIZE;

	CMath::TMATRIX tOrientation = pcTransform->GetMatrix();
	tOrientation._41 = tOrientation._42 = tOrientation._43 = 0.0f;

	CMath::TVECTOR3 tExtent = collider->GetExtent() * pcTransform->GetScale();

	const float OFFSET = 0.2f;
	float fExtentX = CMath::Vector3Dot(CMath::TVECTOR3(1, 0, 0), CMath::Vector3Transform(tExtent, tOrientation)) + OFFSET;
	float fExtentZ = CMath::Vector3Dot(CMath::TVECTOR3(0, 0, 1), CMath::Vector3Transform(tExtent, tOrientation)) + OFFSET;

	tOverlapX.x = (float)FindGridPositionX(tPosition - CMath::TVECTOR3(1, 0, 0) * fabsf(fExtentX), 0, COLUMNS - 1);
	tOverlapX.y = (float)FindGridPositionX(tPosition + CMath::TVECTOR3(1, 0, 0) * fabs(fExtentX), 0, COLUMNS - 1);

	tOverlapY.x = (float)FindGridPositionZ(tPosition - CMath::TVECTOR3(0, 0, 1) * fabsf(fExtentZ), 0, ROWS - 1);
	tOverlapY.y = (float)FindGridPositionZ(tPosition + CMath::TVECTOR3(0, 0, 1) * fabs(fExtentZ), 0, ROWS - 1);

	if (tOverlapX.x > tOverlapX.y)
	{
		float temp = tOverlapX.x;
		tOverlapX.x = tOverlapX.y;
		tOverlapX.y = temp;
	}

	if (tOverlapY.x > tOverlapY.y)
	{
		float temp = tOverlapY.x;
		tOverlapY.x = tOverlapY.y;
		tOverlapY.y = temp;
	}
}
void CPhysicsManager::GetMinMaxOverlap(CCapsuleCollider* collider, CMath::TVECTOR2& tOverlapX, CMath::TVECTOR2& tOverlapY)
{
	CTransform* pcTransform = collider->m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tPosition = pcTransform->GetPosition() + collider->GetCenter();

	const int COLUMNS = ROOM_WIDTH / CELL_SIZE;
	const int ROWS = ROOM_HEIGHT / CELL_SIZE;

	const float OFFSET = 0.2f;
	float fExtent = collider->GetRadius() * pcTransform->GetScale().x + OFFSET;

	tOverlapX.x = (float)FindGridPositionX(tPosition - CMath::TVECTOR3(1, 0, 0) * fExtent, 0, COLUMNS - 1);
	tOverlapX.y = (float)FindGridPositionX(tPosition + CMath::TVECTOR3(1, 0, 0) * fExtent, 0, COLUMNS - 1);

	tOverlapY.x = (float)FindGridPositionZ(tPosition - CMath::TVECTOR3(0, 0, 1) * fExtent, 0, ROWS - 1);
	tOverlapY.y = (float)FindGridPositionZ(tPosition + CMath::TVECTOR3(0, 0, 1) * fExtent, 0, ROWS - 1);

	if (tOverlapX.x > tOverlapX.y)
	{
		float temp = tOverlapX.x;
		tOverlapX.x = tOverlapX.y;
		tOverlapX.y = temp;
	}

	if (tOverlapY.x > tOverlapY.y)
	{
		float temp = tOverlapY.x;
		tOverlapY.x = tOverlapY.y;
		tOverlapY.y = temp;
	}
}

void CPhysicsManager::SetStaticGridPositions(ICollider* collider, CMath::TVECTOR2 tOverlapX, CMath::TVECTOR2 tOverlapZ)
{
	for (int i = (int)tOverlapX.x; i <= (int)tOverlapX.y; i++)
	{
		for (int j = (int)tOverlapZ.x; j <= (int)tOverlapZ.y; j++)
		{
			m_StaticGridPositions[collider].push_back(TCoordinate(i, j));
		}
	}
}
void CPhysicsManager::SetDynamicGridPositions(ICollider* collider, CMath::TVECTOR2 tOverlapX, CMath::TVECTOR2 tOverlapZ)
{
	for (int i = (int)tOverlapX.x; i <= (int)tOverlapX.y; i++)
	{
		for (int j = (int)tOverlapZ.x; j <= (int)tOverlapZ.y; j++)
		{
			m_DynamicGridPositions[collider].push_back(TCoordinate(i, j));
		}
	}
}

bool CPhysicsManager::IsCollidingRayBox(CBoxCollider * pBoxCollider, CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tNormDirection, float fMaxDistance, CMath::TVECTOR3 & tCollisionPoint, CMath::TVECTOR3& tCollisionExit)
{
	/*  The formula to find the point in which a ray is at a specific time t is:
		Point p = ray.origin + ray.direction * t;

		This can be expanded into:
		point.x = ray.origin.x + ray.direction.x * tx
		point.y = ray.origin.y + ray.direction.y * ty
		point.z = ray.origin.z + ray.direction.z * tz

		Since we need to find t along the ray, we solve for t:
		tx = (point.x - ray.origin.x) / ray.direction.x
		ty = (point.y - ray.origin.y) / ray.direction.y
		tz = (point.z - ray.origin.z) / ray.direction.z

		Basically we need to find the time t in which the ray enters the limits of each axis and the time it exits:
		MinTx = (MinX - ray.origin.x) / ray.direction.x;
		MaxTx = (MaxX - ray.origin.x) / ray.direction.x;
		MinTy = (MinY - ray.origin.y) / ray.direction.y;
		MaxTy = (MaxY - ray.origin.y) / ray.direction.y;
		MinTz = (MinZ - ray.origin.z) / ray.direction.z;
		MaxTz = (MaxZ - ray.origin.z) / ray.direction.z;

		If the ray has entered the limits of all axis before it has exited any of them, then there is an intersection.

		The formula would be complete for an AABB, but since we're working with OOBs, we need to find to project the values to the local axis of the OOB.
		*/

	CTransform* pcTransform = pBoxCollider->m_pcOwner->GetComponent<CTransform>();
	CMath::TMATRIX tMatrix = pcTransform->GetMatrix();

	CMath::TVECTOR3 tCenterPosition = pcTransform->GetPosition() + pBoxCollider->GetLocalCenter();
	CMath::TVECTOR3 tExtents = pBoxCollider->GetExtent() * pcTransform->GetScale();

	// Get the local axes of the OOB based on the transform of the entity
	const int nAxesCount = 3;
	CMath::TVECTOR3 tLocalAxes[nAxesCount];
	tLocalAxes[0] = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrix.r[0].x, tMatrix.r[0].y, tMatrix.r[0].z));
	tLocalAxes[1] = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrix.r[1].x, tMatrix.r[1].y, tMatrix.r[1].z));
	tLocalAxes[2] = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrix.r[2].x, tMatrix.r[2].y, tMatrix.r[2].z));

	// The vector from the origin of the ray to the center of the OOB
	CMath::TVECTOR3 tOriginToCenter = tCenterPosition - tOrigin;

	// The dot product of the normalized direction of the ray and the local axes gives the angle between them
	CMath::TVECTOR3 tAngleDirectionToLocalAxes = CMath::TVECTOR3(
		CMath::Vector3Dot(tLocalAxes[0], tNormDirection),
		CMath::Vector3Dot(tLocalAxes[1], tNormDirection),
		CMath::Vector3Dot(tLocalAxes[2], tNormDirection));

	// The dot product of the local axes and the vector from origin of the ray to center of OOB allows us to fin the min and max limit of the OOB
	CMath::TVECTOR3 tProjectedOriginToCenter = CMath::TVECTOR3(
		CMath::Vector3Dot(tLocalAxes[0], tOriginToCenter),
		CMath::Vector3Dot(tLocalAxes[1], tOriginToCenter),
		CMath::Vector3Dot(tLocalAxes[2], tOriginToCenter));

	float fTime[6] = { 0,0,0,0,0,0 };
	for (int i = 0; i < nAxesCount; i++)
	{
		if (tAngleDirectionToLocalAxes.mData[i] == 0)
			tAngleDirectionToLocalAxes.mData[i] = 0.0000001f;

		fTime[i * 2] = (tProjectedOriginToCenter.mData[i] - tExtents.mData[i]) / tAngleDirectionToLocalAxes.mData[i];
		fTime[i * 2 + 1] = (tProjectedOriginToCenter.mData[i] + tExtents.mData[i]) / tAngleDirectionToLocalAxes.mData[i];
	}

	//Get the greatest of the entry points
	float fGreatestMin = fmaxf(fmaxf(fminf(fTime[0], fTime[1]), fminf(fTime[2], fTime[3])), fminf(fTime[4], fTime[5]));

	// Get the smallest of the exit points
	float fSmallestMax = fminf(fminf(fmaxf(fTime[0], fTime[1]), fmaxf(fTime[2], fTime[3])), fmaxf(fTime[4], fTime[5]));

	if (fSmallestMax < 0.0f)
		return false;

	if (fGreatestMin > fSmallestMax)
		return false;

	float fEntryDistance = fGreatestMin < 0.0f ? fSmallestMax : fGreatestMin;

	if (fEntryDistance > fMaxDistance)
		return false;

	tCollisionPoint = tOrigin + tNormDirection * fEntryDistance;
	tCollisionExit = tOrigin + tNormDirection * fSmallestMax;

	return true;

}
bool CPhysicsManager::IsCollidingRayCapsule(CCapsuleCollider * pCapCollider, CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tNormDirection, float fMaxDistance, CMath::TVECTOR3 & tCollisionEntry, CMath::TVECTOR3& tCollisionExit)
{
	CMath::TVECTOR3 tCapsuleBottom, tCapsuleTop;
	pCapCollider->GetMiddleSegment(OUT tCapsuleBottom, OUT tCapsuleTop);

	// THIS CODE IS NOT ACCURATE, ASSUMES SPHERE FROM INACCURATE APROXIMATION

	CMath::TVECTOR3 tClosestPointRay, tSphereCenter;
	ClosestPtSegmentSegment(tOrigin, tOrigin + tNormDirection * fMaxDistance, tCapsuleBottom, tCapsuleTop, OUT tClosestPointRay, OUT tSphereCenter);

	float radius = pCapCollider->GetRadius() * pCapCollider->m_pcOwner->GetComponent<CTransform>()->GetScale().x;

	CMath::TVECTOR3 tE = tSphereCenter - tOrigin;

	float fA = CMath::Vector3Dot(tNormDirection, tE);

	float fEMagSq = CMath::Vector3Dot(tE, tE);

	float fBSq = fEMagSq - fA * fA;
	
	if (fBSq > radius * radius)
		return false;

	float fFSq = radius * radius - fBSq;

	float fF = sqrtf(fFSq);

	float t = fA - fF;

	if (t < 0.0f)
		return false;

	tCollisionEntry = tOrigin + tNormDirection * t;
	tCollisionExit = tOrigin + tNormDirection * (t + 2 * fF);

	return true;
}

void CEntityMask::Add(int nEntityType)
{
	nValue |= 1 << nEntityType;
}
void CEntityMask::Remove(int nEntityType)
{
	nValue ^= 1 << nEntityType;
}
void CEntityMask::Invert()
{
	nValue = ~nValue;
}