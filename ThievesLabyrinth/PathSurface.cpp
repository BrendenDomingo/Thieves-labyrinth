#include "PathSurface.h"
#include "Transform.h"
#include "Entity.h"
#include "EventManager.h"
#include "PhysicsManager.h"
#include "Level.h"
#include "BoxCollider.h"

CPathSurface::CPathSurface(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::PATH_SURFACE;

	m_fNodeDistance = 5.0f;

	m_bHasWeights = false;

	int nRoomWidth = 100;
	int nRoomHeight = 100;

	m_nColumns = int(nRoomWidth / m_fNodeDistance);
	m_nRows = int(nRoomHeight / m_fNodeDistance);

	CTransform* pcTransform = pcOwner->GetComponent<CTransform>();
	m_tOrigin = pcTransform->GetPosition();

	m_ptNodes = new TSearchNode[m_nRows * m_nColumns];

	for (int row = 0; row < m_nRows; row++)
	{
		for (int col = 0; col < m_nColumns; col++)
		{
			bool bIsLeft = (col == 0);
			bool bIsRight = (col == m_nColumns - 1);
			bool bIsTop = (row == 0);
			bool bIsBottom = (row == m_nRows - 1);

			TSearchNode* tCurrent = &m_ptNodes[GetIndex(row, col, m_nColumns)];

			if (!bIsLeft)
			{
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row, col - 1, m_nColumns)]);

				if (!bIsBottom)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row + 1, col - 1, m_nColumns)]);

				if (!bIsTop)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row - 1, col - 1, m_nColumns)]);
			}

			if (!bIsRight)
			{
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row, col + 1, m_nColumns)]);

				if (!bIsBottom)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row + 1, col + 1, m_nColumns)]);

				if (!bIsTop)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row - 1, col + 1, m_nColumns)]);
			}

			if(!bIsBottom)
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row + 1, col, m_nColumns)]);

			if(!bIsTop)
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row - 1, col, m_nColumns)]);

			float fPositionX = m_tOrigin.x - (float)(nRoomWidth) / 2.0f + col * m_fNodeDistance;
			float fPositionZ = m_tOrigin.z + (float)(nRoomHeight) / 2.0f - row * m_fNodeDistance;
			tCurrent->m_tPosition = Vector3(fPositionX, 0.5f, fPositionZ);
		}
	}
}
CPathSurface::~CPathSurface()
{
	delete[] m_ptNodes;
}

int CPathSurface::GetRows() const
{
	return m_nRows;
}
int CPathSurface::GetColumns() const
{
	return m_nColumns;
}
CMath::TVECTOR3 CPathSurface::GetOrigin() const
{
	return m_tOrigin
;
}
float CPathSurface::GetNodeDistance() const
{
	return m_fNodeDistance;
}

TSearchNode* CPathSurface::GetNodeAtIndex(int x, int z) const
{
	return &m_ptNodes[GetIndex(z, x, m_nColumns)];
}

void CPathSurface::SetOrigin(CMath::TVECTOR3 tNewOrigin)
{
	int nRoomWidth = 100;
	int nRoomHeight = 100;

	m_tOrigin = tNewOrigin;

	for (int row = 0; row < m_nRows; row++)
	{
		for (int col = 0; col < m_nColumns; col++)
		{
			bool bIsLeft = (col == 0);
			bool bIsRight = (col == m_nColumns - 1);
			bool bIsTop = (row == 0);
			bool bIsBottom = (row == m_nRows - 1);

			TSearchNode* tCurrent = &m_ptNodes[GetIndex(row, col, m_nColumns)];
			tCurrent->m_ptNeighbors.clear();

			if (!bIsLeft)
			{
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row, col - 1, m_nColumns)]);

				if (!bIsBottom)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row + 1, col - 1, m_nColumns)]);

				if (!bIsTop)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row - 1, col - 1, m_nColumns)]);
			}

			if (!bIsRight)
			{
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row, col + 1, m_nColumns)]);

				if (!bIsBottom)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row + 1, col + 1, m_nColumns)]);

				if (!bIsTop)
					tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row - 1, col + 1, m_nColumns)]);
			}

			if (!bIsBottom)
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row + 1, col, m_nColumns)]);

			if (!bIsTop)
				tCurrent->m_ptNeighbors.push_back(&m_ptNodes[GetIndex(row - 1, col, m_nColumns)]);

			float fPositionX = m_tOrigin.x - (float)(nRoomWidth) / 2.0f + col * m_fNodeDistance;
			float fPositionZ = m_tOrigin.z + (float)(nRoomHeight) / 2.0f - row * m_fNodeDistance;
			tCurrent->m_tPosition = Vector3(fPositionX, 0.5f, fPositionZ);
		}
	}
}
void CPathSurface::CalculateWeights()
{
	for (int i = 0; i < m_nRows; i++)
	{
		Vector3 tOrigin = m_ptNodes[GetIndex(i, 0, m_nColumns)].m_tPosition + Vector3(0,2,0);
		Vector3 tRowVector = m_ptNodes[GetIndex(i, m_nColumns - 1, m_nColumns)].m_tPosition + Vector3(0,2,0) - tOrigin;
		Vector3 tDirection = CMath::Vector3Normalize(tRowVector);
		float fMaxDirection = CMath::Vector3Magnitude(tRowVector);

		CEntityMask mask;
		mask.Add(eEntity::ENVIRONMENT);

		std::vector<TRayHit> hit;
		if (CPhysicsManager::RayOverlap(tOrigin, tDirection, hit, fMaxDirection, &mask))
		{
			for (int j = 0; j < m_nColumns; j++)
			{
				for (int k = 0; k < hit.size(); k++)
				{
					TSearchNode* ptNode = &m_ptNodes[GetIndex(i, j, m_nColumns)];
					TSearchNode* ptNodeAbove = i > 0 ? &m_ptNodes[GetIndex(i - 1, j, m_nColumns)] : nullptr;
					TSearchNode* ptNodeBelow = i < m_nRows - 1 ? &m_ptNodes[GetIndex(i + 1, j, m_nColumns)] : nullptr;
					TSearchNode* ptNodeLeft = j > 0 ? &m_ptNodes[GetIndex(i, j - 1, m_nColumns)] : nullptr;
					TSearchNode* ptNodeRight = j < m_nColumns - 1 ? &m_ptNodes[GetIndex(i, j + 1, m_nColumns)] : nullptr;

					bool bNodeInvalid = CheckNodeValidity(ptNode, &hit[k]);
					bool bNodeAboveInvalid = CheckNodeValidity(ptNodeAbove, &hit[k]);
					bool bNodeBelowInvalid = CheckNodeValidity(ptNodeBelow, &hit[k]);
					bool bNodeLeftInvalid  = CheckNodeValidity(ptNodeLeft, &hit[k]);
					bool bNodeRightInvalid = CheckNodeValidity(ptNodeRight, &hit[k]);

					if (bNodeInvalid)
					{
						ptNode->m_fWeight = 0.0f;
						ptNode->m_tColor = Vector4(0, 0, 0, 1);
					}

					if (bNodeAboveInvalid)
					{
						ptNodeAbove->m_fWeight = 0.0f;
						ptNodeAbove->m_tColor = Vector4(0, 0, 0, 1);
					}

					if (bNodeBelowInvalid)
					{
						ptNodeBelow->m_fWeight = 0.0f;
						ptNodeBelow->m_tColor = Vector4(0, 0, 0, 1);
					}

					if (bNodeLeftInvalid)
					{
						ptNodeLeft->m_fWeight = 0.0f;
						ptNodeLeft->m_tColor = Vector4(0, 0, 0, 1);
					}

					if (bNodeRightInvalid)
					{
						ptNodeRight->m_fWeight = 0.0f;
						ptNodeRight->m_tColor = Vector4(0, 0, 0, 1);
					}
				}
			}
		}
	}

	m_bHasWeights = true;
}

bool CPathSurface::HasWeights() const
{
	return m_bHasWeights;
}

void CPathSurface::DrawNodes()
{
	for (int row = 0; row < m_nRows; row++)
	{
		for (int col = 0; col < m_nColumns; col++)
		{
			int index = GetIndex(row, col, m_nColumns);
			TSearchNode* current = &m_ptNodes[index];
			for (TSearchNode* neighbor : current->m_ptNeighbors)
			{
				TDebugLineMessage vertex = TDebugLineMessage(current->m_tPosition, current->m_tColor);
				CEventManager::SendDebugLineMessage(vertex);
				vertex = TDebugLineMessage(neighbor->m_tPosition, neighbor->m_tColor);
				CEventManager::SendDebugLineMessage(vertex);
			}
		}
	}
}

CPathSurface& CPathSurface::operator=(CPathSurface& cCopy)
{
	this->m_nComponentType = cCopy.m_nComponentType;
	this->m_tOrigin = cCopy.m_tOrigin;

	if (this->m_ptNodes)
	{
		delete[] m_ptNodes;
	}

	this->m_ptNodes = new TSearchNode[m_nRows * m_nColumns];
	for (int row = 0; row < m_nRows; row++)
	{
		for (int col = 0; col < m_nColumns; col++)
		{
			int index = GetIndex(row, col, m_nColumns);
			this->m_ptNodes[index] = cCopy.m_ptNodes[index];
		}
	}

	return *this;
}

int CPathSurface::GetIndex(int row, int column, int numColumns) const
{
	return row * numColumns + column;
}
bool CPathSurface::CheckNodeValidity(TSearchNode* node, TRayHit* hit) const
{
	if (!node || !hit)
		return false;

	CTransform* pcTransform = hit->tEntity->GetComponent<CTransform>();
	CMath::TMATRIX tMatrix = pcTransform->GetMatrix();
	CBoxCollider* boxCollider = hit->tEntity->GetComponent<CBoxCollider>();
	CCapsuleCollider* capCollider = hit->tEntity->GetComponent<CCapsuleCollider>();

	bool bNodeWithinXBounds = false;
	bool bNodeWithinZBounds = false;

	bNodeWithinXBounds = node->m_tPosition.x >= hit->tEntryPoint.x - 2.5f && node->m_tPosition.x <= hit->tExitPoint.x + 2.5f;

	return bNodeWithinXBounds;
}