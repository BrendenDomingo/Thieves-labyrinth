#include "Stats.h"
#include "EnumTypes.h"



CStats::CStats(IEntity* pcOwner) : IComponent(pcOwner)
{
}

CStats::CStats(IEntity* pcOwner, float fHP, float fMP, float fMovementSpeed, float fBaseDamage, float fAttackSpeed) : IComponent(pcOwner)
{
	m_fHP = fHP;
	m_fMaxHP = fHP;
	m_fMP = fMP;
	m_fMaxMP = fMP;
	m_fMovementSpeed = fMovementSpeed;
	m_fBaseDamage = fBaseDamage;
	m_fAttackSpeed = fAttackSpeed;

	m_nComponentType = eComponent::STATS;
}

float CStats::GetHP()
{
	return m_fHP;
}

float CStats::GetMaxHP()
{
	return m_fMaxHP;
}

float CStats::GetMP()
{
	return m_fMP;
}

float CStats::GetMaxMP()
{
	return m_fMaxMP;
}

float CStats::GetMovementSpeed()
{
	return m_fMovementSpeed;
}

float CStats::GetBaseDamage()
{
	return m_fBaseDamage;
}

float CStats::GetAttackSpeed()
{
	return m_fAttackSpeed;
}

void CStats::SetHP(float nHP)
{
	m_fHP = nHP;
	if (m_fHP > m_fMaxHP)
	{
		m_fHP = m_fMaxHP;
	}
}

void CStats::SetMaxHP(float fMaxHP)
{
	m_fMaxHP = fMaxHP;
	if (m_fHP > m_fMaxHP)
	{
		m_fHP = m_fMaxHP;
	}
}

void CStats::SetMP(float nMP)
{
	m_fMP = nMP;
	if (m_fMP > m_fMaxMP)
	{
		m_fMP = m_fMaxMP;
	}
}

void CStats::SetMaxMP(float fMaxMP)
{
	m_fMaxMP = fMaxMP;
	if (m_fMP > m_fMaxMP)
	{
		m_fMP = m_fMaxMP;
	}
}

void CStats::SetMovementSpeed(float nMovementSpeed)
{
	m_fMovementSpeed = nMovementSpeed;
}

void CStats::SetBaseDamage(float nDamage)
{
	m_fBaseDamage = nDamage;
}

void CStats::SetAttackSpeed(float fAttackSpeed)
{
	m_fAttackSpeed = fAttackSpeed;
}

CStats & CStats::operator=(CStats & cCopy)
{
	m_fHP = cCopy.GetHP();
	m_fMP = cCopy.GetMP();
	m_fMaxHP = cCopy.GetMaxHP();
	m_fMaxMP = cCopy.GetMaxMP();

	m_fMovementSpeed = cCopy.GetMovementSpeed();
	m_fBaseDamage = cCopy.GetBaseDamage();
	m_fAttackSpeed = cCopy.GetAttackSpeed();

	return *this;
}


CStats::~CStats()
{
}
