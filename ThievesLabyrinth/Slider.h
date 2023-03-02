#pragma once
#include "Button.h"

class CSlider
{
	static bool m_bSMaC, m_bSMuC, m_bSSC, m_bPMaC, m_bPMuC, m_bPSC, m_bSG, m_bPG;
	static void SMaC();
	static void SMuC();
	static void SSC();
	static void PMaC();
	static void PMuC();
	static void PSC();
	static void SG();
	static void PG();

	float m_fValue, m_fWidth, m_fHeight, m_fwWidth, m_fwHeight, m_fLeft, m_fRight, m_fTop, m_fBottom;
	float m_fBoundsMin, m_fBoundsMax;
	int m_nVary, m_nType, m_nMenu;
	void ChangeSliderValue();
	void ChangePosition();
	D2D1_RECT_F m_rBox;
public:
	CButton* m_pcButtonToMove;
	CSlider(int menu, int nVary, int nType, float fWidth, float fHeight, float fwWidth, float fwHeight, 
		float fLeft, float fRight, float fTop, float fBottom);

	// Slider update function
	void Update();

	// Resize Event for the sliders
	// width: Swapchain width
	// height: Swapchain height
	// wWidth: Window width
	// wHeight: Window Height
	void ResizeEvent(float width, float height, float wWidth, float wHeight);

	// Get the slider box that the boundaries are 'tested' with
	D2D1_RECT_F	GetBox();

	// Get the value of the slider between 0-1
	float GetValue();
	~CSlider();
};

