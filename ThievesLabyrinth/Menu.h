#pragma once
#include "Slider.h"
#include <vector>
#include <d3d11.h>

class CMenu
{
	ID2D1Bitmap1*		m_pd2dButtonSprite;
	ID2D1SpriteBatch*	m_pd2dSpriteBatch;
	const WCHAR*		m_pTitleText;
	D2D1_RECT_F			m_TitleBox;
	size_t				m_TitleLength;
	float m_fLeft, m_fRight, m_fTop, m_fBottom;
	std::vector<D2D1_RECT_F> m_BoundaryBoxSet;
	std::vector<D2D1_RECT_U> m_UnpressedBoxes;
	std::vector<D2D1_RECT_U> m_PressedBoxes;
	bool m_isOptions;
public:
	std::vector<CButton*> m_pcButtons;
	std::vector<CSlider*> m_pcSliders;
	std::vector<const WCHAR*> m_pDescriptionText;
	std::vector<D2D1_RECT_F> m_DescriptionBoxes;
	std::vector<size_t> m_DescriptionLength;
	CMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, bool isOptions = false);

	// pFunction: Function the button uses
	// buttonText: Text the button displays
	// width: Swapchain Width
	// height: Swapchain Height
	// wWidth: Window Width
	// wHeight: Window Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void AddButton(void(*pFunction)(void), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, 
		float left = 0.0f, float right = 0.0f, float top = 0.0f, float bottom = 0.0f);

	// Add a volume slider to the menu
	// menu: Menu of the specific slider
	// nType: What volume the slider changes
	// fwidth: Swapchain Width
	// fheight: Swapchain Height
	// fwWidth: Window Width
	// fwHeight: Window Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void AddSlider(int menu, int nVary, int nType, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom);

	// title_text: Text to render to the screen
	// width: Swapchain Width
	// height: Swapchain Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void MakeTitle(const WCHAR* title_text, int width, int height, float left, float right, float top, float bottom);

	// Called on the Window Resize Event
	// fwidth: Swapchain Width
	// fheight: Swapchain Height
	// fwWidth: Window Width
	// fwHeight: Window Height
	void ResizeItems(float fWidth, float fHeight, float fwWidth, float fwHeight);

	// Add text describing something somewhere
	// text: Text to render to the screen
	// width: Swapchain Width
	// height: Swapchain Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void AddDescriptionText(const WCHAR* text, int width, int height, float left, float right, float top, float bottom);

	// Get the length of the title
	size_t GetTitleLength();

	// Get the box the title sits in
	D2D1_RECT_F GetTitleBox();

	// Update the menu items
	void Update();

	// Get the Title Text
	const WCHAR* GetTitle();

	// Sets sprites for buttons and sliders
	void SetSprites();

	// Get the sprite for the buttons
	ID2D1Bitmap1*	GetButtonSprite();

	// Get the spritebatch for the buttons
	ID2D1SpriteBatch* GetSpriteBatch();

	// Informs if the menu is an options menu
	bool IsOptionsMenu();

	// Resizes Description Boxes
	// fWidth: Width of the swap chain
	// fHeight: Height of the swap chain
	void ResizeBoxes(float fWidth, float fHeight);

	~CMenu();
};

