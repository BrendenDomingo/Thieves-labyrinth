#pragma once
#include "GUIManager.h"
#include "EnumTypes.h"
#include "InputManager.h"
#include "Time.h"
#include "WICTextureLoader.h"
#include "InputManager.h"

#include <string>

CHUD* CGUIManager::m_pcHud;
bool CGUIManager::m_bControls, CGUIManager::m_bFade, CGUIManager::m_bShowInventory, CGUIManager::m_bStartScroll;
std::vector<CMenu*> CGUIManager::m_menus;
unsigned int CGUIManager::m_nCurrent;

void CGUIManager::CreateMenus(int minimumMenu)
{
	if (minimumMenu >= m_menus.size())
	{
		while (minimumMenu >= m_menus.size())
		{
			CMenu *nMenu = nullptr;
			if (m_menus.size() == eMenu::START_CONTROLS || m_menus.size() == eMenu::PAUSE_CONTROLS)
			{
				nMenu = new CMenu(m_pd3dDevice, m_pd2dContext, true);
				nMenu->ResizeBoxes(m_fWidth, m_fHeight);
			}
			else
			{
				nMenu = new CMenu(m_pd3dDevice, m_pd2dContext);
			}

			m_menus.push_back(nMenu);
		}
	}
}

CGUIManager::CGUIManager(ID3D11Device* pd3dDevice, IDXGISwapChain* m_pdxgSwapChain, ID3D11Texture2D* pd3dTexture)
{
	m_nCurrent = eMenu::START;
	m_pd2dFactory = nullptr;
	m_pd2dDevice = nullptr;
	m_pd2dContext = nullptr;
	m_pd2dHRT = nullptr;
	m_pd2dColorBrush = nullptr;
	m_pd2dColorBrush2 = nullptr;
	m_pd2dColorBrush3 = nullptr;
	m_pd2dColorBrush4 = nullptr;
	m_pd2dColorBrush5 = nullptr;
	m_pd2dColorBrush6 = nullptr;
	m_pdxgDevice = nullptr;
	m_pdxgSurface = nullptr;
	m_pd2dBitmap = nullptr;
	m_pd2dRT = nullptr;
	m_pdwWriteFactory = nullptr;
	m_pdwTextFormat = nullptr;
	m_pdwTitleFormat = nullptr;
	m_pcHud = nullptr;
	m_pd2dTitleSprite = nullptr;
	m_bShowInventory = m_bControls = m_bFade = false;
	m_pd3dDevice = pd3dDevice;

	// Initialize D2DFactory to create device and swapchain
	HRESULT hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_pdxgDevice);
	D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_NONE };
#ifdef _DEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_ERROR;
#endif
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, &m_pd2dFactory);
	// Make D2D1Device3
	hr = m_pd2dFactory->CreateDevice(m_pdxgDevice, &m_pd2dDevice);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"D2D1CreateDevice", L"Error", MB_OK);
		exit(0);
	}
	// Make D2D1DeviceContext3
	// This is the only way to render sprites to the screen
	hr = m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dContext);
	m_pd2dContext->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);

	// Create render target for text rendering
	IDXGISurface* pdxgSurface = nullptr;
	FLOAT dpiX, dpiY;
	m_pd2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	hr = pd3dTexture->QueryInterface( __uuidof(IDXGISurface), reinterpret_cast<void**>(&pdxgSurface));
	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX, dpiY
		);
	hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(pdxgSurface, &props, &m_pd2dRT);

	D2D1_COLOR_F test;
	test.b = test.g = test.r = 0.0f;
	test.a = 0.5f;
	// Spell pig backwards, then say pretty colors
	hr = m_pd2dRT->CreateSolidColorBrush(test, &m_pd2dColorBrush);
	hr = m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold), &m_pd2dColorBrush2);
	hr = m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LimeGreen), &m_pd2dColorBrush3);
	hr = m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &m_pd2dColorBrush4);
	hr = m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pd2dColorBrush5);
	hr = m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pd2dColorBrush6);

	// Pain in the butt text rendering
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), (IUnknown**)&m_pdwWriteFactory);
	IDWriteFontSetBuilder* pFontSetBuilder = nullptr;
	hr = m_pdwWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
	std::vector<std::wstring> filePaths;

	filePaths.push_back(L"..\\Assets\\Fonts\\DUNGRG__.TTF");
	filePaths.push_back(L"..\\Assets\\Fonts\\bloodcrow.TTF");

	for (size_t i = 0; i < filePaths.size(); i++)
	{
		IDWriteFontFile* pFontFile = nullptr;
		hr = m_pdwWriteFactory->CreateFontFileReference(filePaths[i].c_str(), nullptr, &pFontFile);

		BOOL isSupported;
		DWRITE_FONT_FILE_TYPE fileType;
		UINT32 numberOfFonts;
		hr = pFontFile->Analyze(&isSupported, &fileType, nullptr, &numberOfFonts);

		if (isSupported)
		{
			for (UINT32 fontIndex = 0; fontIndex < numberOfFonts; fontIndex++)
			{
				IDWriteFontFaceReference* pFontFaceReference = nullptr;
				hr = m_pdwWriteFactory->CreateFontFaceReference(pFontFile, fontIndex, DWRITE_FONT_SIMULATIONS_NONE, &pFontFaceReference);

				if (SUCCEEDED(hr))
				{
					hr = pFontSetBuilder->AddFontFaceReference(pFontFaceReference);
				}
			}
		}
	}

	IDWriteFontSet* pFontSet = nullptr;
	hr = pFontSetBuilder->CreateFontSet(&pFontSet);

	IDWriteFontCollection1* pFontCollection = nullptr;

	hr = m_pdwWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &pFontCollection);

	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Dungeon",				// Font family name.
		pFontCollection,        // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		25.0f,
		L"en-us",
		&m_pdwTextFormat
	);

	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Blood Crow",			// Font family name.
		pFontCollection,        // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_ULTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		40.0f,
		L"en-us",
		&m_pdwTitleFormat
	);

	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Dungeon",				// Font family name.
		pFontCollection,        // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		25.0f,
		L"en-us",
		&m_pdwHUDFormat
	);

	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Dungeon",				// Font family name.
		pFontCollection,        // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		25.0f,
		L"en-us",
		&m_pdwInventoryFormat
	);

	pFontCollection->Release();
	pFontSet->Release();
	pFontSetBuilder->Release(); 
	//pFontFile->Release();

	// This centers all text to the center of their specific box
	m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	// It sucks that this has to happen for different Font sizes
	m_pdwTitleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pdwTitleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	// And for different positioning of text
	m_pdwHUDFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_pdwHUDFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	// This is why I wasn't the biggest fan of English
	m_pdwInventoryFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	m_pdwInventoryFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	// Hold onto swapchain width and height for drawing the hud
	DXGI_SWAP_CHAIN_DESC sd;
	m_pdxgSwapChain->GetDesc(&sd);
	m_fWidth = (float)sd.BufferDesc.Width;
	m_fHeight = (float)sd.BufferDesc.Height;

	// So this is used to create the Bitmap to render sprites to
	DXGI_SURFACE_DESC sDesc;
	D2D1_BITMAP_PROPERTIES1 bitProp;
	IDXGISurface2* texSurface = nullptr;
	ZeroMemory(&sDesc, sizeof(DXGI_SURFACE_DESC));
	ZeroMemory(&bitProp, sizeof(D2D1_BITMAP_PROPERTIES1));
	pdxgSurface->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface); // should always succeed
	texSurface->GetDesc(&sDesc);
	bitProp.pixelFormat = { sDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED };
	bitProp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	// create compatible bitmap to main DXGI surface (for high speed sprite rendering)
	if (FAILED(m_pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &m_pd2dBitmap)))
	{
		// This will severely upset me if it happens
		MessageBox(nullptr, L"Failed to make a bitmap to render menu stuff to", L"Error", MB_OK);
		texSurface->Release();
		exit(0);
	}
	texSurface->Release();
	pdxgSurface->Release();
	// THAT WAS TOO LONG!!
	m_pcHud = new CHUD(pd3dDevice, m_pd2dContext, m_fWidth, m_fHeight);
	SetHudMenu(pd3dDevice, m_pd2dContext, m_fWidth, m_fHeight, m_fWidth, m_fHeight);

	// Create Logos to render in the beginning
	std::vector<std::wstring> logoFiles;
	logoFiles.push_back(L"../Assets/Textures/UI/Full_Sail_Logo.jpg");
	logoFiles.push_back(L"../Assets/Textures/UI/GPGames_Logo.png");
	logoFiles.push_back(L"../Assets/Textures/UI/ElseIfStudio_Logo.png");
	m_pd2dContext->CreateSpriteBatch(&m_pd2dLogoBatch);
	D2D1_RECT_F offscreen = { 0, };
	m_pd2dLogoBatch->AddSprites(1, &offscreen);

	for (size_t i = 0; i < logoFiles.size(); i++)
	{
		ID3D11Texture2D* fromDisk = nullptr;
		ID3D11ShaderResourceView* ignored = nullptr;
		//Sprite sheet will use this in the future
		if (FAILED(DirectX::CreateWICTextureFromFile(pd3dDevice, logoFiles[i].c_str(), (ID3D11Resource**)&fromDisk, &ignored)))
		{
			MessageBox(nullptr, L"Failed to load texture for logo", L"Error", MB_OK);
			if (fromDisk)	fromDisk->Release();
			if (ignored)	ignored->Release();
			exit(0);
		}
		IDXGISurface2* texSurface = nullptr;
		fromDisk->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface);

		DXGI_SURFACE_DESC sDesc;
		D2D1_BITMAP_PROPERTIES1 bitProp;
		ZeroMemory(&sDesc, sizeof(DXGI_SURFACE_DESC));
		ZeroMemory(&bitProp, sizeof(D2D1_BITMAP_PROPERTIES1));
		texSurface->GetDesc(&sDesc);
		bitProp.pixelFormat = { sDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED };
		bitProp.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
		bitProp.dpiX = bitProp.dpiY = 96.0f; // THIS MUST NOT BE CHANGED!!
		// I don't know why, but not using this specific float can easily break EVERYTHING
		// If you put zero (which *some* functions seem to say is a-ok!) it will draw nothing!!!
		// You also get no errors, I guess it just assumes you are fine with drawing nothing at all.
		ID2D1Bitmap1* Sprite = nullptr;
		m_pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &Sprite);
		texSurface->Release();
		if (fromDisk) { fromDisk->Release(); fromDisk = nullptr; }
		if (ignored) { ignored->Release();	 ignored = nullptr; }
		m_LogoSprites.push_back(Sprite);
	}

	// Set up for the logo
	{
		ID3D11Texture2D* annoying = nullptr;
		ID3D11ShaderResourceView* screwYou = nullptr;
		//Sprite sheet will use this in the future
		if (FAILED(DirectX::CreateWICTextureFromFile(pd3dDevice, L"../Assets/Textures/UI/Logo.png", (ID3D11Resource**)&annoying, &screwYou)))
		{
			MessageBox(nullptr, L"Failed to load texture for logo", L"Error", MB_OK);
			if (annoying)	annoying->Release();
			if (screwYou)	screwYou->Release();
			exit(0);
		}
		IDXGISurface2* texSurface = nullptr;
		annoying->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface);

		DXGI_SURFACE_DESC sDesc;
		D2D1_BITMAP_PROPERTIES1 bitProp;
		ZeroMemory(&sDesc, sizeof(DXGI_SURFACE_DESC));
		ZeroMemory(&bitProp, sizeof(D2D1_BITMAP_PROPERTIES1));
		texSurface->GetDesc(&sDesc);
		bitProp.pixelFormat = { sDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED };
		bitProp.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
		bitProp.dpiX = bitProp.dpiY = 96.0f; // THIS MUST NOT BE CHANGED!!
		// I don't know why, but not using this specific float can easily break EVERYTHING
		// If you put zero (which *some* functions seem to say is a-ok!) it will draw nothing!!!
		// You also get no errors, I guess it just assumes you are fine with drawing nothing at all.
		m_pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &m_pd2dTitleSprite);
		texSurface->Release();
		if (annoying) { annoying->Release(); annoying = nullptr; }
		if (screwYou) { screwYou->Release(); screwYou = nullptr; }
	}
}

void CGUIManager::Update()
{
	if (CInputManager::GetKey(VK_TAB))
	{
		m_bShowInventory = true;
	}
	else
	{
		m_bShowInventory = false;
	}
	if (m_nCurrent == eMenu::GAME)
	{
		m_pcHud->Update(m_bShowInventory);
	}
	else
	{
		if(m_menus.size() > 0)
			m_menus[m_nCurrent]->Update();
	}
}

bool CGUIManager::FadingDone()
{
	return m_bFade;
}

void CGUIManager::RestartScroll()
{
	m_bStartScroll = true;
}

void CGUIManager::RenderLogos()
{
	static int nCurrentLogo = 0;
	static float fTimer = 3.0f;
	static float fFadeIn = 0.0f - CTime::GetDeltaF();
	float clamped;
	if (fFadeIn < 1.0f)
	{
		fFadeIn += CTime::GetDeltaF();
		clamped = std::fmaxf(0.0f, std::fminf(1.0f, fFadeIn));
	}
	else if (CInputManager::GetAnyKeyDown() >= 0)
	{
		clamped = 0.0f;
		fTimer = 3.0f;
		fFadeIn = 0.0f;
		nCurrentLogo++;
		if (nCurrentLogo >= m_LogoSprites.size())
		{
			m_bFade = true;
			return;
		}
	}
	else
	{
		fTimer -= CTime::GetDeltaF();
		clamped = std::fminf(1.0f, std::fmaxf(0, fTimer));
		if (fTimer <= 0.0f)
		{
			clamped = 0.0f;
			fTimer = 3.0f;
			fFadeIn = 0.0f;
			nCurrentLogo++;
			if (nCurrentLogo >= m_LogoSprites.size())
			{
				m_bFade = true;
				return;
			}
		}
	}

	// Covers entire window
	D2D1_RECT_F box = D2D1::RectF(0, 0, m_fWidth, m_fHeight);

	// Logo fading based on a clamped value between 0 and 1
	D2D1_COLOR_F color;
	color.a = color.r = color.g = color.b = clamped;
	m_pd2dLogoBatch->SetSprites(0, 1, &box, nullptr, &color);

	m_pd2dContext->SetTarget(m_pd2dBitmap);
	m_pd2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	m_pd2dContext->BeginDraw();
	m_pd2dContext->DrawSpriteBatch(m_pd2dLogoBatch, m_LogoSprites[nCurrentLogo]);
	m_pd2dContext->EndDraw();
	m_pd2dContext->SetTarget(nullptr);
}

void CGUIManager::UpdateEndScenario(int nKilled, int nItems)
{
	static std::wstring kill;
	static std::wstring itemsCollected;
	static std::wstring timeElapsed;
	unsigned totalTime = (unsigned)CTime::GetTotalTime();
	kill.clear();
	itemsCollected.clear();
	timeElapsed.clear();
	kill += L"Enemies Killed: " + std::to_wstring(nKilled);
	itemsCollected += L"Items Collected: " + std::to_wstring(nItems);
	timeElapsed += L"Time Elapsed: " + std::to_wstring(totalTime / 60) + L":";
	if (totalTime % 60 < 10)
	{
		timeElapsed += L"0";
	}
	timeElapsed += std::to_wstring(totalTime % 60);

	if (m_menus[eMenu::WIN]->m_pDescriptionText.size() >= 3 && m_menus[eMenu::LOSE]->m_pDescriptionText.size() >= 3)
	{
		m_menus[eMenu::WIN]->m_pDescriptionText[0] = m_menus[eMenu::LOSE]->m_pDescriptionText[0] = kill.c_str();
		m_menus[eMenu::WIN]->m_DescriptionLength[0] = m_menus[eMenu::LOSE]->m_DescriptionLength[0] = kill.size();
		m_menus[eMenu::WIN]->m_pDescriptionText[1] = m_menus[eMenu::LOSE]->m_pDescriptionText[1] = itemsCollected.c_str();
		m_menus[eMenu::WIN]->m_DescriptionLength[1] = m_menus[eMenu::LOSE]->m_DescriptionLength[1] = itemsCollected.size();
		m_menus[eMenu::WIN]->m_pDescriptionText[2] = m_menus[eMenu::LOSE]->m_pDescriptionText[2] = timeElapsed.c_str();
		m_menus[eMenu::WIN]->m_DescriptionLength[2] = m_menus[eMenu::LOSE]->m_DescriptionLength[2] = timeElapsed.size();
	}
}

CGUIManager::~CGUIManager()
{
	// SAFE RELEASE EVERYTHING!!
	if (m_pdwTextFormat)	{ m_pdwTextFormat->Release();		m_pdwTextFormat = nullptr; }
	if (m_pdwTitleFormat)	{ m_pdwTitleFormat->Release();		m_pdwTitleFormat = nullptr; }
	if (m_pdwHUDFormat)		{ m_pdwHUDFormat->Release();		m_pdwHUDFormat = nullptr; }
	if (m_pdwWriteFactory)	{ m_pdwWriteFactory->Release();		m_pdwWriteFactory = nullptr; }
	if (m_pd2dBitmap)		{ m_pd2dBitmap->Release();			m_pd2dBitmap = nullptr; }
	if (m_pd2dHRT)			{ m_pd2dHRT->Release();				m_pd2dHRT = nullptr; }
	if (m_pd2dRT)			{ m_pd2dRT->Release();				m_pd2dRT = nullptr; }
	if (m_pd2dColorBrush)	{ m_pd2dColorBrush->Release();		m_pd2dColorBrush = nullptr; }
	if (m_pd2dColorBrush2)	{ m_pd2dColorBrush2->Release();		m_pd2dColorBrush2 = nullptr; }
	if (m_pd2dColorBrush3)	{ m_pd2dColorBrush3->Release();		m_pd2dColorBrush3 = nullptr; }
	if (m_pd2dColorBrush4)	{ m_pd2dColorBrush4->Release();		m_pd2dColorBrush4 = nullptr; }
	if (m_pd2dColorBrush5)	{ m_pd2dColorBrush5->Release();		m_pd2dColorBrush5 = nullptr; }
	if (m_pd2dColorBrush6)	{ m_pd2dColorBrush6->Release();		m_pd2dColorBrush6 = nullptr; }
	if (m_pd2dContext)		{ m_pd2dContext->Release();			m_pd2dContext = nullptr; }
	if (m_pd2dFactory)		{ m_pd2dFactory->Release();			m_pd2dFactory = nullptr; }
	if (m_pd2dDevice)		{ m_pd2dDevice->Release();			m_pd2dDevice = nullptr; }
	if (m_pdxgDevice)		{ m_pdxgDevice->Release();			m_pdxgDevice = nullptr; }
	if (m_pdxgSurface)		{ m_pdxgSurface->Release();			m_pdxgSurface = nullptr; }
	if (m_pd2dLogoBatch)	{ m_pd2dLogoBatch->Release();		m_pd2dLogoBatch = nullptr; }
	if (m_pd2dTitleSprite)	{ m_pd2dTitleSprite->Release();		m_pd2dTitleSprite = nullptr; }
	for (size_t i = 0; i < m_LogoSprites.size(); i++)
	{
		if (m_LogoSprites[i])
		{
			m_LogoSprites[i]->Release();
			m_LogoSprites[i] = nullptr;
		}
	}
	if (m_pcHud) { delete m_pcHud;	m_pcHud = nullptr; }
	for (size_t i = 0; i < m_menus.size(); i++)
	{
		if (m_menus[i])
			delete m_menus[i];
	}
	m_menus.clear();
	m_LogoSprites.clear();
	//m_nCurrent = eMenu::GAME;
}

void CGUIManager::Render()
{
	float scaleX = m_fWidth / 1024.0f, scaleY = m_fHeight / 576.0f;
	IDWriteTextLayout* pdwTextLayout = nullptr;
	// Set the context to the place we want to render to
	m_pd2dContext->SetTarget(m_pd2dBitmap);
	m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
	if (m_nCurrent == eMenu::GAME) // Draws HUD in-game
	{
		/*m_pd2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pcHud->DrawBaseHUD(m_pd2dContext, m_pd2dRT, m_pd2dColorBrush, m_pd2dColorBrush4, m_pd2dColorBrush5, m_fWidth, m_fHeight);
		m_pcHud->WriteHealthAndMana(m_pd2dRT, m_pdwWriteFactory, m_pdwHUDFormat, m_pdwTitleFormat, m_pd2dColorBrush3, m_fWidth, m_fHeight, m_bControls);
		if (m_bShowInventory)
		{
			m_pcHud->DrawInventory(m_pd2dContext, m_pd2dRT, m_pdwWriteFactory, m_pdwInventoryFormat, m_pdwTitleFormat,
				m_pd2dColorBrush, m_pd2dColorBrush2, m_pd2dColorBrush3, m_pd2dColorBrush4, m_pd2dColorBrush5,
				m_fWidth, m_fHeight);
		}*/
	}
	else if (m_nCurrent == eMenu::CREDITS)
	{
		//// Scroll Depth does things by ratios
		//// To scroll text across the screen
		//static float scrollDepth = 0.8f + (CTime::GetDeltaF() * 0.1f);
		//if (m_bStartScroll)
		//{
		//	scrollDepth = 0.8f + (CTime::GetDeltaF() * 0.1f);
		//	m_bStartScroll = false;
		//}
		//scrollDepth -= (CTime::GetDeltaF() * 0.07f);
		//if (m_menus[eMenu::CREDITS]->GetTitle())
		//{
		//	if (scrollDepth < -1.0f - m_menus[eMenu::CREDITS]->GetTitleBox().bottom / m_fHeight)
		//	{
		//		RestartScroll();
		//	}
		//}
		//else if (scrollDepth < -1.0f)
		//{
		//	RestartScroll();
		//}
		//std::wstring theCredits;
		//for (size_t i = 0; i < m_menus[m_nCurrent]->m_pDescriptionText.size(); i++)
		//{
		//	theCredits += m_menus[m_nCurrent]->m_pDescriptionText[i];
		//	theCredits += L"\n";
		//}
		//D2D1_RECT_F box;
		//float width, height;
		//if (m_menus[eMenu::CREDITS]->GetTitle())
		//{
		//	box = m_menus[m_nCurrent]->GetTitleBox();
		//	width = box.right - box.left; height = box.bottom - box.top;
		//	m_pdwWriteFactory->CreateTextLayout(
		//		m_menus[m_nCurrent]->GetTitle(),
		//		(UINT32)m_menus[m_nCurrent]->GetTitleLength(),
		//		m_pdwTitleFormat,
		//		width,
		//		height,
		//		&pdwTextLayout
		//	);
		//	m_pd2dRT->BeginDraw();
		//	m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY },
		//		D2D1::Point2F(box.left + (width / 2.0f), box.top + (height / 2.0f))) * D2D1::Matrix3x2F::Translation({ 0, (scrollDepth * m_fHeight) }));
		//	m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush2);
		//	m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
		//	pdwTextLayout->Release();
		//	pdwTextLayout = nullptr;
		//	m_pd2dRT->EndDraw();
		//	box = D2D1::RectF(0, box.bottom, m_fWidth, m_fHeight + box.bottom);
		//}
		//else
		//{
		//	box = D2D1::RectF(0, 0, m_fWidth, m_fHeight);
		//}
		//width = box.right - box.left; height = box.bottom - box.top;
		//m_pdwWriteFactory->CreateTextLayout(
		//	theCredits.c_str(),
		//	(UINT32)theCredits.size(),
		//	m_pdwTextFormat,
		//	width,
		//	height,
		//	&pdwTextLayout
		//);
		//
		//m_pd2dRT->BeginDraw();
		//m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY },
		//	D2D1::Point2F(box.left + (width / 2.0f), box.top + (height / 2.0f))) * D2D1::Matrix3x2F::Translation({ 0, (scrollDepth * m_fHeight) }));
		//m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush3);
		//m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
		//pdwTextLayout->Release();
		//pdwTextLayout = nullptr;
		//box = D2D1::RectF(0, (m_fHeight * 0.75f), m_fWidth, m_fHeight);
		//m_pd2dRT->FillRectangle(box, m_pd2dColorBrush6);
		//m_pd2dRT->EndDraw();
		//m_pd2dContext->BeginDraw();
		//// Sets the sprites for all the buttons and sliders the menu contains
		//m_menus[m_nCurrent]->SetSprites();
		//// Draws the sprites
		//m_pd2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		//m_pd2dContext->DrawSpriteBatch(m_menus[m_nCurrent]->GetSpriteBatch(), m_menus[m_nCurrent]->GetButtonSprite());
		//m_pd2dContext->EndDraw();

		//m_pd2dRT->BeginDraw();
		//size_t size = m_menus[m_nCurrent]->m_pcButtons.size();
		//// Render button text for all buttons
		//for (size_t i = 0; i < size; i++)
		//{
		//	D2D1_RECT_F box = m_menus[m_nCurrent]->m_pcButtons[i]->GetButtonBox();
		//	float width = box.right - box.left, height = box.bottom - box.top;

		//	m_pdwWriteFactory->CreateTextLayout(
		//		m_menus[m_nCurrent]->m_pcButtons[i]->GetButtonText(),
		//		(UINT32)m_menus[m_nCurrent]->m_pcButtons[i]->GetTextLength(),
		//		m_pdwTextFormat,
		//		width,
		//		height,
		//		&pdwTextLayout
		//	);
		//	m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY },
		//		D2D1::Point2F(box.left + (width / 2.0f), box.top + (height / 2.0f))));
		//	if (m_menus[m_nCurrent]->m_pcButtons[i]->IsHovered())
		//	{
		//		m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush3);
		//	}
		//	else
		//	{
		//		m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush2);
		//	}
		//	m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
		//	pdwTextLayout->Release();
		//	pdwTextLayout = nullptr;
		//}
		//m_pd2dRT->EndDraw();
	}
	else if (m_menus.size() > 0 && m_nCurrent < m_menus.size())
	{
		// Draw a transparent box to hold the menu items in
		m_pd2dRT->BeginDraw();
		m_pd2dRT->FillRectangle(D2D1::RectF(m_fWidth * 0.1f, m_fHeight * 0.05f, m_fWidth * 0.9f, m_fHeight * 0.95f), m_pd2dColorBrush);
		m_pd2dRT->EndDraw();
		// Render the menu
		m_pd2dContext->BeginDraw();
		if (m_nCurrent == eMenu::START)
		{
			D2D1_RECT_F box = D2D1::RectF(m_fWidth * 0.3f, m_fHeight * 0.05f, m_fWidth * 0.7f, m_fHeight * 0.35f);
			m_pd2dLogoBatch->SetSprites(0, 1, &box, nullptr, &D2D1::ColorF(D2D1::ColorF::White));
			m_pd2dContext->DrawSpriteBatch(m_pd2dLogoBatch, m_pd2dTitleSprite);
		}
		// Sets the sprites for all the buttons and sliders the menu contains
		m_menus[m_nCurrent]->SetSprites();
		// Draws the sprites
		m_pd2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pd2dContext->DrawSpriteBatch(m_menus[m_nCurrent]->GetSpriteBatch(), m_menus[m_nCurrent]->GetButtonSprite());
		m_pd2dContext->EndDraw();
		// Draw Text
		m_pd2dRT->BeginDraw();
		// Render title text if it exists
		const WCHAR* titleText = m_menus[m_nCurrent]->GetTitle();
		if (titleText)
		{
			float width, height;
			D2D1_RECT_F work = m_menus[m_nCurrent]->GetTitleBox();
			width = work.right - work.left;
			height = work.bottom - work.top;
			m_pdwWriteFactory->CreateTextLayout(
				titleText,
				(UINT32)m_menus[m_nCurrent]->GetTitleLength(),
				m_pdwTitleFormat,
				width,
				height,
				&pdwTextLayout
			);
			/*m_pd2dRT->DrawTextW(titleText, (UINT32)m_menus[m_nCurrent]->GetTitleLength(),
				m_pdwTitleFormat, m_menus[m_nCurrent]->GetTitleBox(), m_pd2dColorBrush2);*/
			m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY },
				D2D1::Point2F(work.left + (width / 2.0f), work.top + (height / 2.0f))));
			m_pd2dRT->DrawTextLayout(D2D1::Point2F(work.left, work.top), pdwTextLayout, m_pd2dColorBrush2);
			m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
			pdwTextLayout->Release();
			pdwTextLayout = nullptr;
		}
		size_t size = m_menus[m_nCurrent]->m_pcButtons.size();
		// Render button text for all buttons
		for (size_t i = 0; i < size; i++)
		{
			D2D1_RECT_F box = m_menus[m_nCurrent]->m_pcButtons[i]->GetButtonBox();
			float width = box.right - box.left, height = box.bottom - box.top;

			m_pdwWriteFactory->CreateTextLayout(
				m_menus[m_nCurrent]->m_pcButtons[i]->GetButtonText(),
				(UINT32)m_menus[m_nCurrent]->m_pcButtons[i]->GetTextLength(),
				m_pdwTextFormat,
				width,
				height,
				&pdwTextLayout
			);
			m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY },
				D2D1::Point2F(box.left + (width / 2.0f), box.top + (height / 2.0f))));
			if (m_menus[m_nCurrent]->m_pcButtons[i]->IsHovered())
			{
				m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush3);
			}
			else
			{
				m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush2);
			}
			m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
			pdwTextLayout->Release();
			pdwTextLayout = nullptr;
		}
		// Do some extra stuff for options menus
		// This is for descriptive text
		if (m_menus[m_nCurrent]->IsOptionsMenu())
		{
			size_t buffer = 0;
			if (!m_bControls)
			{
				buffer = 7;
			}
			for (size_t i = 0; i < 7; i++)
			{
				D2D1_RECT_F box = m_menus[m_nCurrent]->m_DescriptionBoxes[i];
				float width = box.right - box.left, height = box.bottom - box.top;
				m_pdwWriteFactory->CreateTextLayout(
					m_menus[m_nCurrent]->m_pDescriptionText[i + buffer],
					(UINT32)m_menus[m_nCurrent]->m_DescriptionLength[i + buffer],
					m_pdwTextFormat,
					width,
					height,
					&pdwTextLayout
				);
				m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY },
					D2D1::Point2F(box.left + (width / 2.0f), box.top + (height / 2.0f))));
				m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush2);
				/*m_pd2dRT->DrawTextW(m_menus[m_nCurrent]->m_pDescriptionText[i],
					(UINT32)m_menus[m_nCurrent]->m_DescriptionLength[i], m_pdwTextFormat,
					m_menus[m_nCurrent]->m_DescriptionBoxes[i], m_pd2dColorBrush2);*/
				m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
				pdwTextLayout->Release();
				pdwTextLayout = nullptr;
			}
		}
		else
		{
			size = m_menus[m_nCurrent]->m_pDescriptionText.size();
			// Render all description text
			for (size_t i = 0; i < size; i++)
			{
				D2D1_RECT_F box = m_menus[m_nCurrent]->m_DescriptionBoxes[i];
				float width = box.right - box.left, height = box.bottom - box.top;
				m_pdwWriteFactory->CreateTextLayout(
					m_menus[m_nCurrent]->m_pDescriptionText[i],
					(UINT32)m_menus[m_nCurrent]->m_DescriptionLength[i],
					m_pdwTextFormat,
					width,
					height,
					&pdwTextLayout
				);
				m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Scale({ scaleX, scaleY },
					D2D1::Point2F(box.left + (width / 2.0f), box.top + (height / 2.0f))));
				m_pd2dRT->DrawTextLayout(D2D1::Point2F(box.left, box.top), pdwTextLayout, m_pd2dColorBrush2);
				/*m_pd2dRT->DrawTextW(m_menus[m_nCurrent]->m_pDescriptionText[i],
					(UINT32)m_menus[m_nCurrent]->m_DescriptionLength[i], m_pdwTextFormat,
					m_menus[m_nCurrent]->m_DescriptionBoxes[i], m_pd2dColorBrush2);*/
				m_pd2dRT->SetTransform(D2D1::Matrix3x2F::Identity());
				pdwTextLayout->Release();
				pdwTextLayout = nullptr;
			}
		}
		m_pd2dRT->EndDraw();
	}

	m_pd2dContext->SetTarget(nullptr); // Allows for resizing if needed
	// I don't know if I need to do this, but it is better to be safe than sorry
}

void CGUIManager::AddButtonToMenu(int menu, void(*pFunction)(), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	CreateMenus(menu);
	// Add that thing to the menu
	m_menus[menu]->AddButton(pFunction, buttonText, width, height, wWidth, wHeight, left, right, top, bottom);
}

void CGUIManager::AddSliderToMenu(int menu, int nVary, int nType, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	CreateMenus(menu);
	// Add that thing to the menu
	m_menus[menu]->AddSlider(menu, nVary, nType, width, height, wWidth, wHeight, left, right, top, bottom);
}

void CGUIManager::SetTitleToMenu(int menu, const WCHAR * title_text, int width, int height, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	CreateMenus(menu);
	// Add that thing to the menu
	m_menus[menu]->MakeTitle(title_text, width, height, left, right, top, bottom);
}

void CGUIManager::AddDescriptionToMenu(int menu, const WCHAR * text, int width, int height, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	CreateMenus(menu);
	// Add that thing to the menu
	m_menus[menu]->AddDescriptionText(text, width, height, left, right, top, bottom);
}

void CGUIManager::WindowResizeEvent(IDXGISwapChain* pdxgSwapChain, ID3D11Texture2D* pd3dTexture, float fwWidth, float fwHeight)
{
	// Resize all menu items
	for (size_t i = 0; i < m_menus.size(); i++)
	{
		m_menus[i]->ResizeItems(m_fWidth, m_fHeight, fwWidth, fwHeight);
	}
	m_pcHud->ResizeItems(m_fWidth, m_fHeight, fwWidth, fwHeight);
}

void CGUIManager::ChangeCurrentMenu(int menu)
{
	if (menu < eMenu::COUNT)
	{
		m_nCurrent = menu;
	}
}

void CGUIManager::Release2D()
{
	// Release components to be changed/resized
	if (m_pd2dRT) { m_pd2dRT->Release();			m_pd2dRT = nullptr; }
	if (m_pd2dBitmap) { m_pd2dBitmap->Release();	m_pd2dBitmap = nullptr; }
	if (m_pdxgSurface) { m_pdxgSurface->Release();	m_pdxgSurface = nullptr; }
}

void CGUIManager::ReInitialize2D(IDXGISwapChain* pdxgSwapChain, ID3D11Texture2D* pd3dTexture)
{
	// Re-create Render Target view
	IDXGISurface* raster = nullptr;
	if (FAILED(pd3dTexture->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(&raster))))
	{
		MessageBox(nullptr, L"Failed to resize render target view and bitmap", L"Error", MB_OK);
		raster->Release();
		exit(0);
	}
	FLOAT dpiX, dpiY;
	m_pd2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
			D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);
	DWORD something;
	if (FAILED(m_pd2dFactory->CreateDxgiSurfaceRenderTarget(raster, &props, &m_pd2dRT)))
	{
		MessageBox(nullptr, L"Failed to resize render target view", L"Error", MB_OK);
		raster->Release();
		something = GetLastError();
		exit(0);
	}
	// Recreate compatible bitmap for sprites
	DXGI_SURFACE_DESC sDesc;
	D2D1_BITMAP_PROPERTIES1 bitProp;
	IDXGISurface2* texSurface = nullptr;
	ZeroMemory(&sDesc, sizeof(DXGI_SURFACE_DESC));
	ZeroMemory(&bitProp, sizeof(D2D1_BITMAP_PROPERTIES1));
	raster->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface); // should always succeed
	texSurface->GetDesc(&sDesc);
	bitProp.pixelFormat = { sDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED };
	bitProp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	// create compatible bitmap to main DXGI surface (for high speed sprite rendering)
	if (FAILED(m_pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &m_pd2dBitmap)))
	{
		MessageBox(nullptr, L"Failed to resize bitmap for sprite rendering", L"Error", MB_OK);
		raster->Release();
		exit(0);
	}
	texSurface->Release();
	raster->Release();
	// Hold onto new swapchain width and height
	DXGI_SWAP_CHAIN_DESC sd;
	pdxgSwapChain->GetDesc(&sd);
	m_fWidth = (float)sd.BufferDesc.Width;
	m_fHeight = (float)sd.BufferDesc.Height;
}

void CGUIManager::SetHudMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	m_pcHud->SetMenu(pd3dDevice, pd2dContext, fWidth, fHeight, fwWidth, fwHeight);
}

void CGUIManager::SetPlayerValues(float fCurrHealth, float fMaxHealth, float fCurrMana, float fMaxMana)
{
	m_pcHud->SetPlayerValues(fCurrHealth, fMaxHealth, fCurrMana, fMaxMana);
}

void CGUIManager::SetMiniMap(std::vector<std::vector<Spot>> cMap)
{
	m_pcHud->SetMiniMap(cMap);
}

void CGUIManager::NewGame()
{
	m_pcHud->NewGame();
}

void CGUIManager::SetControls(bool bControls)
{
	m_bControls = bControls;
}

int CGUIManager::GetMenuState()
{
	return m_nCurrent;
}