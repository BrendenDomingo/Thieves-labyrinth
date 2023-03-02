#pragma once

#include "Component.h"


class CRigidbody : public IComponent
{
	CMath::TVECTOR3 m_tVelocity;

public:
	CRigidbody(IEntity* pcOwner);

	
	/***************
	* Accessors
	***************/
	CMath::TVECTOR3 GetVelocity();

	/***************
	* Mutators
	***************/
	void SetVelocity(CMath::TVECTOR3 tVelocity);

	CRigidbody& operator=(CRigidbody& CCopy);

	~CRigidbody();
};

