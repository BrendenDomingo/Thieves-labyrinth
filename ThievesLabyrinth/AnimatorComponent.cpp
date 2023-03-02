#include "AnimatorComponent.h"
#include "EnumTypes.h"


CAnimatorComponent::CAnimatorComponent(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::ANIMATOR;
	m_nCurrentAnimation = 0;
	m_dCurrentTime = 0.0;
	m_fSpeed = 1.0f;

	m_bLoop = true;
	m_bRunning = false;
}

int CAnimatorComponent::GetAnimator()
{
	return m_nAnimator;
}

int CAnimatorComponent::GetCurrentAnimation()
{
	return m_nCurrentAnimation;
}

double CAnimatorComponent::GetCurrentAnimationTime()
{
	return m_dCurrentTime;
}

float CAnimatorComponent::GetSpeed()
{
	return m_fSpeed;
}

bool CAnimatorComponent::IsLooping()
{
	return m_bLoop;
}

bool CAnimatorComponent::IsRunning()
{
	return m_bRunning;
}

void CAnimatorComponent::SetAnimator(int nAnimator)
{
	m_nAnimator = nAnimator;
}

void CAnimatorComponent::SetAnimation(int nAnimation)
{
	m_nCurrentAnimation = nAnimation;
	m_bRunning = true;
}

void CAnimatorComponent::SetAnimationTime(double dTime)
{
	m_dCurrentTime = dTime;
}

void CAnimatorComponent::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CAnimatorComponent::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;
}

void CAnimatorComponent::SetRunning(bool bRunning)
{
	m_bRunning = bRunning;
}

CAnimatorComponent & CAnimatorComponent::operator=(CAnimatorComponent & cCopy)
{
	m_nAnimator = cCopy.GetAnimator();
	m_nCurrentAnimation = cCopy.GetCurrentAnimation();
	m_dCurrentTime = cCopy.GetCurrentAnimationTime();
	m_fSpeed = cCopy.m_fSpeed;

	return *this;
}

CAnimatorComponent::~CAnimatorComponent()
{
}
