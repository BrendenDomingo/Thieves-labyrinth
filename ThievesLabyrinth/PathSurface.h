#pragma once
#include "Component.h"
#include <vector>

typedef CMath::TVECTOR2 Vector2;
typedef CMath::TVECTOR3 Vector3;
typedef CMath::TVECTOR4 Vector4;

struct TRayHit;

struct TSearchNode
{
	Vector3 m_tPosition;
	float m_fWeight;
	std::vector<TSearchNode*> m_ptNeighbors;
	Vector4 m_tColor;

	TSearchNode() : m_fWeight(1.0f), m_tColor(1,1,1,1) {}
};

class CPathSurface : public IComponent
{
	Vector3 m_tOrigin;
	TSearchNode* m_ptNodes;

	int m_nRows;
	int m_nColumns;

	float m_fNodeDistance;

	bool m_bHasWeights;

public:
	CPathSurface(IEntity* pcOwner);
	~CPathSurface();

	int GetRows() const;
	int GetColumns() const;
	CMath::TVECTOR3 GetOrigin() const;
	float GetNodeDistance() const;

	TSearchNode* GetNodeAtIndex(int x, int z) const;

	void SetOrigin(CMath::TVECTOR3 tNewOrigin);
	void CalculateWeights();

	bool HasWeights() const;

	void DrawNodes();

	CPathSurface& operator=(CPathSurface& cCopy);

private:
	int GetIndex(int column, int row, int numColumns) const;
	bool CheckNodeValidity(TSearchNode* node, TRayHit* hit) const;

};
