#include "IndicatorController.h"
#include "Time.h"
#include "EntityManager.h"
#include "EnumTypes.h"
#include "Entity.h"
#include "Transform.h"

CIndicatorController::CIndicatorController(IEntity * pcOwner) : IComponent(pcOwner)
{
	m_fTimer = 1.0f;
	m_nComponentType = eComponent::CLICK_INDICATOR;
	m_pcTransform = m_pcOwner->GetComponent<CTransform>();
}

void CIndicatorController::Update(float fDeltaTime)
{

	if(m_fTimer > 0.0f)
	{
		m_fTimer -= fDeltaTime * 2;
		
		m_pcTransform->SetScale(m_pcTransform->GetScale() + CMath::TVECTOR3(fDeltaTime, fDeltaTime, fDeltaTime) * 2);
	}
	else
	{
		CEntityManager::AddEntityToDeletionQueue(m_pcOwner);
	}
}

CIndicatorController& CIndicatorController::operator=(CIndicatorController & cToCopy)
{
	m_fTimer = cToCopy.m_fTimer;

	return *this;
}

float CIndicatorController::GetTime()
{
	return m_fTimer;
}

CIndicatorController::~CIndicatorController()
{
}
