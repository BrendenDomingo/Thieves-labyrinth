#include "MinotaurController.h"
#include "Entity.h"
#include "InputManager.h"
#include "Transform.h"
#include <iostream>
#include "EventManager.h"


CMinotaurController::CMinotaurController(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::MINOTAUR_CONTROLLER;
	m_fSpeed = 15.0f;

	m_pcRigidbody = this->m_pcOwner->GetComponent<CRigidbody>();
	m_pcTransform = this->m_pcOwner->GetComponent<CTransform>();
}

void CMinotaurController::Update()
{

	if (!m_pcRigidbody)
	{
		// TODO: Add debug error message
		return;
	}
	CMath::TVECTOR3 TVel, tNormalVelocity;

	if (CInputManager::GetKeyDown('W'))
	{
		TVel.z = 1;
	}
	else if (CInputManager::GetKeyDown('S'))
	{
		TVel.z = -1;
	}

	if (CInputManager::GetKeyDown('D'))
	{
		TVel.x = 1;
	}
	else if (CInputManager::GetKeyDown('A'))
	{
		TVel.x = -1;
	}

	if (CInputManager::GetKeyDown(VK_RBUTTON))
	{
		CInputManager::GetMousePosNDC(TVel.x, TVel.z);
	}

	tNormalVelocity = CMath::Vector3Normalize(TVel);
	m_pcRigidbody->SetVelocity(tNormalVelocity * m_fSpeed);

	if (CMath::Vector3Magnitude(tNormalVelocity) > 0)
	{
		m_pcTransform->TurnTo(tNormalVelocity);
		CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::PLAYERMOVE));
	}
	else
	{
		CEventManager::SendAudioMessage(TAudioMessage(false, eAudio::SFX, eSFX::PLAYERMOVE));
	}
}

CMinotaurController::~CMinotaurController()
{
}

void CMinotaurController::SetSpeed(float _speed)
{
	m_fSpeed = _speed;
}

float CMinotaurController::GetSpeed()
{
	return m_fSpeed;
}

