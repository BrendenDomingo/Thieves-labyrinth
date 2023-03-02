#pragma once
#include "Collider.h"

class CTransform;

class CBoxCollider : public ICollider
{
	CMath::TVECTOR3 m_tCenter;
	CMath::TVECTOR3 m_tExtent;

public:
	CBoxCollider(IEntity* pcOwner);

	/***************
	* Accessors
	***************/

	CMath::TVECTOR3 GetCenter();
	CMath::TVECTOR3 GetLocalCenter();
	CMath::TVECTOR3 GetExtent();

	/***************
	* Mutators
	***************/

	void SetCenter(CMath::TVECTOR3 tCenter);
	void SetExtent(CMath::TVECTOR3 tExtent);

	void RenderCollider();

	CBoxCollider& operator=(CBoxCollider& cCopy);
	~CBoxCollider();
};
