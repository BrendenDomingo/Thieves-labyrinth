#include "LightComponent.h"
#include "EnumTypes.h"
#include "Entity.h"
#include "Transform.h"

CLightComponent::CLightComponent(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_pcOwner = pcOwner;
	m_nComponentType = eComponent::LIGHT;
}

void CLightComponent::SetPosition(CMath::TVECTOR3 vPos)
{
	m_vLightPosition = vPos;
}

void CLightComponent::SetColor(CMath::TVECTOR4 vColor)
{
	m_vColor = vColor;
}

void CLightComponent::SetRadius(float fRadius)
{
	m_fRadius = fRadius;
}


CMath::TVECTOR3 CLightComponent::GetRelativePosition()
{
	return m_vLightPosition;
}

CMath::TVECTOR3 CLightComponent::GetWorldPosition()
{
	return  m_pcOwner->GetComponent<CTransform>()->GetPosition() + m_vLightPosition;
}

CMath::TVECTOR4 CLightComponent::GetColor()
{
	return m_vColor;
}

float CLightComponent::GetRadius()
{
	return m_fRadius;
}

CLightComponent& CLightComponent::operator=(CLightComponent & cCopy)
{
	m_vColor = cCopy.m_vColor;
	m_vLightPosition = cCopy.m_vLightPosition;
	m_fRadius = cCopy.m_fRadius;
	

	return *this;
}

bool CLightComponent::operator==(const CLightComponent * cRight)
{
	if (CMath::IsEqual(m_fRadius, cRight->m_fRadius) &&
		CMath::IsEqual(m_vColor, cRight->m_vColor) && 
		CMath::IsEqual(m_vLightPosition, cRight->m_vLightPosition)
		)
	{
		return true;
	}
	return false;
}

CLightComponent::~CLightComponent()
{
}
