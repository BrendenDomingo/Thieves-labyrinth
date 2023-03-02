#pragma once
#include "Component.h"

class CAnimatorComponent : public IComponent
{
	int		m_nAnimator;
	int		m_nCurrentAnimation;
	double	m_dCurrentTime;
	float	m_fSpeed;
	bool    m_bLoop;
	bool	m_bRunning;

public:
	CAnimatorComponent(IEntity* pcOwner);

	int GetAnimator();
	int GetCurrentAnimation();
	double GetCurrentAnimationTime();
	float GetSpeed();
	bool IsLooping();
	bool IsRunning();

	void SetAnimator(int nAnimator);
	void SetAnimation(int nAnimation);
	void SetAnimationTime(double dTime);
	void SetSpeed(float fSpeed);
	void SetLoop(bool bLoop);
	void SetRunning(bool bRunning);

	CAnimatorComponent& operator=(CAnimatorComponent& cCopy);

	~CAnimatorComponent();
};

