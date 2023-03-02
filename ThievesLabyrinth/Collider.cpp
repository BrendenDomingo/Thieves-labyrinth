#include "Collider.h"
#include "EventManager.h"
#include "Transform.h"
#include "Entity.h"

ICollider::ICollider(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_pcTransform = (CTransform*) pcOwner->GetComponent<CTransform>();
	m_bIsTrigger = false;
}

bool ICollider::IsTrigger()
{
	return m_bIsTrigger;
}

void ICollider::SetTrigger(bool bIsTrigger)
{
	m_bIsTrigger = bIsTrigger;
}


ICollider::~ICollider()
{
}

