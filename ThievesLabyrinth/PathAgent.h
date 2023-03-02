#pragma once
#include "Component.h"
#include "PathSurface.h"
#include "CustomPriorityQueue.h"
#include <vector>
#include <map>

class CTransform;
class CRigidbody;
class CStats;

typedef CMath::TVECTOR3 Vector3;
typedef CMath::TVECTOR2 Vector2;

class CPathAgent : public IComponent
{
private:

	struct TPlannerNode
	{
		TPlannerNode* m_ptParent;
		TSearchNode* m_ptSearchNode;
		float m_fGivenCost;
		float m_fHeuristicCost;
		float m_fFinalCost;
	};

	struct NodeCompare
	{
		bool operator()(TPlannerNode* a, TPlannerNode* b)
		{
			return (a->m_fFinalCost > b->m_fFinalCost);
		}
	};

	bool m_bNavigate;
	bool m_bTargetChanged;

	float m_fRaycastTimer;
	float m_fMaxRaycastTimer;
	bool  m_bCanWalkStraight;

	CPathSurface* m_pcSurface;
	TSearchNode* m_ptStart;
	TSearchNode* m_ptTarget;

	CTransform* m_pcTransform;
	CRigidbody* m_pcRigidbody;
	CStats*		m_pcStats;

	TSearchNode* m_ptCurrent;
	Vector3 m_tNextPosition;

	Vector3 m_tTargetPosition;
	CTransform* m_pcTargetTransform;

	custom_priority_queue<TPlannerNode*, std::vector<TPlannerNode*>, NodeCompare> m_tQueue;
	std::map<TSearchNode*, TPlannerNode*> m_tVisited;
	std::vector<TSearchNode*> m_tFinalPathDebug;
	std::vector<Vector3> m_tFinalPath;

public:
	CPathAgent(IEntity* pcOwner);
	~CPathAgent();

	CPathSurface* GetSurface();

	void SetTarget(CMath::TVECTOR3 tNewTarget);
	void SetTarget(CTransform* pcTransform);

	void SetSurface(CPathSurface* pcNewSurface);

	void CalculatePath();
	void Start();
	void Stop();

	bool IsNavigating() const;

	TSearchNode* GetStartNode(Vector3 tStartPosition, Vector2 nMin, Vector2 nMax) const;
	TSearchNode* GetTargetNode(Vector3 tTargetPosition, Vector2 nMin, Vector2 nMax) const;

	int GetCoordinateX(float fPosition, int nMin, int nMax) const;
	int GetCoordinateZ(float fPosition, int nMin, int nMax) const;

	void Navigate(); 

private:
	void ClearVisited();
	void ClearPath();
	void SetPath(TPlannerNode* pTargetNode);
	void CalculateTargetNode(Vector3 tPosition);
	float CalculateHeuristic(TSearchNode* current) const;
	float GetDistance(TSearchNode* tNodeA, TSearchNode* tNodeB);
	bool IsTargetInSight() const;
};
