#include "Transform.h"
#include "EventManager.h"
#include "EnumTypes.h"


CTransform::CTransform(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_tMatrix = CMath::MatrixIdentity();

	m_nComponentType = eComponent::TRANSFORM;
}

CTransform::~CTransform()
{

}

void CTransform::SetPosition(CMath::TVECTOR3 tPosition)
{
	m_tMatrix.r[3] = tPosition;
}

void CTransform::SetRotation(CMath::TVECTOR4 tRotation)
{
	CMath::TVECTOR3 tPosition, tScale;
	CMath::MatrixDecompose(&tScale , nullptr, &tPosition, m_tMatrix);
	m_tMatrix = CMath::MatrixScaleFromVector(tScale) * CMath::MatrixRotationQuaternion(tRotation);
	m_tMatrix.r[3] = tPosition;
}

void CTransform::SetScale(CMath::TVECTOR3 tScale)
{
	CMath::TVECTOR3 vScale;
	CMath::MatrixDecompose(&vScale, nullptr, nullptr, m_tMatrix);
	m_tMatrix = CMath::MatrixScaleFromVector(tScale/vScale) * m_tMatrix;
	/*CMath::TVECTOR4 tRotation;
	CMath::TVECTOR3 tPosition;
	CMath::MatrixDecompose(nullptr, &tRotation, &tPosition, m_tMatrix);
	m_tMatrix = CMath::MatrixScaleFromVector(tScale) * CMath::MatrixRotationQuaternion(tRotation);
	m_tMatrix.r[3] = tPosition;*/
}

void CTransform::SetMatrix(CMath::TMATRIX mMat)
{
	m_tMatrix = mMat;
}

void CTransform::ResetTransform()
{
	m_tMatrix = CMath::MatrixIdentity();
}

CMath::TVECTOR3 CTransform::GetPosition()
{
	return *(CMath::TVECTOR3*)&m_tMatrix.r[3];
}

CMath::TVECTOR4 CTransform::GetRotation()
{
	CMath::TVECTOR4 tRotation;
	CMath::MatrixDecompose(nullptr, &tRotation, nullptr, m_tMatrix);
	return tRotation;
}

CMath::TVECTOR3 CTransform::GetScale()
{
	CMath::TVECTOR3 tScale;
	CMath::MatrixDecompose(&tScale, nullptr, nullptr, m_tMatrix);
	return tScale;
}

CMath::TMATRIX CTransform::GetMatrix()
{
	return m_tMatrix;
}

void CTransform::LookAt(CMath::TVECTOR3 targetPosition, CMath::TVECTOR3 up)
{
	CMath::TVECTOR3 x, y, z, vScale, vPos;
	CMath::MatrixDecompose(&vScale, nullptr, &vPos, m_tMatrix);

	z = CMath::Vector3Normalize(targetPosition - *(CMath::TVECTOR3*)&m_tMatrix.r[3]);
	x = CMath::Vector3Normalize(CMath::Vector3Cross(z, { 0,1,0 }));
	y = CMath::Vector3Normalize(CMath::Vector3Cross(x, z));
	m_tMatrix.r[0] = -x;
	m_tMatrix.r[1] = y;
	m_tMatrix.r[2] = z;
	m_tMatrix.r[3] = vPos;
	m_tMatrix = CMath::MatrixScaleFromVector(vScale) * m_tMatrix;
	return;
	//CMath::TVECTOR3 forward = newForward;
	//CMath::TVECTOR3 up = CMath::TVECTOR3(0, 1, 0);
	//CMath::TVECTOR3 right = CMath::Vector3Cross(up, forward);
	//up = CMath::Vector3Cross(forward, right);
	//CMath::TVECTOR4 newRotation;
	//float tr = right.x + up.y + forward.z;
	//if (tr > 0)
	//{
	//	float S = sqrtf(tr + 1.0f) * 2; // S=4*newRotation.w 
	//	newRotation.w = 0.25f * S;
	//	newRotation.x = (forward.y - up.z) / S;
	//	newRotation.y = (right.z - forward.x) / S;
	//	newRotation.z = (up.x - right.y) / S;
	//}
	//else if ((right.x > up.y) && (right.x > forward.z))
	//{
	//	float S = sqrtf(1.0f + right.x - up.y - forward.z) * 2; // S=4*newRotation.x 
	//	newRotation.w = (forward.y - up.z) / S;
	//	newRotation.x = 0.25f * S;
	//	newRotation.y = (right.y + up.x) / S;
	//	newRotation.z = (right.z + forward.x) / S;
	//}
	//else if (up.y > forward.z)
	//{
	//	float S = sqrtf(1.0f + up.y - right.x - forward.z) * 2; // S=4*newRotation.y
	//	newRotation.w = (right.z - forward.x) / S;
	//	newRotation.x = (right.y + up.x) / S;
	//	newRotation.y = 0.25f * S;
	//	newRotation.z = (up.z + forward.y) / S;
	//}
	//else
	//{
	//	float S = sqrtf(1.0f + forward.z - right.x - up.y) * 2; // S=4*newRotation.z
	//	newRotation.w = (up.x - right.y) / S;
	//	newRotation.x = (right.z + forward.x) / S;
	//	newRotation.y = (up.z + forward.y) / S;
	//	newRotation.z = 0.25f * S;
	//}
	//this->SetRotation(newRotation);
}

void CTransform::TurnTo(CMath::TVECTOR3 vWhereToLook, float fSpeed)
{
	float fdx = CMath::Vector3Dot(vWhereToLook, *(CMath::TVECTOR3*)&m_tMatrix.r[0]);
	if (CMath::IsEqual(fdx, 0.0f))
		fdx += EPSILON;

	CMath::TVECTOR3 vPos;
	vPos = m_tMatrix.r[3];
	m_tMatrix.r[3] = { 0,0,0 };

	m_tMatrix *= CMath::MatrixYRotation(TORAD(fdx) * fSpeed);
	m_tMatrix.r[3] = vPos;


	/*float fdx = CMath::Vector3Dot(vWhereToLook, *(CMath::TVECTOR3*)&m_tMatrix.r[0]);
	CMath::TVECTOR3 vPos, vScale;
	CMath::TVECTOR4 vRot;
	CMath::MatrixDecompose(&vScale, &vRot, &vPos, m_tMatrix);
	CMath::TMATRIX mTemp = CMath::MatrixRotationQuaternion(vRot);

	mTemp *= CMath::MatrixYRotation(TORAD(fdx) * fSpeed);
	mTemp = CMath::MatrixScaleFromVector(vScale) * mTemp;
	mTemp.r[3] = vPos;
	m_tMatrix = mTemp;*/
}

void CTransform::TurnTo(CTransform * target, float fDeltaTime, float fSpeed)
{
	TurnTo(CMath::Vector3Normalize(target->GetPosition() - GetPosition()), fDeltaTime * fSpeed);
}

void CTransform::RenderTransform()
{
	CMath::TVECTOR4 tColors[3]{ { 1, 0, 0, 1}, { 0, 1, 0, 1}, { 0, 0, 1, 1} };

	CMath::TVECTOR3 tPosA{ GetPosition() };

	float fScaleFactor = 5;
	for (int i = 0; i < 3; i++)
	{
		TDebugLineMessage cVertex = TDebugLineMessage(tPosA, tColors[i]);
		CEventManager::SendDebugLineMessage(cVertex);
		CMath::TVECTOR3 tPosB = tPosA + CMath::Vector3Normalize(*(CMath::TVECTOR3*)&m_tMatrix.r[i]) * fScaleFactor;
		//CMath::TVECTOR3 tPosB{ tPosA.x + m_tMatrix.r[i].x * fScaleFactor,  tPosA.y + m_tMatrix.r[i].y * fScaleFactor,  tPosA.z + m_tMatrix.r[i].z * fScaleFactor };

		cVertex = TDebugLineMessage(tPosB, tColors[i]);
		CEventManager::SendDebugLineMessage(cVertex);

	}

}

CTransform & CTransform::operator=(CTransform & cCopy)
{
	//m_tPosition = cCopy.GetPosition();
	//m_tRotation = cCopy.GetRotation();
	//m_tScale = cCopy.GetScale();
	m_tMatrix = cCopy.GetMatrix();

	return *this;
}

