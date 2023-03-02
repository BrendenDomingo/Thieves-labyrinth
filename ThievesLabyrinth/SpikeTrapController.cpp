#include "SpikeTrapController.h"
#include "EnumTypes.h"
#include "BoxCollider.h"
#include "Entity.h"
#include "Transform.h"
#include "EventManager.h"
#include "MeshRenderer.h"

CSpikeTrapController::CSpikeTrapController(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::SPIKE_TRAP_COMPONENT;
	m_pcCollider = m_pcOwner->GetComponent<CBoxCollider>();
	m_pcTransform = m_pcOwner->GetComponent<CTransform>();

	m_fCurrentTime = 0.0f;
	m_fCooldown = 2.0f;
	m_fActiveTime = 1.0f;
	m_fDamagePerHit = 2.0f;
}

void CSpikeTrapController::Update(float fDeltaTime)
{

	if(m_bIsTrapActive)
	{
		if(m_fCurrentTime >= m_fActiveTime)
		{
			ToggleTrap();
			return;
		}
	}
	else
	{
		if(m_fCurrentTime >= m_fCooldown)
		{
			ToggleTrap();
			return;
		}

	}

	m_fCurrentTime += fDeltaTime;
}

void CSpikeTrapController::SetDamage(float fDamage)
{
	m_fDamagePerHit = fDamage;
}

float CSpikeTrapController::GetDamage()
{
	return m_fDamagePerHit;
}

void CSpikeTrapController::ToggleTrap()
{
	if(m_bIsTrapActive)
	{
		//m_pcTransform->SetPosition(m_pcTransform->GetPosition() - CMath::TVECTOR3{ 0,1.5f,0 });
		//m_pcTransform->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 0, 0 }, 180));
		m_pcCollider->SetActiveState(false);
		m_pcOwner->GetComponent<CMeshRenderer>()->SetActiveState(false);
	}
	else
	{
		CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::SPIKES));
		//m_pcTransform->SetPosition(m_pcTransform->GetPosition() + CMath::TVECTOR3{0,1.5f,0});
		m_pcCollider->SetActiveState(true);
		m_pcOwner->GetComponent<CMeshRenderer>()->SetActiveState(true);
	}

	m_fCurrentTime = 0.0f;
	m_bIsTrapActive = !m_bIsTrapActive;
}

CSpikeTrapController& CSpikeTrapController::operator=(CSpikeTrapController & cToCopy)
{
	m_fActiveTime = cToCopy.m_fActiveTime;
	m_fCooldown = cToCopy.m_fCooldown;
	m_fCurrentTime = cToCopy.m_fCooldown;
	m_bIsTrapActive = cToCopy.m_bIsTrapActive;

	return *this;
}

CSpikeTrapController::~CSpikeTrapController()
{
}
