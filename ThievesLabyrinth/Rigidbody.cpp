#include "Rigidbody.h"
#include "EnumTypes.h"
#include "Entity.h"


CRigidbody::CRigidbody(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::RIGIDBODY;
}

void CRigidbody::SetVelocity(CMath::TVECTOR3 tVelocity)
{
	m_tVelocity = tVelocity;
}

CRigidbody & CRigidbody::operator=(CRigidbody & cCopy)
{
	m_tVelocity = cCopy.GetVelocity();

	return *this;
}

CMath::TVECTOR3 CRigidbody::GetVelocity()
{
	return m_tVelocity;
}

CRigidbody::~CRigidbody()
{
}

