#pragma once
#include "Component.h"

class CStats : public IComponent
{
	float		m_fHP;
	float		m_fMaxHP;
	float		m_fMP;
	float		m_fMaxMP;
	float		m_fMovementSpeed;
	float		m_fBaseDamage;
	float		m_fAttackSpeed;

public:
	CStats(IEntity* pcOwner);

	CStats(IEntity* pcOwner, float nHP, float nMP, float fMovementSpeed, float nBaseDamage, float m_fAttackSpeed);

	/***************
	* Accessors
	***************/

	float GetHP();
	float GetMaxHP();
	float GetMP();
	float GetMaxMP();
	float GetMovementSpeed();
	float GetBaseDamage();
	float GetAttackSpeed();

	/***************;
	* Mutators
	***************/

	void SetHP(float fHP);
	void SetMaxHP(float fMaxHP);
	void SetMP(float fMP);
	void SetMaxMP(float fMaxMP);
	void SetMovementSpeed(float fMovementSpeed);
	void SetBaseDamage(float fDamage);
	void SetAttackSpeed(float fAttackSpeed);

	CStats& operator=(CStats& cCopy);

	~CStats();
};

