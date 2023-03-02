#pragma once
#include "Collider.h"

class CCapsuleCollider : public ICollider
{
public:
	CCapsuleCollider(IEntity* pcOwner);
	CCapsuleCollider& operator=(CCapsuleCollider& cCopy);
	~CCapsuleCollider();

	CMath::TVECTOR3 GetCenter();
	float GetHeight();
	float GetRadius();
	void  GetMiddleSegment(CMath::TVECTOR3& fBottom, CMath::TVECTOR3& tTop);

	void SetCenter(CMath::TVECTOR3 tCenter);
	void SetHeight(float fHeight);
	void SetRadius(float fRadius);

	void RenderCollider();

private:
	CMath::TVECTOR3 m_tCenter;
	float m_fHeight;
	float m_fRadius;
};
