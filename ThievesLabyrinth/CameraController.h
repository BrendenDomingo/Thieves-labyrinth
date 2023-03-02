#pragma once
#include "Component.h"

class IEntity;
class CRigidbody;
class CTransform;

class CCameraController : public IComponent
{
	CRigidbody* m_pcRigidBody;
	CTransform* m_pPlayerPos;
	static CTransform* m_pCameraTransform;
	float m_fSpeed;
	float m_fSpeedDebug;
	bool follow;
	bool m_bDebug;
	IEntity* Target;
	CMath::TVECTOR4 m_tCameraRotation;
	CMath::TVECTOR3 m_tCameraOffset;
	CMath::TVECTOR3 m_tCameraOffsetDebug;

public:
	CCameraController(IEntity* pcOwner);

	static CTransform* GetCamera();

	void Update();

	~CCameraController();
};

