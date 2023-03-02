#include "PathAgent.h"
#include "EnumTypes.h"
#include "Transform.h"
#include "Entity.h"
#include "Rigidbody.h"
#include "Stats.h"
#include "PhysicsManager.h"
#include "Time.h"

typedef CMath::TVECTOR2 Vector2;
typedef CMath::TVECTOR3 Vector3;

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

CPathAgent::CPathAgent(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::PATH_AGENT;
	m_pcSurface = nullptr;
	m_ptStart = nullptr;
	m_ptTarget = nullptr;
	m_ptCurrent = nullptr;
	m_bTargetChanged = false;
	m_pcTargetTransform = nullptr;

	m_pcTransform = m_pcOwner->GetComponent<CTransform>();
	m_pcRigidbody = m_pcOwner->GetComponent<CRigidbody>();
	m_pcStats = m_pcOwner->GetComponent<CStats>();

	m_fRaycastTimer = 0.2f;
	m_fMaxRaycastTimer = 0.2f;
	m_bCanWalkStraight = false;

	m_bNavigate = false;
}
CPathAgent::~CPathAgent() 
{
	ClearVisited();
}

CPathSurface* CPathAgent::GetSurface()
{
	return m_pcSurface;
}

void CPathAgent::SetTarget(CMath::TVECTOR3 tNewTarget)
{
	m_pcTargetTransform = nullptr;
	m_tTargetPosition = tNewTarget;
	CalculateTargetNode(m_tTargetPosition);
}
void CPathAgent::SetTarget(CTransform* pcTransform)
{
	m_pcTargetTransform = pcTransform;
}
void CPathAgent::SetSurface(CPathSurface* pcNewSurface)
{
	m_pcSurface = pcNewSurface;
}

void CPathAgent::CalculatePath()
{
	if (!m_ptTarget)
		return;

	ClearVisited();

	int nRows = m_pcSurface->GetRows();
	int nColumns = m_pcSurface->GetColumns();

	Vector3 tStartPosition = m_pcTransform->GetPosition();
	m_ptStart = GetStartNode(tStartPosition, Vector2(0, 0), Vector2((float)(nColumns - 1), (float)(nRows - 1)));

	TPlannerNode* ptBegin = new TPlannerNode();
	ptBegin->m_ptParent = nullptr;
	ptBegin->m_ptSearchNode = m_ptStart;
	ptBegin->m_fHeuristicCost = ptBegin->m_fGivenCost = ptBegin->m_fFinalCost = 0.0f;

	m_tQueue.push(ptBegin);

	m_tVisited[m_ptStart] = ptBegin;

	while (!m_tQueue.empty())
	{
		TPlannerNode* current = m_tQueue.top();
		m_tQueue.pop();

		TSearchNode* search = current->m_ptSearchNode;

		if (search == m_ptTarget)
		{
			m_tQueue.clear();
			ClearPath();
			SetPath(current);
			return;
		}

		for (TSearchNode* neighbor : search->m_ptNeighbors)
		{
			if (neighbor->m_fWeight == 0.0f)
				continue;

			float fNewGivenCost = current->m_fGivenCost + GetDistance(search, neighbor);

			if (m_tVisited.count(neighbor) > 0)
			{
				TPlannerNode* neighborPlanner = m_tVisited[neighbor];
				if (fNewGivenCost < neighborPlanner->m_fGivenCost)
				{
					m_tQueue.remove(neighborPlanner);
					neighborPlanner->m_fGivenCost = fNewGivenCost;
					neighborPlanner->m_fFinalCost = fNewGivenCost + neighborPlanner->m_fHeuristicCost;
					neighborPlanner->m_ptParent = current;
					m_tQueue.push(neighborPlanner);
				}

			}

			else // if the node hasn't been visited before
			{
				TPlannerNode* neighborPlanner = new TPlannerNode();
				neighborPlanner->m_ptParent = current;
				neighborPlanner->m_ptSearchNode = neighbor;
				neighborPlanner->m_fGivenCost = fNewGivenCost;
				neighborPlanner->m_fHeuristicCost = CalculateHeuristic(neighbor);
				neighborPlanner->m_fFinalCost = neighborPlanner->m_fHeuristicCost + fNewGivenCost;

				m_tQueue.push(neighborPlanner);

				m_tVisited[neighbor] = neighborPlanner;
			}
		}
	}
}
void CPathAgent::Navigate()
{
	Vector3 tPosition = m_pcTransform->GetPosition();

	if (m_pcTargetTransform)
	{
		m_tTargetPosition = m_pcTargetTransform->GetPosition();
		CalculateTargetNode(m_tTargetPosition);
	}

	m_fRaycastTimer += CTime::GetDeltaF();
	if (m_fRaycastTimer >= m_fMaxRaycastTimer)
	{
		m_bCanWalkStraight = IsTargetInSight();
		m_fRaycastTimer = 0.0f;
	}

	if (m_bCanWalkStraight)
	{
		m_ptCurrent = nullptr;

		if (CMath::Vector3Magnitude(m_tTargetPosition - tPosition) < 2.0f)
		{
			Stop();
			m_pcRigidbody->SetVelocity({ 0, 0, 0 });
			return;
		}

		Vector3 direction = CMath::Vector3Normalize(m_tTargetPosition - tPosition);
		Vector3 velocity = direction * m_pcStats->GetMovementSpeed();
		m_pcRigidbody->SetVelocity({ velocity.x, 0, velocity.z });
	}

	else
	{
		CalculatePath();
		
		// If there is no path, stop
		if (m_tFinalPath.empty())
		{
			m_ptCurrent = nullptr;
			return;
		}

		// set a pointer the current node if there isn't one
		if (!m_ptCurrent)
		{
			m_ptCurrent = m_tFinalPathDebug.back();
			m_tNextPosition = m_tFinalPath.back();
		}

		// if agent is close enough to next position, move to the next one after that
		if (CMath::Vector3Magnitude(m_tNextPosition - tPosition) < 2.0f)
		{
			m_tFinalPathDebug.erase(--m_tFinalPathDebug.end());
			m_tFinalPath.erase(--m_tFinalPath.end());

			if (m_tFinalPath.empty())
			{
				Stop();
				m_tFinalPathDebug.clear();
				m_tFinalPath.clear();
				m_ptCurrent->m_tColor = Vector4(1, 1, 1, 1);
				m_ptCurrent = nullptr;
				m_pcRigidbody->SetVelocity({ 0,0,0 });
				return;
			}

			m_ptCurrent->m_tColor = Vector4(1, 1, 1, 1);
			m_ptCurrent = m_tFinalPathDebug.back();
			m_tNextPosition = m_tFinalPath.back();
		}

		Vector3 direction = CMath::Vector3Normalize(m_tNextPosition - CMath::TVECTOR3(0, tPosition.y, 0) - tPosition);
		Vector3 velocity = direction * m_pcStats->GetMovementSpeed();

		m_pcRigidbody->SetVelocity(velocity);
	}

}

TSearchNode* CPathAgent::GetStartNode(Vector3 tStartPosition, Vector2 tMin, Vector2 tMax) const
{
	int nMinX = (int)tMin.x;
	int nMinZ = (int)tMin.y;

	int nMaxX = (int)tMax.x;
	int nMaxZ = (int)tMax.y;

	int x = GetCoordinateX(tStartPosition.x, nMinX, nMaxX);
	int z = GetCoordinateZ(tStartPosition.z, nMinZ, nMaxZ);

	TSearchNode* ptResult = m_pcSurface->GetNodeAtIndex(x, z);
	return ptResult;
}
TSearchNode* CPathAgent::GetTargetNode(Vector3 tTargetPosition, Vector2 tMin, Vector2 tMax) const
{
	int nMinX = (int)tMin.x;
	int nMinZ = (int)tMin.y;

	int nMaxX = (int)tMax.x;
	int nMaxZ = (int)tMax.y;

	int x = GetCoordinateX(tTargetPosition.x, nMinX, nMaxX);
	int z = GetCoordinateZ(tTargetPosition.z, nMinZ, nMaxZ);

	TSearchNode* ptResult = m_pcSurface->GetNodeAtIndex(x, z);
	return ptResult;
}

void CPathAgent::Start()
{
	m_bNavigate = true;
}
void CPathAgent::Stop()
{
	m_bNavigate = false;
}

bool CPathAgent::IsNavigating() const
{
	return m_bNavigate;
}

int CPathAgent::GetCoordinateX(float fStartPosition, int nMin, int nMax) const
{
	float fHalfDistance = m_pcSurface->GetNodeDistance() * 0.5f;

	int nMidX = (int)roundf((float)(nMax + nMin) * 0.5f);

	if (nMidX == nMin)
		return nMin;

	if (nMidX == nMax)
		return nMax;

	TSearchNode* ptCurrent = m_pcSurface->GetNodeAtIndex(nMidX, 0);
	float fPosition = ptCurrent->m_tPosition.x;

	if (fPosition > fStartPosition - fHalfDistance && fPosition < fStartPosition + fHalfDistance)
		return nMidX;

	if (fStartPosition < fPosition)
		return GetCoordinateX(fStartPosition, nMin, nMidX);

	// else if fStartPosition > fPosition
	return GetCoordinateX(fStartPosition, nMidX, nMax);
}
int CPathAgent::GetCoordinateZ(float fStartPosition, int nMin, int nMax) const
{
	float fHalfDistance = m_pcSurface->GetNodeDistance() * 0.5f;

	int nMidZ = (int)roundf((float)(nMax + nMin) * 0.5f);

	if (nMidZ == nMin)
		return nMin;

	if (nMidZ == nMax)
		return nMax;

	TSearchNode* ptCurrent = m_pcSurface->GetNodeAtIndex(0, nMidZ);
	float fPosition = ptCurrent->m_tPosition.z;

	if (fPosition > fStartPosition - fHalfDistance && fPosition < fStartPosition + fHalfDistance)
		return nMidZ;

	if (fStartPosition < fPosition)
		return GetCoordinateZ(fStartPosition, nMidZ, nMax);

	// else if fStartPosition > fPosition
	return GetCoordinateZ(fStartPosition, nMin, nMidZ);
}
float CPathAgent::CalculateHeuristic(TSearchNode* current) const
{
	return CMath::Vector3Magnitude(m_ptTarget->m_tPosition - current->m_tPosition);
}

void CPathAgent::ClearVisited()
{
	for (auto pair : m_tVisited)
	{
		if (pair.second)
		{
			delete pair.second;
			pair.second = nullptr;
		}
	}
	m_tVisited.clear();
}
void CPathAgent::ClearPath()
{
	for (int i = 1; i < m_tFinalPathDebug.size(); i++)
	{
		m_tFinalPathDebug[i]->m_tColor = Vector4(1, 1, 1, 1);
	}
	m_tFinalPathDebug.clear();
	m_tFinalPath.clear();
}
void CPathAgent::SetPath(TPlannerNode* pTargetNode)
{
	TPlannerNode* pathNode = pTargetNode;
	while (pathNode)
	{
		if (pathNode->m_ptParent != nullptr)
		{
			if (pathNode->m_ptSearchNode != m_ptTarget)
			{
				pathNode->m_ptSearchNode->m_tColor = Vector4(0, 1, 0, 1);
				m_tFinalPath.push_back(pathNode->m_ptSearchNode->m_tPosition);
			}

			else
			{
				m_tFinalPath.push_back(m_tTargetPosition);

				if(CMath::Vector3Magnitude(m_pcOwner->GetComponent<CTransform>()->GetPosition() - m_tTargetPosition) >
					CMath::Vector3Magnitude(m_pcOwner->GetComponent<CTransform>()->GetPosition() - m_ptTarget->m_tPosition))
				{
					m_tFinalPath.push_back(m_ptTarget->m_tPosition);
					m_tFinalPathDebug.push_back(pathNode->m_ptSearchNode);
				}
			}

			m_tFinalPathDebug.push_back(pathNode->m_ptSearchNode);
		}

		TSearchNode* temp = pathNode->m_ptSearchNode;
		pathNode = pathNode->m_ptParent;

		delete m_tVisited[temp];
		m_tVisited[temp] = nullptr;
		m_tVisited.erase(temp);
	}
}

void CPathAgent::CalculateTargetNode(Vector3 tPosition)
{
	if (m_ptTarget)
	{
		m_ptTarget->m_tColor = Vector4(1, 1, 1, 1);
	}
	int nRows = m_pcSurface->GetRows();
	int nColumns = m_pcSurface->GetColumns();

	m_ptTarget = GetTargetNode(tPosition, Vector2(0, 0), Vector2((float)nColumns - 1, (float)nRows - 1));
	m_ptTarget->m_tColor = Vector4(1, 0, 0, 1);
	m_bTargetChanged = true;
}

float CPathAgent::GetDistance(TSearchNode * tNodeA, TSearchNode * tNodeB)
{
	return CMath::Vector3Magnitude(tNodeA->m_tPosition - tNodeB->m_tPosition);
}

bool CPathAgent::IsTargetInSight() const
{

	Vector3 tOrigin = m_pcOwner->GetComponent<CTransform>()->GetPosition() + Vector3(0, 2, 0);
	Vector3 tTarget = m_tTargetPosition;
	tTarget += Vector3(0, 2, 0);
	Vector3 tScaledVec = tTarget - tOrigin;
	Vector3 tDirection = CMath::Vector3Normalize(tScaledVec);
	float fDistance = CMath::Vector3Magnitude(tScaledVec);
	CEntityMask mask;
	mask.Add(eEntity::ENVIRONMENT);

	TRayHit hit;
	return !CPhysicsManager::Raycast(tOrigin, tDirection, hit, fDistance, &mask);
}

