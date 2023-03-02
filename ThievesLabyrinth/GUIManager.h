#pragma once
#include "System.h"
#include <vector>
#include <dxgi1_2.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "HUD.h"

#pragma comment(lib, "d2d1.lib")

class CGUIManager : public ISystem
{
	ID2D1Factory4*				m_pd2dFactory;
	ID2D1Device3*				m_pd2dDevice;
	ID2D1DeviceContext3*		m_pd2dContext;
	ID2D1HwndRenderTarget*		m_pd2dHRT;
	ID2D1SolidColorBrush*		m_pd2dColorBrush;
	ID2D1SolidColorBrush*		m_pd2dColorBrush2;
	ID2D1SolidColorBrush*		m_pd2dColorBrush3;
	ID2D1SolidColorBrush*		m_pd2dColorBrush4;
	ID2D1SolidColorBrush*		m_pd2dColorBrush5;
	ID2D1SolidColorBrush*		m_pd2dColorBrush6;
	IDXGIDevice*				m_pdxgDevice;
	IDXGISurface*				m_pdxgSurface;
	ID2D1RenderTarget*			m_pd2dRT;
	ID2D1Bitmap1*				m_pd2dBitmap;
	ID3D11Device*				m_pd3dDevice;
	static CHUD*				m_pcHud;
	static bool					m_bControls;
	static bool					m_bFade;
	RECT m_rc;

	// Writing text stuff
	IDWriteFactory3*			m_pdwWriteFactory;
	IDWriteTextFormat*			m_pdwTextFormat;
	IDWriteTextFormat*			m_pdwTitleFormat;
	IDWriteTextFormat*			m_pdwHUDFormat;
	IDWriteTextFormat*			m_pdwInventoryFormat;

	// Menu stuff
	static std::vector<CMenu*>	m_menus;
	static unsigned int			m_nCurrent;
	float						m_fWidth, m_fHeight;
	static bool					m_bShowInventory;
	static bool					m_bStartScroll;

	// Logo Stuff
	ID2D1SpriteBatch*			m_pd2dLogoBatch;
	std::vector<ID2D1Bitmap1*>	m_LogoSprites;
	ID2D1Bitmap1*				m_pd2dTitleSprite;

	// Only called in the constructor, but it sets up stuff for the inventory menu
	// pd3dDevice: Direct3D device to create textures
	// pd2dContext: Direct2D context to create sprites and spritebatches
	// fWidth: Swapchain width
	// fHeight: Swapchain Height
	// fwWidth: Window Width
	// fwHight: Window Height
	void SetHudMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight, float fwWidth, float fwHeight);

	// Called in case a new menu is needed
	// minimumMenu: The menu that needs to be made at the bare minimum
	void CreateMenus(int minimumMenu);

public:
	CGUIManager(ID3D11Device* m_pd3dDevice, IDXGISwapChain* _m_pdxgSwapChain, ID3D11Texture2D* pd3dTexture);

	// Draws everything necessary
	void Render();

	// Add a button to the specified menu
	// If the menu doesn't exist, it will create the menu
	// menu: Menu to add to
	// pFunction: Function that is called when a button is clicked
	// buttonText: Text displayed on the button
	// width: Swapchain width
	// height: Swapchain Height
	// wWidth: Window Width
	// wHeight: Window Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively 
	void AddButtonToMenu(int menu, void(*pFunction)(), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, float left = 0.0f, float right = 0.0f, float top = 0.0f, float bottom = 0.0f);

	// Add a slider to the specified menu
	// If the menu doesn't exist, it will create the menu
	// NOTE: Will be called when creating the options menus ONLY
	// menu: Menu to add to
	// nType: What volume will be affected in which menu
	// width: Swapchain width
	// height: Swapchain Height
	// wWidth: Window Width
	// wHeight: Window Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void AddSliderToMenu(int menu, int nVary, int nType, int width, int height, float wWidth, float wHeight, 
		float left, float right, float top, float bottom);

	// Give a title to a menu
	// If the menu doesn't exist, it will create the menu
	// menu: Menu to add to
	// text: Text to show
	// width: Swapchain width
	// height: Swapchain Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void SetTitleToMenu(int menu, const WCHAR* title_text, int width, int height, 
		float left, float right, float top, float bottom);

	// Adds text to a menu, meant for describing sliders
	// If the menu doesn't exist, it will create the menu
	// NOTE: Will be called when creating volume sliders
	// menu: Menu to add to
	// text: Text to show
	// width: Swapchain width
	// height: Swapchain Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void AddDescriptionToMenu(int menu, const WCHAR* text, int width, int height,
		float left, float right, float top, float bottom);

	// Call this function when the window has been resized
	// pdxgSwapChain: Swap chain used for rendering in 3d
	// necessary to make new render target view
	// fwWidth: Window width
	// fwHeight: Window Height
	void WindowResizeEvent(IDXGISwapChain* pdxgSwapChain, ID3D11Texture2D* pd3dTexture, float fwWidth, float fwHeight);

	// Called to change current menu rendered
	// Only works if there can be that window
	// menu: Menu to change to
	void ChangeCurrentMenu(int menu);

	// Releases 2D assets for resizing
	void Release2D();

	// Initializes 2D assets after resizing
	void ReInitialize2D(IDXGISwapChain* pdxgSwapChain, ID3D11Texture2D* pd3dTexture);

	// This is probably still being called in the Event Manager
	// It doesn't need to be, but let me know if you want it gone
	// fCurrHealth: Player's current health
	// fMaxHealth: Player's max health
	// fCurrMana: Player's current mana
	// fMaxMana: Player's max mana
	static void SetPlayerValues(float fCurrHealth = 0.0f, float fMaxHealth = 0.0f, float fCurrMana = 0.0f, float fMaxMana = 0.0f);

	// Allows the HUD to create the Minimap of the current level
	// cMap: Level layout defined in the level constructor
	static void SetMiniMap(std::vector<std::vector<Spot>> cMap);

	// Tells the HUD to display the objective of the game
	// Only works when a new game is started
	static void NewGame();

	// Sets the control scheme to show in the options menus
	// bControls: Set true for Mouse, false for Keyboard
	static void SetControls(bool bControls);

	// Called so we don't get out of the Options Menu prematurely
	int GetMenuState();

	// Updates all the UI items
	static void Update();

	// Informs whether the logo displaying
	// (GP Games, Full Sail University, etc.)
	// Are done rendering
	static bool FadingDone();

	// Resets the scroll effect for the credits
	static void RestartScroll();

	// Renders Logos to the screen
	void RenderLogos();

	// Changes the text on the win/loss screens
	// depending on the number of enemies killed
	// and items collected
	static void UpdateEndScenario(int nKilled, int nItems);

	~CGUIManager();
};

