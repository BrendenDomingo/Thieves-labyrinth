#pragma once
#include <d2d1_3.h>

#pragma comment(lib, "d2d1.lib")

class CButton
{
	RECT m_Collision;
	float m_fLeft, m_fRight, m_fTop, m_fBottom;
	D2D1_RECT_F m_fBox;
	const WCHAR* m_pButtonText;
	size_t m_nTextLength;
	bool m_bHovered;
	void(*m_pFunction)();

	// Checks to see if the cursor is over the button
	bool CheckMouseCollision();

public:
	CButton(void(*pFunction)(), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, 
		float left = 0.0f, float right = 0.1f, float top = 0.0f, float bottom = 0.1f);

	// Call this when the window is resized
	// fWidth: Swapchain width
	// fHeight: Swapchain height
	// fwWidth: Window Width
	// fwHeight: Window Height
	void ChangeBoundary(float fWidth, float fHeight, float fwWidth, float fwHeight);

	// Get the boundaries the button is being drawn in
	D2D1_RECT_F GetButtonBox();

	// Check for mouse clicks inside the button
	void Update();

	// Get the text the button contains
	const WCHAR* GetButtonText();

	// Get the length the button text is
	size_t GetTextLength();

	// MUST ONLY BE CALLED BY THE SLIDER CLASS
	// Changes the left and right boundaries to simulate "sliding"
	// fLeft: new left ratio between 0-1
	// fRight: new right ratio between 0-1
	void ChangeLeftRight(float fLeft, float fRight);

	// Returns wether or not the mouse is hovering over the button
	bool IsHovered();

	~CButton();
};

