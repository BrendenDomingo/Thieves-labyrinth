#pragma once
#include <d2d1_3.h>
#include <d3d11.h>
#include <dwrite_3.h>
#include "Menu.h"
#include "Level.h"
#include "Math.h"
#include "Entity.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class CHUD
{
	ID2D1SpriteBatch*	m_pd2dSpriteBatch, *m_pd2dInventoryBatch;
	float m_fCurrHealth, m_fMaxHealth, m_fCurrMana, m_fMaxMana;
	CMath::TVECTOR3 m_fMaxTime;
	CMath::TVECTOR3 m_fCurrTime;
	CMath::TVECTOR3 m_fCurrBuffTime;
	CMath::TVECTOR3 m_fMaxBuffTime;
	float m_fWidth, m_fHeight;
	CMath::TVECTOR2 m_tCurrPass;
	int m_nCurrActive;
	static int m_nCurrConsume;
	static bool m_bChange;
	bool m_bRenderMenu, m_bDisplaySwap;
	int* m_pnConsume;
	size_t m_nEnemyCount;
	CMenu* m_pcMenu;
	double m_dTime;
	std::vector<std::vector<Spot>> m_cMap;
	std::vector<std::vector<D2D1_RECT_F>> m_rMap;
	std::vector<D2D1_RECT_U> m_RectPos;
	std::vector<D2D1_RECT_U> m_ButtonPos;
	std::vector<std::wstring> m_ItemDescriptions;
	ID2D1Bitmap1*	m_pd2dTempSprite, *m_pd2dInventorySprite;

	////////////////
	//Menu Functions
	////////////////

	static void UseHealth();
	static void UseMana();
	static void UseDamage();
	static void UseHaste();
	static void UseResist();
	static void Nothing();

	// Resizes the Minimap in the Resize event
	// fWidth: Swapchain width
	// fHeight: Swapchain height
	void ResizeMiniMap(float fWidth, float fHeight);
public:
	CHUD(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight);

	// Draws the HUD, without drawing the inventory
	// Calls BeginDraw and EndDraw
	// pd2dContext: Direct2D context used to draw sprites
	// pd2dRT: Render Target used to write text and draw boxes
	// pd2dBrushie: Brush used with the Render Target
	// fWidth: Swapchain Width
	// fHeight: Swapchain Height
	void DrawBaseHUD(ID2D1DeviceContext3* pd2dContext, ID2D1RenderTarget* pd2dRT, ID2D1SolidColorBrush* pd2dBrushie, 
		ID2D1SolidColorBrush* pd2dBrushie2, ID2D1SolidColorBrush* pd2dBrushie3, float fWidth, float fHeight);

	// Sets up the menu the Inventory utilizes in-game
	// pd3dDevice: Direct3D device to create sprite
	// pd2dContext: Direct2D context to create spritebatch
	// fWidth: Swapchain Width
	// fHeight: Swapchain Height
	// fwWidth: Window Width
	// fwHeight: Window Height
	void SetMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight, float fwWidth, float fwHeight);

	// Draws the Inventory
	// Calls BeginDraw and EndDraw
	// pd2dContext: Direct2D context used to draw sprites
	// pd2dRT: Render Target used to write text and draw boxes
	// pdxwTextFormat: Text format used to set text specific font, size and orientation,
	// used to show how many consumable items are left
	// pdxwTextFormat: Text format used to set text specific font, size and orientation,
	// used to render the title of the inventory menu
	// pd2dBrushie: Brush with transparency for a background box
	// pd2dBrushie2: Brush used to show off current room in the minimap
	// pd2dBrushie3: Brush used to render text for how many items remain and visited rooms
	// pd2dBrushie4: Brush used to show what rooms haven't been visited
	// fWidth: Swapchain Width
	// fHeight: Swapchain Height
	void DrawInventory(ID2D1DeviceContext3* pd2dContext, ID2D1RenderTarget* pd2dRT, 
		IDWriteFactory* pdwWriteFactory, IDWriteTextFormat* pdxwTextFormat, IDWriteTextFormat* pdxwTitleFormat,
		ID2D1SolidColorBrush* pd2dBrushie, ID2D1SolidColorBrush* pd2dBrushie2, ID2D1SolidColorBrush* pd2dBrushie3, ID2D1SolidColorBrush* pd2dBrushie4,
		ID2D1SolidColorBrush* pd2dBrushie5, float fWidth, float fHeight);

	// Resize Event
	// fWidth: Swapchain Width
	// fHeight: Swapchain Height
	// fwWidth: Window Width
	// fwHeight: Window Height
	void ResizeItems(float fWidth, float fHeight, float fwWidth, float fwHeight);

	// Updates Player values and Inventory Menu
	void Update(bool bInventory);

	// This is probably still being called in the Event Manager
	// It doesn't need to be, but let me know if you want it gone
	// fCurrHealth: Player's current health
	// fMaxHealth: Player's max health
	// fCurrMana: Player's current mana
	// fMaxMana: Player's max mana
	void SetPlayerValues(float fCurrHealth = 0.0f, float fMaxHealth = 0.0f, float fCurrMana = 0.0f, float fMaxMana = 0.0f);

	// Temporary value that renders the current Health and Mana values of the player
	// Waiting for max values to do health and mana bars
	// pd2dRT: Render Target used to write text and draw boxes
	// pdxwTextFormat: Text format used to set text specific font, size and orientation
	// pxdwTextFormat: Format to write text to the screen for a new game start only
	// pd2dBrushie: Brush used with the Render Target
	// fWidth: Swapchain Width
	// fHeight: Swapchain Height
	void WriteHealthAndMana(ID2D1RenderTarget* pd2dRT, IDWriteFactory* pdwWriteFactory, IDWriteTextFormat* pdxwTextFormat, IDWriteTextFormat* pdxwTitleFormat,
							ID2D1SolidColorBrush* pd2dBrushie, float fWidth, float fHeight, bool bControls = false);

	// Sets up the Minimap to be used in the Inventory Menu
	// Called when the GUIManager wants to call it
	// cMap: Level layout defined in the CLevel constructor
	void SetMiniMap(std::vector<std::vector<Spot>> cMap);

	// Sets the timer to 5 seconds to display
	// the game objective
	void NewGame();

	static IEntity* m_pcHoveredEntity;

	~CHUD();
};

