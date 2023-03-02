#pragma once
#include "Component.h"

class CTransform;

class ICollider : public IComponent
{

protected:
	CTransform* m_pcTransform;

	bool m_bIsTrigger;

public:
	ICollider(IEntity* pcOwner);

	/***************
	* Accessors
	***************/

	bool IsTrigger();

	/***************
	* Mutators
	***************/

	void SetTrigger(bool bIsTrigger);


	virtual void RenderCollider() = 0;

	~ICollider();
};