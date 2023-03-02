#include "CapsuleCollider.h"
#include "EventManager.h"
#include "Transform.h"

#define DEG2RAD(x) x * 3.1416f / 180.0f

CCapsuleCollider::CCapsuleCollider(IEntity* pcOwner) : ICollider(pcOwner)
{
	m_pcOwner = pcOwner;
	m_tCenter = CMath::TVECTOR3(0, 0, 0);
	m_fHeight = 5.0f;
	m_fRadius = 2.0f;
}

CCapsuleCollider & CCapsuleCollider::operator=(CCapsuleCollider & cCopy)
{
	m_tCenter = cCopy.GetCenter();
	m_fHeight = cCopy.GetHeight();
	m_fRadius = cCopy.GetRadius();
	m_bIsTrigger = cCopy.IsTrigger();

	return *this;
}

CCapsuleCollider::~CCapsuleCollider()
{

}

CMath::TVECTOR3 CCapsuleCollider::GetCenter()
{
	return m_tCenter;
}

float CCapsuleCollider::GetHeight()
{
	return m_fHeight;
}

float CCapsuleCollider::GetRadius()
{
	return m_fRadius;
}

void CCapsuleCollider::GetMiddleSegment(CMath::TVECTOR3 & tBottom, CMath::TVECTOR3 & tTop)
{
	CMath::TMATRIX tTransformMatrix = m_pcTransform->GetMatrix();
	CMath::TVECTOR3 tYAxis = CMath::TVECTOR3(tTransformMatrix.r[1].x, tTransformMatrix.r[1].y, tTransformMatrix.r[1].z);

	tBottom = m_pcTransform->GetPosition() + m_tCenter - tYAxis * (m_fHeight / 2.0f);
	tTop = m_pcTransform->GetPosition() + m_tCenter + tYAxis * (m_fHeight / 2.0f);
}

void CCapsuleCollider::SetCenter(CMath::TVECTOR3 tCenter)
{
	m_tCenter = tCenter;
}

void CCapsuleCollider::SetHeight(float fHeight)
{
	m_fHeight = fHeight - m_fRadius * 2.0f;
}

void CCapsuleCollider::SetRadius(float fRadius)
{
	m_fHeight += m_fRadius * 2.0f;
	m_fRadius = fRadius;
	m_fHeight -= m_fRadius * 2.0f;
}

void CCapsuleCollider::RenderCollider()
{
	CMath::TMATRIX tTransformMatrix = m_pcTransform->GetMatrix();
	CMath::TVECTOR3 tPosition = m_pcTransform->GetPosition() + m_tCenter;

	CMath::TVECTOR3 tAxisX = CMath::TVECTOR3(tTransformMatrix.r[0].x, tTransformMatrix.r[0].y, tTransformMatrix.r[0].z);
	CMath::TVECTOR3 tAxisY = CMath::TVECTOR3(tTransformMatrix.r[1].x, tTransformMatrix.r[1].y, tTransformMatrix.r[1].z);
	CMath::TVECTOR3 tAxisZ = CMath::TVECTOR3(tTransformMatrix.r[2].x, tTransformMatrix.r[2].y, tTransformMatrix.r[2].z);

	const int VERT_COUNT = 6;
	const int CIRCLE_VERT_COUNT = 16;
	const CMath::TVECTOR4 DEBUG_COLOR = CMath::TVECTOR4(0, 1, 0, 1);

	CMath::TVECTOR3 Right[] = {
		(tAxisX * (m_fRadius * 0.6f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition,
		(tAxisX * (m_fRadius * 0.8f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisX * m_fRadius) - (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisX * m_fRadius) + (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisX * (m_fRadius * 0.8f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisX * (m_fRadius * 0.6f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition };

	CMath::TVECTOR3 Left[] = {
		(tAxisX * (-m_fRadius * 0.6f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition,
		(tAxisX * (-m_fRadius * 0.8f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisX * -m_fRadius) - (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisX * -m_fRadius) + (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisX * (-m_fRadius * 0.8f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisX * (-m_fRadius * 0.6f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition, };

	CMath::TVECTOR3 Front[] = {
		(tAxisZ * (m_fRadius * 0.6f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition,
		(tAxisZ * (m_fRadius * 0.8f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisZ * m_fRadius) - (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisZ * m_fRadius) + (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisZ * (m_fRadius * 0.8f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisZ * (m_fRadius * 0.6f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition };

	CMath::TVECTOR3 Back[] = {
		(tAxisZ * (-m_fRadius * 0.6f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition,
		(tAxisZ * (-m_fRadius * 0.8f)) - (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisZ * -m_fRadius) - (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisZ * -m_fRadius) + (tAxisY * m_fHeight * 0.5f) + tPosition,
		(tAxisZ * (-m_fRadius * 0.8f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.6f)) + tPosition,
		(tAxisZ * (-m_fRadius * 0.6f)) + (tAxisY * (m_fHeight * 0.5f + m_fRadius * 0.8f)) + tPosition };

	float deg30 = DEG2RAD(30);
	float deg45 = DEG2RAD(45);
	float deg60 = DEG2RAD(60);
	CMath::TVECTOR3 TopCircle[] = {
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * cos(deg30) + tAxisZ * m_fRadius * sin(deg30) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * cos(deg45) + tAxisZ * m_fRadius * sin(deg45) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * cos(deg60) + tAxisZ * m_fRadius * sin(deg60) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisZ * m_fRadius + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(-deg60) + tAxisZ * m_fRadius * -sin(-deg60) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(-deg45) + tAxisZ * m_fRadius * -sin(-deg45) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(-deg30) + tAxisZ * m_fRadius * -sin(-deg30) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * -m_fRadius + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(deg30) + tAxisZ * m_fRadius * -sin(deg30) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(deg45) + tAxisZ * m_fRadius * -sin(deg45) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(deg60) + tAxisZ * m_fRadius * -sin(deg60) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisZ * -m_fRadius + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * cos(-deg60) + tAxisZ * m_fRadius * sin(-deg60) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * cos(-deg45) + tAxisZ * m_fRadius * sin(-deg45) + tPosition,
		tAxisY * m_fHeight * 0.5f + tAxisX * m_fRadius * cos(-deg30) + tAxisZ * m_fRadius * sin(-deg30) + tPosition, };

	CMath::TVECTOR3 BottomCircle[] = {
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * cos(deg30) + tAxisZ * m_fRadius * sin(deg30) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * cos(deg45) + tAxisZ * m_fRadius * sin(deg45) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * cos(deg60) + tAxisZ * m_fRadius * sin(deg60) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisZ * m_fRadius + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(-deg60) + tAxisZ * m_fRadius * -sin(-deg60) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(-deg45) + tAxisZ * m_fRadius * -sin(-deg45) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(-deg30) + tAxisZ * m_fRadius * -sin(-deg30) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * -m_fRadius + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(deg30) + tAxisZ * m_fRadius * -sin(deg30) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(deg45) + tAxisZ * m_fRadius * -sin(deg45) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * -cos(deg60) + tAxisZ * m_fRadius * -sin(deg60) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisZ * -m_fRadius + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * cos(-deg60) + tAxisZ * m_fRadius * sin(-deg60) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * cos(-deg45) + tAxisZ * m_fRadius * sin(-deg45) + tPosition,
		tAxisY * -m_fHeight * 0.5f + tAxisX * m_fRadius * cos(-deg30) + tAxisZ * m_fRadius * sin(-deg30) + tPosition, };

	CMath::TVECTOR3 Bottom = tAxisY * -(m_fHeight * 0.5f + m_fRadius) + tPosition;
	CMath::TVECTOR3 Top = tAxisY * (m_fHeight * 0.5f + m_fRadius) + tPosition;

	TDebugLineMessage vertex = TDebugLineMessage(Bottom, DEBUG_COLOR);
	for (int i = 0; i < VERT_COUNT; i++)
	{
		CEventManager::SendDebugLineMessage(vertex);
		vertex = TDebugLineMessage(Right[i], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);
	}
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(Bottom, DEBUG_COLOR);
	for (int i = 0; i < VERT_COUNT; i++)
	{
		CEventManager::SendDebugLineMessage(vertex);
		vertex = TDebugLineMessage(Left[i], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);
	}
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(Bottom, DEBUG_COLOR);
	for (int i = 0; i < VERT_COUNT; i++)
	{
		CEventManager::SendDebugLineMessage(vertex);
		vertex = TDebugLineMessage(Front[i], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);
	}
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(Bottom, DEBUG_COLOR);
	for (int i = 0; i < VERT_COUNT; i++)
	{
		CEventManager::SendDebugLineMessage(vertex);
		vertex = TDebugLineMessage(Back[i], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);
	}
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);

	for (int i = 0; i < CIRCLE_VERT_COUNT - 1; i++)
	{
		vertex = TDebugLineMessage(TopCircle[i], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);
		vertex = TDebugLineMessage(TopCircle[i + 1], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);

		vertex = TDebugLineMessage(BottomCircle[i], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);
		vertex = TDebugLineMessage(BottomCircle[i + 1], DEBUG_COLOR);
		CEventManager::SendDebugLineMessage(vertex);
	}
	vertex = TDebugLineMessage(TopCircle[CIRCLE_VERT_COUNT - 1], DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(TopCircle[0], DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BottomCircle[CIRCLE_VERT_COUNT - 1], DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(BottomCircle[0], DEBUG_COLOR);
	CEventManager::SendDebugLineMessage(vertex);
}
