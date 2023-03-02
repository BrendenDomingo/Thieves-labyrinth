#pragma once
#include "Inventory.h"
#include "EnumTypes.h"



CInventory::CInventory(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::INVENTORY;
	m_nActive = m_nCurrConsume = m_nLastActive = 0;
	m_tPassive = m_tLastPassive = { 0, 0 };
	m_pnConsumables = new int[eConsumableItems::COUNT];
	for (int i = 0; i < eConsumableItems::COUNT; i++)
	{
		m_pnConsumables[i] = 0;
	}
	m_bChangePassive = m_bChangeActive = false;
}

CMath::TVECTOR2 CInventory::GetPassive()
{
	return m_tPassive;
}

int CInventory::GetActive()
{
	return m_nActive;
}

CMath::TVECTOR2 CInventory::GetLastPassive()
{
	return m_tLastPassive;
}

int CInventory::GetLastActive()
{
	return m_nLastActive;
}

bool CInventory::ChangePassive()
{
	if (m_bChangePassive)
	{
		m_bChangePassive = false;
		return true;
	}
	return false;
}

CMath::TVECTOR2 CInventory::TChangePassive()
{
	return m_tChangePassive;
}

bool CInventory::ChangeActive()
{
	if (m_bChangeActive)
	{
		m_bChangeActive = false;
		return true;
	}
	return false;
}

void CInventory::GetConsumables(int** nSet)
{
	*nSet = m_pnConsumables;
}

int CInventory::GetCurrentConsumable()
{
	return m_nCurrConsume;
}

void CInventory::AddConsumableItem(int nItem)
{
	if (nItem > eConsumableItems::NONE && nItem < eConsumableItems::COUNT)
	{
		m_pnConsumables[nItem]++;
		if (m_nCurrConsume == eConsumableItems::NONE)
		{
			SetCurrentConsumable(nItem);
		}
	}
}

void CInventory::Previous()
{
	int nCheck = m_nCurrConsume - 1;
	if (nCheck <= eConsumableItems::NONE)
	{
		nCheck = eConsumableItems::COUNT - 1;
	}

	if (m_pnConsumables[nCheck] <= 0)
	{
		int nHold = nCheck;
		bool bChange = true;
		for (int i = nHold - 1; i > eConsumableItems::NONE; i--)
		{
			if (m_pnConsumables[i] > 0)
			{
				bChange = false;
				nCheck = i;
				break;
			}
		}
		if (bChange)
		{
			for (int i = eConsumableItems::COUNT - 1; i > nHold; i--)
			{
				if (m_pnConsumables[i] > 0)
				{
					bChange = false;
					nCheck = i;
					break;
				}
			}
			if (bChange)
			{
				nCheck = eConsumableItems::NONE;
			}
		}
	}

	SetCurrentConsumable(nCheck);
}

void CInventory::Next()
{
	int nCheck = m_nCurrConsume + 1;
	if (nCheck >= eConsumableItems::COUNT)
	{
		nCheck = eConsumableItems::NONE + 1;
	}

	if (m_pnConsumables[nCheck] <= 0)
	{
		int nHold = nCheck;
		bool bChange = true;
		for (int i = nHold + 1; i < eConsumableItems::COUNT; i++)
		{
			if (m_pnConsumables[i] > 0)
			{
				bChange = false;
				nCheck = i;
				break;
			}
		}
		if (bChange)
		{
			for (int i = eConsumableItems::NONE + 1; i < nHold; i++)
			{
				if (m_pnConsumables[i] > 0)
				{
					bChange = false;
					nCheck = i;
					break;
				}
			}
			if (bChange)
			{
				nCheck = eConsumableItems::NONE;
			}
		}
	}

	SetCurrentConsumable(nCheck);
}

int CInventory::UseCurrentConsumable()
{
	if (m_nCurrConsume > eConsumableItems::NONE && m_nCurrConsume < eConsumableItems::COUNT)
	{
		if (m_pnConsumables[m_nCurrConsume] > 0)
		{
			int nReturn = m_nCurrConsume;
			m_pnConsumables[m_nCurrConsume]--;
			if (m_pnConsumables[m_nCurrConsume] <= 0)
			{
				// Do something so that the player doesn't do something stupid
				int nCurrent = m_nCurrConsume;
				bool bCheck = true;
				for (int i = nCurrent + 1; i < eConsumableItems::COUNT; i++)
				{
					if (m_pnConsumables[i] > 0)
					{
						SetCurrentConsumable(i);
						bCheck = false;
						break;
					}
				}
				if (bCheck)
				{
					for (int i = eConsumableItems::NONE + 1; i < nCurrent; i++)
					{
						if (m_pnConsumables[i] > 0)
						{
							SetCurrentConsumable(i);
							bCheck = false;
							break;
						}
					}
					if (bCheck)
					{
						SetCurrentConsumable(eConsumableItems::NONE);
					}
				}
			}
			return nReturn;
		}
	}
	return 0;
}

void CInventory::SetCurrentConsumable(int nItem)
{
	// Make sure it is legal for arrays
	if (nItem < eConsumableItems::COUNT && nItem > -1)
	{
		// See if there are items in the place we are switching to
		if (m_pnConsumables[nItem] > 0)
		{
			m_nCurrConsume = nItem;
		}
		// Make sure the program didn't detect this issue sooner
		else if (nItem == eConsumableItems::NONE)
		{
			m_nCurrConsume = nItem;
		}
		// Go to the next available item slot
		else
		{
			int nCurrent = nItem;
			bool bCheck = true;
			for (int i = nCurrent + 1; i < eConsumableItems::COUNT; i++)
			{
				if (m_pnConsumables[i] > 0)
				{
					m_nCurrConsume = i;
					bCheck = false;
					break;
				}
			}
			if (bCheck)
			{
				for (int i = eConsumableItems::NONE + 1; i < nCurrent; i++)
				{
					if (m_pnConsumables[i] > 0)
					{
						m_nCurrConsume = i;
						bCheck = false;
						break;
					}
				}
				if (bCheck)
				{
					m_nCurrConsume = 0;
				}
			}
		}
	}
}

void CInventory::SetPassive(int nPassive, int nSlot)
{
	m_tLastPassive = m_tPassive;
	m_tPassive.mData[nSlot] = (float)nPassive;
	m_bChangePassive = true;
	m_tChangePassive.mData[nSlot] = 1;
}

void CInventory::SetActive(int nActive)
{
	m_nLastActive = m_nActive;
	m_nActive = nActive;
	m_bChangeActive = true;
}

void CInventory::ResetChangePassive()
{
	m_tChangePassive = { 0,0 };
}

CInventory & CInventory::operator=(CInventory& cCopy)
{
	//Be safe and get rid of this
	if (m_pnConsumables)	delete m_pnConsumables;
	cCopy.GetConsumables(&m_pnConsumables);
	m_tPassive = cCopy.GetPassive();
	m_nActive = cCopy.GetActive();
	m_nCurrConsume = cCopy.GetCurrentConsumable();
	return *this;
}

CInventory::~CInventory()
{
	if (m_pnConsumables)	delete m_pnConsumables;
	m_pnConsumables = nullptr;
}
