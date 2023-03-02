#include "BoxCollider.h"
#include "EventManager.h"
#include "Transform.h"
#include "EnumTypes.h"
#include "Entity.h"

CBoxCollider::CBoxCollider(IEntity * pcOwner) : ICollider(pcOwner)
{
	m_pcOwner = pcOwner;
	m_nComponentType = eComponent::BOX_COLLIDER;
}

CMath::TVECTOR3 CBoxCollider::GetCenter()
{
	return m_tCenter;
}

CMath::TVECTOR3 CBoxCollider::GetLocalCenter()
{
	return CMath::Vector3Transform(m_tCenter, m_pcTransform->GetMatrix());
}

CMath::TVECTOR3 CBoxCollider::GetExtent()
{
	return m_tExtent;
}

void CBoxCollider::SetCenter(CMath::TVECTOR3 tCenter)
{
	m_tCenter = tCenter;
}

void CBoxCollider::SetExtent(CMath::TVECTOR3 tExtent)
{
	m_tExtent = tExtent;
}

void CBoxCollider::RenderCollider()
{
	if (m_pcOwner->m_nEntityId == 326)
	{
		bool debugBreak = true;
	}
	CMath::TMATRIX tTransformMatrix = m_pcTransform->GetMatrix();

	CMath::TVECTOR3 tLocalOffset = GetLocalCenter();

	CMath::TVECTOR3 AA = CMath::Point3Transform(m_tExtent, tTransformMatrix) + tLocalOffset;
	CMath::TVECTOR3 BA = CMath::Point3Transform(CMath::TVECTOR3{ -m_tExtent.x,  m_tExtent.y,   m_tExtent.z }, tTransformMatrix) + tLocalOffset;
	CMath::TVECTOR3 CA = CMath::Point3Transform(CMath::TVECTOR3{ -m_tExtent.x,  m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + tLocalOffset;
	CMath::TVECTOR3 DA = CMath::Point3Transform(CMath::TVECTOR3{ m_tExtent.x,  m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + tLocalOffset;
	CMath::TVECTOR3 AB = CMath::Point3Transform(CMath::TVECTOR3{ m_tExtent.x, -m_tExtent.y,   m_tExtent.z }, tTransformMatrix) + tLocalOffset;
	CMath::TVECTOR3 BB = CMath::Point3Transform(CMath::TVECTOR3{ -m_tExtent.x, -m_tExtent.y,   m_tExtent.z }, tTransformMatrix) + tLocalOffset;
	CMath::TVECTOR3 CB = CMath::Point3Transform(CMath::TVECTOR3{ -m_tExtent.x, -m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + tLocalOffset;
	CMath::TVECTOR3 DB = CMath::Point3Transform(CMath::TVECTOR3{ m_tExtent.x, -m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + tLocalOffset;

	//Top square 
	TDebugLineMessage vertex = TDebugLineMessage(AA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(BA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(CA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(DA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);


	vertex = TDebugLineMessage(DA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(AA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	//Bottom square 
	vertex = TDebugLineMessage(AB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(BB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(CB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(DB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);


	vertex = TDebugLineMessage(DB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(AB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	//Connectors

	vertex = TDebugLineMessage(AA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(AB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(BB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(CB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);


	vertex = TDebugLineMessage(DA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(DB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
}

CBoxCollider & CBoxCollider::operator=(CBoxCollider & cCopy)
{
	m_tCenter = cCopy.GetCenter();
	m_tExtent = cCopy.GetExtent();
	m_bIsTrigger = cCopy.IsTrigger();
	return *this;
}

CBoxCollider::~CBoxCollider()
{
}
