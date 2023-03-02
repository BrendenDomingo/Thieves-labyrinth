#include "CameraController.h"

#include "InputManager.h"
#include "EventManager.h"
#include "Rigidbody.h"

#include "Time.h"
#include "Entity.h"

#include "Transform.h"
#include "EnumTypes.h"
#include "DebugDefines.h"

CTransform* CCameraController::m_pCameraTransform;

CCameraController::CCameraController(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_pPlayerPos = CEventManager::SendEntityMessage(TEntityMessage(0))->GetComponent<CTransform>();
	m_pCameraTransform = m_pcOwner->GetComponent<CTransform>();
	m_pcRigidBody = m_pcOwner->GetComponent<CRigidbody>();

	CInputManager::SetViewTransform(*m_pCameraTransform);

	CMath::TVECTOR3 vPos, vScale;
	CMath::TVECTOR4 vRot;
	float fRadians = TORAD(65.0f);
	float fDistance = 30.0f;

	CMath::TMATRIX mat = CMath::MatrixXRotation(fRadians) * CMath::MatrixYRotation(TORAD(45.0f));
	CMath::MatrixDecompose(&vScale, &vRot, &vPos, mat);

	m_pCameraTransform->SetRotation(vRot);	
	m_tCameraRotation = vRot;

	float fXZ = -cosf(fRadians) * (fDistance - 10);
	float fYDis = sinf(fRadians) * fDistance;

	m_tCameraOffset = CMath::TVECTOR3{ fXZ, fYDis , fXZ };
	m_tCameraOffsetDebug = CMath::TVECTOR3{ fXZ * 2.0f , fYDis * 2.0f, fXZ * 2.0f };

	m_nComponentType = eComponent::CAMERA_CONTROLLER;
	m_bDebug = false;
	m_fSpeed = 15.0f;
	m_fSpeedDebug = 100.0f;
}

CTransform * CCameraController::GetCamera()
{
	return m_pCameraTransform;
}

void CCameraController::Update()
{
	if (!m_pcRigidBody)
	{
		// TODO: Add debug error message
		return;
	}

#if DEBUGCAMERA
	
	if(CInputManager::GetKeyDown('X'))
	{
		m_bDebug = !m_bDebug;
		if (m_bDebug)
		{
			m_pCameraTransform->SetPosition(m_pPlayerPos->GetPosition() + m_tCameraOffsetDebug);
		}
		m_pCameraTransform->SetRotation(m_tCameraRotation);
	}


	if (m_bDebug)
	{
		CMath::TVECTOR3 vMovement;
		float fDelta = (float)CTime::GetDelta();

		if(CInputManager::GetKey('W'))
		{
			vMovement.z = 1;
		}
		else if(CInputManager::GetKey('S'))
		{
			vMovement.z = -1;
		}

		if(CInputManager::GetKey('D'))
		{
			vMovement.x = 1;
		}
		else if(CInputManager::GetKey('A'))
		{
			vMovement.x = -1;
		}

		if (CInputManager::GetKey(VK_SPACE))
		{
			//vMovement.y = 1;
			m_pCameraTransform->SetMatrix( m_pCameraTransform->GetMatrix() * CMath::MatrixTranslationFromVector(CMath::TVECTOR3{ 0, 1.0f * fDelta,0 } * m_fSpeedDebug));
		}

		if (CInputManager::GetKey(VK_CONTROL))
		{
			//vMovement.y = -1;			
			m_pCameraTransform->SetMatrix(m_pCameraTransform->GetMatrix() * CMath::MatrixTranslationFromVector(CMath::TVECTOR3{ 0, -1.0f * fDelta,0 } * m_fSpeedDebug));

		}

		if (CInputManager::GetKey(VK_RBUTTON))
		{
			CMath::TVECTOR2 vMouse;
			int x = 0, y = 0;
			CInputManager::GetMouseDelta(x, y);
			vMouse.x = (float)x;
			vMouse.y = (float)y;

			CMath::TMATRIX tRotMat = m_pCameraTransform->GetMatrix();
			CMath::TVECTOR4 vPosition = tRotMat.r[3];

			tRotMat.r[3] = { 0,0,0 };
			tRotMat *= CMath::MatrixYRotation(vMouse.x * fDelta);
			tRotMat.r[3] = vPosition;
			tRotMat = CMath::MatrixXRotation(vMouse.y * fDelta) * tRotMat;

			m_pCameraTransform->SetMatrix(tRotMat);
		}

		m_pCameraTransform->SetMatrix(CMath::MatrixTranslationFromVector(vMovement * fDelta * m_fSpeedDebug) * m_pCameraTransform->GetMatrix() );
	}
	else
#endif // DEBUGCAMERA

	{
		m_pCameraTransform->SetPosition(m_pPlayerPos->GetPosition() + m_tCameraOffset);
	}
}

CCameraController::~CCameraController()
{
}
