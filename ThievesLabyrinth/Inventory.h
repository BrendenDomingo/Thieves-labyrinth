#pragma once
#include "Component.h"

class CInventory :
	public IComponent
{
	CMath::TVECTOR2 m_tPassive, m_tLastPassive, m_tChangePassive;
	int m_nActive, m_nCurrConsume;
	int* m_pnConsumables;
	bool m_bChangePassive, m_bChangeActive;
	int m_nLastActive;
public:
	CInventory(IEntity* pcOwner);

	/***************
	* Accessors
	***************/

	// Get the current passive item
	CMath::TVECTOR2		GetPassive();

	// Get the current active item
	int		GetActive();

	// Get the last passive item
	CMath::TVECTOR2		GetLastPassive();

	// Get the last active item
	int		GetLastActive();

	// Determine if we need to change the Passive item
	bool	ChangePassive();

	// Determine what passive item was changed
	CMath::TVECTOR2	TChangePassive();

	// Determine if we need to change the Active Item
	bool	ChangeActive();

	// Sets a pointer to the array of consumables
	// nSet: pointer address to set the array to
	void	GetConsumables(int** nSet);

	// Get the current consumable item
	int		GetCurrentConsumable();

	/***************
	* Mutators
	***************/

	// Add a consumable item to the count
	// If the item does not exist, this does nothing
	// If the current consumable is none, it swaps to that item
	void	AddConsumableItem(int nItem);

	// Changes the Current Consumable
	// to the previous Item Slot available
	void	Previous();

	// Changes the Current Consumable
	// to the next Item Slot available
	void	Next();

	// Uses the current consumable item
	// Returns what item to be used
	// If none can be used, returns 0
	int		UseCurrentConsumable();

	// Set the consumable item to be used
	// when UseCurrentConsumable is called
	// If none can be set, defaults to no item
	// nItem: Consumable item to set to
	void	SetCurrentConsumable(int nItem);

	// Sets what passive item is equipped
	// If the item does not exist, defaults to none
	// nPassive: Passive item to set to
	void	SetPassive(int nPassive, int nSlot);

	// Sets what active item is equipped
	// If the item does not exist, defaults to none
	// nActive: Active item to set to
	void	SetActive(int nActive);

	void	ResetChangePassive();

	CInventory & operator=(CInventory& cCopy);

	~CInventory();
};

