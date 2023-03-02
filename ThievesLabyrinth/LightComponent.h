#pragma once
#include "Component.h"

class CLightComponent : public IComponent
{
	CMath::TVECTOR3 m_vLightPosition;
	CMath::TVECTOR4 m_vColor;
	float			m_fRadius;

public:
	CLightComponent(IEntity* pcOwner);

	void SetPosition(CMath::TVECTOR3 vPos);
	void SetColor(CMath::TVECTOR4 vColor);
	void SetRadius(float fRadius);

	CMath::TVECTOR3 GetRelativePosition();
	CMath::TVECTOR3 GetWorldPosition();
	CMath::TVECTOR4 GetColor();
	float GetRadius();

	CLightComponent& operator=(CLightComponent& cCopy);
	bool operator==(const CLightComponent* cRight);

	~CLightComponent();
};

