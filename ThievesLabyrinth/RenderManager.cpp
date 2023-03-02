#include "RenderManager.h"
#include "Entity.h"
#include "DebugManager.h"
#include "GUIManager.h"
#include "AnimationManager.h"
#include "ComponentManager.h"
#include "MeshRenderer.h"
#include "EventManager.h"
#include "GameLogic.h"
#include "Time.h"

#include "WICTextureLoader.h"
#include "CameraController.h"

#include "Transform.h"
#include "InputManager.h"
#include "AnimatorComponent.h"
#include "LightComponent.h"
#include "IndicatorController.h"
#include "ParticleEffect.h"
#include "FireDemonController.h"

#include "DefaultVS.csh"
#include "DefaultPS.csh"
#include "DebugVS.csh"
#include "DebugPS.csh"
#include "SkinnedVS.csh"
#include "DissolvePS.csh"
#include "SeeThroughPS.csh"
#include "SolidColorPS.csh"
#include "FullscreenQuadVS.csh"
#include "Pass2DPS.csh"
#include "BrightnessPS.csh"
#include "HorizontalBlurPS.csh"
#include "VerticalBlurPS.csh"
#include "TorchFirePS.csh"
#include "FirewallPS.csh"
#include "FadeOutPS.csh"
#include "FlashPS.csh"
#include "ParticlesPS.csh"
#include "AOEAttackPS.csh"

#include <dxgi1_2.h>
#pragma comment(lib, "dxgi.lib")

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }

#define VALIDATE(hr, message) { if(FAILED(hr)) { MessageBox(nullptr, message, L"Error", MB_OK); exit(0);} } 

D3D11_VIEWPORT CRenderManager::m_pd3dViewport[eViewport::COUNT];
//CGUIManager* CRenderManager::m_pcGUIManager;
IDXGISwapChain*	CRenderManager::m_pd3dSwapChain;
TCameraConstantBuffer CRenderManager::m_tCameraBuffer;
float CRenderManager::m_fBrightness;


CRenderManager::CRenderManager(HWND cWindow, CComponentManager * pcComponentManager)
{
	m_pcComponentManager = pcComponentManager;
	m_cWindow = cWindow;


	m_tCameraBuffer.mViewMatrix = XMMatrixIdentity();
	m_tCameraBuffer.mProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65), 1024 / 576.0f, 0.1f, 1000.0f);
	m_fBrightness = 1.0f;

	CInputManager::SetProjMat(*(CMath::TMATRIX*)&m_tCameraBuffer.mProjMatrix);
	//CInputManager::SetProjMat(*(CMath::TMATRIX*)&m_d3dProjMatrix);


	InitializeDirectXComponents();
}

void CRenderManager::InitializeDirectXComponents()
{

	#pragma region Device and Swap Chain


	//Use IDXGISwapChain::SetFullscreenState to change to Fullscreen mode.

	UINT               numLevelsRequested = 1;
	D3D_FEATURE_LEVEL  featureLevelsSupported;
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	RECT rc;
	GetClientRect(m_cWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	/************************************* ADAPTER(Graphics Card) TEST **********************************************************/
#define USE_ADAPTER
#ifdef USE_ADAPTER
	D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
	ID3D11Device1*                      g_pd3dDevice1 = nullptr;
	ID3D11DeviceContext1*               g_pImmediateContext1 = nullptr;
	IDXGISwapChain1*                    g_pSwapChain1 = nullptr;

	HRESULT hr = S_OK;

	

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG

	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif // _DEBUG


	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &m_pd3dDevice, &featureLevelsSupported, &m_pd3dDeviceContext);

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &m_pd3dDevice, &featureLevelsSupported, &m_pd3dDeviceContext);
		}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return;

#ifdef _DEBUG
	VALIDATE(m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&DebugDevice)),L"DebugDevice");
#endif

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				DXGI_ADAPTER_DESC adapterDesc;
				adapter->GetDesc(&adapterDesc);
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
		return;

	// Create swap chain
	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		// DirectX 11.1 or later
		hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
		if (SUCCEEDED(hr))
		{
			(void)m_pd3dDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd1 = {};
		sd1.Width = width;
		sd1.Height = height;
		sd1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd1.SampleDesc.Count = 1;
		sd1.SampleDesc.Quality = 0;
		sd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		sd1.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(m_pd3dDevice, m_cWindow, &sd1, nullptr, nullptr, &g_pSwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_pd3dSwapChain));
		}

		dxgiFactory2->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = m_cWindow;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
#if _DEBUG
		sd.Flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

		hr = dxgiFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pd3dSwapChain);
	}


	//dxgiFactory->MakeWindowAssociation(m_cWindow, DXGI_MWA_NO_ALT_ENTER);

	SAFE_RELEASE(dxgiFactory);
	SAFE_RELEASE(g_pd3dDevice1);
	SAFE_RELEASE(g_pImmediateContext1);
	SAFE_RELEASE(g_pSwapChain1);
#else


	D3D_FEATURE_LEVEL  featureLevelsRequested = D3D_FEATURE_LEVEL_11_0;

	VALIDATE(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlags,
		&featureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&sd,
		&m_pd3dSwapChain,
		&m_pd3dDevice,
		&featureLevelsSupported,
		&m_pd3dDeviceContext),
		L"D3D11CreateDeviceAndSwapChain");


#ifdef _DEBUG
	VALIDATE(m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&DebugDevice)), L"DebugDevice");
#endif

#endif // !USE_ADAPTER


	
	#pragma endregion

	#pragma region Rasterizer State

	m_pd3dViewport[eViewport::DEFAULT].Width = (float)width;
	m_pd3dViewport[eViewport::DEFAULT].Height = (float)height;
	m_pd3dViewport[eViewport::DEFAULT].MinDepth = 0.0f;
	m_pd3dViewport[eViewport::DEFAULT].MaxDepth = 1.0f;
	m_pd3dViewport[eViewport::DEFAULT].TopLeftX = 0;
	m_pd3dViewport[eViewport::DEFAULT].TopLeftY = 0;

	m_pd3dDeviceContext->RSSetViewports(1, &m_pd3dViewport[eViewport::DEFAULT]);

	D3D11_RASTERIZER_DESC rd;

	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthBias = false;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = true;

	VALIDATE(m_pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRasterizerState[eRasterizerState::DEFAULT]), L"D3D11CreateRasterizerState");

	rd.CullMode = D3D11_CULL_NONE;
	VALIDATE(m_pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRasterizerState[eRasterizerState::NO_CULLING]), L"D3D11CreateRasterizerState");

	rd.FillMode = D3D11_FILL_WIREFRAME;
	VALIDATE(m_pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRasterizerState[eRasterizerState::WIREFRAME]), L"D3D11CreateRasterizerState");

	m_pd3dDeviceContext->RSSetState(m_pd3dRasterizerState[eRasterizerState::DEFAULT]);

	#pragma endregion

	CreateResources(width, height, false); //Creates buffers and textures that requiered the window size
	
	std::vector<std::thread> srvThreads;
	#pragma region SRV

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile, this, "..//Assets//Textures//Player//FireGolem//FireGolem_Diffuse.png", eSRV::FIRE_DEMON_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Player//FireGolem//Boss_Death_texture.png", eSRV::FIRE_DEMON_EXTINGUISHED);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile, this, "..//Assets//Textures//Player//Jet//Jet_Main_DIF.png", eSRV::JET_DIFFUSE);



	#pragma region Enemies
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Mage//Mage_Diffuse.png", eSRV::ENEMY_MAGE_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Mage//Mage_Normal.png", eSRV::ENEMY_MAGE_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Mage//Mage_Specular.png", eSRV::ENEMY_MAGE_SPECULAR);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Chicken//Chicken_Diffuse.png", eSRV::ENEMY_CHICKEN_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Viking//Viking_Diffuse.png", eSRV::ENEMY_VIKING_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Viking//Viking_Normal.png", eSRV::ENEMY_VIKING_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Viking//Viking_Specular.png", eSRV::ENEMY_VIKING_SPECULAR);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Archer//Archer_Diffuse.png", eSRV::ENEMY_ARCHER_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Archer//Archer_Specular.png", eSRV::ENEMY_ARCHER_SPECULAR);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Enemies//Knight//Knight_Diffuse.png", eSRV::ENEMY_KNIGHT_DIFFUSE);
	#pragma endregion

	#pragma region Environment
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Floor//Floor_Diffuse.png", eSRV::FLOOR_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Floor//Floor_Normal.png", eSRV::FLOOR_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Floor//Floor_Specular.png", eSRV::FLOOR_SPECULAR);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Door//Door_Diffuse.png", eSRV::DOOR_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Door//Door_Normal.png", eSRV::DOOR_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Door//Door_Specular.png", eSRV::DOOR_SPECULAR);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Wall//Wall_Diffuse.png", eSRV::WALL_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Wall//Wall_Normal.png", eSRV::WALL_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Wall//Wall_Specular.png", eSRV::WALL_SPECULAR);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Pillar//Pillar_Diffuse.png", eSRV::PILLAR_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Pillar//Pillar_Normal.png", eSRV::PILLAR_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Pillar//Pillar_Specular.png", eSRV::PILLAR_SPECULAR);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Torch//Torch_Diffuse.png", eSRV::TORCH_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Torch//Torch_Normal.png", eSRV::TORCH_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//SpikeTrap//SpikeTrap_Diffuse.png", eSRV::SPIKE_TRAP_DIFFUSE);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Barrel//Barrel_D.png", eSRV::BARREL_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Barrel//Barrel_N.png", eSRV::BARREL_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Barrel//Barrel_S.png", eSRV::BARREL_SPECULAR);	

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Chair_Table//Chair_Table_D.png", eSRV::CHAIR_TABLE_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Chair_Table//Chair_Table_N.png", eSRV::CHAR_TABLE_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Flag//Flag_D.png", eSRV::FLAG_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Flag//Flag_N.png", eSRV::FLAG_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Rock//Rock_D.png", eSRV::ROCKS_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Rock//Rock_N.png", eSRV::ROCKS_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Rock//Rock_S.png", eSRV::ROCKS_SPECULAR);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Sacks_Logs_Barrel02//Sacks_Logs_Barrels_D.png", eSRV::SACK_LOG_BARREL_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Sacks_Logs_Barrel02//Sacks_Logs_Barrels_N.png", eSRV::SACK_LOG_BARREL_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Chest//Chest_D.png", eSRV::CHEST_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Chest//Chest_N.png", eSRV::CHEST_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Chest//Chest_S.png", eSRV::CHEST_SPECULAR);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Cart//Cart_D.png", eSRV::CART_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Cart//Cart_N.png", eSRV::CART_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Platform//Platform_D.png", eSRV::PLATFORM_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Platform//Platform_N.png", eSRV::PLATFORM_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Altar//Altar_D.png", eSRV::ALTAR_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Altar//Altar_N.png", eSRV::ALTAR_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Arches//Arches_D.png", eSRV::ARCHES_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Arches//Arches_N.png", eSRV::ARCHES_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Bookcase//Bookcase_D.png", eSRV::BOOKCASE_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Bookcase//Bookcase_N.png", eSRV::BOOKCASE_NORMAL);

	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Bookcase//Book_D.png", eSRV::BOOKS_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Bookcase//Book_N.png", eSRV::BOOKS_NORMAL);


	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Statue//Statue_D.png", eSRV::STATUE_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Statue//Statue_N.png", eSRV::STATUE_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//Statue//Statue_S.png", eSRV::STATUE_SPECULAR);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Environment//blue.png", eSRV::SPAWN);
	#pragma endregion
	
	#pragma region Utility
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//LavaTexture.jpg",			eSRV::LAVA_TEXTURE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//NoiseTexture.jpg",			eSRV::SMOKE_NOISE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//CloudNoiseTexture.jpg",		eSRV::CLOUD_NOISE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//PerlinNoiseTexture.png",	eSRV::PERLIN_NOISE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//FadeTexture.jpg",			eSRV::FADE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//FlameShapeTexture.png",		eSRV::FLAME_SHAPE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//Ricardo.jpg",				eSRV::RICARDO);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//Firewall2.jpg",				eSRV::FIREWALL_NOISE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Utility//Test.png",					eSRV::FLASH);
#pragma endregion

	#pragma region Effects
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects/Smoke.png", eSRV::PARTICLE_SMOKE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects/Spark.png", eSRV::PARTICLE_SPARK);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//Fire.png", eSRV::PARTICLE_FIRE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//ShieldEffect.png", eSRV::SHIELD_EFFECT);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//TorchParticle.png", eSRV::TORCH_PARTICLE_EFFECT);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//BasicAttackParticle.png", eSRV::BASIC_ATTACK_PARTICLE_EFFECT);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//FireballParticle.png", eSRV::FIREBALL_PARTICLE_EFFECT);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//HealthParticle.png", eSRV::HEALTH_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//ManaParticle.png", eSRV::MANA_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//DefenseParticle.png", eSRV::DEFENSE_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//DamageParticle.png", eSRV::DAMAGE_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//ArcaneParticle.png", eSRV::ARCANE_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//ChickenParticle.png", eSRV::CHICKEN_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//ExplosionParticle.png", eSRV::EXPLOSION_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Effects//feather.png", eSRV::FEATHER_PARTICLE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile, this, "..//Assets//Textures//Effects//FirewallParticle.png", eSRV::FIREWALL_PARTICLE_EFFECT);

	#pragma endregion

	#pragma region Misc
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Misc//Chest//Loot_diffuse.png", eSRV::LOOT_CHEST_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Misc//Chest//Loot_normal.png", eSRV::LOOT_CHEST_NORMAL);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Misc//Chest//Loot_specular.png", eSRV::LOOT_CHEST_SPECULAR);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Misc//potion//bottle.png", eSRV::POTION);
	#pragma endregion

	#pragma region UI
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//UI//DirectionIndicator.png", eSRV::DIRECTION_INDICATOR);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//UI//ClickIndicator.png", eSRV::CLICK_INDICATOR);
	#pragma endregion
	
	#pragma region Projectiles
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Projectiles//BasicAttackDiffuse.jpg", eSRV::BASIC_ATTACK_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Projectiles//FireballDiffuse.jpg", eSRV::FIREBALL_DIFFUSE); 
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Projectiles//AOEDiffuse.png", eSRV::AOE_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Projectiles//ArrowDiffuse.png", eSRV::ARROW_DIFFUSE);
	srvThreads.emplace_back(&CRenderManager::LoadTextureFromFile,this, "..//Assets//Textures//Projectiles//MageProjectileDiffuse.jfif", eSRV::MAGE_PROJECTILE_DIFFUSE);
	#pragma endregion

	//SRV
	#pragma endregion 

	#pragma region Samplers

	D3D11_SAMPLER_DESC samplerDesc{ };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	VALIDATE(m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::WRAP]), L"CreateSampler");

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	VALIDATE(m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::ANIMATION_SAMPLER]), L"CreateSampler");

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	VALIDATE(m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::CLAMP]), L"CreateSample");

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	VALIDATE(m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::BORDER]), L"CreateSample");

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	VALIDATE(m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::MIRROR]), L"CreateSample");

	#pragma endregion

	std::vector<std::thread> buffThreads;
	#pragma region Buffers
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Player//FireGolem.bin",		eVertexBuffer::FIRE_DEMON); 
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Enemies//BattleMage.bin",		eVertexBuffer::ENEMY_MAGE); 
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Enemies//Chicken.bin",		eVertexBuffer::ENEMY_CHICKEN);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Enemies//Viking.bin",			eVertexBuffer::ENEMY_VIKING); 
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Enemies//Archer.bin",			eVertexBuffer::ENEMY_ARCHER);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Enemies//Knight.bin",			eVertexBuffer::ENEMY_KNIGHT);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Level1.bin",		eVertexBuffer::ROOM1);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Level2.bin",		eVertexBuffer::ROOM2);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Level3.bin",		eVertexBuffer::ROOM3);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Door.bin",		eVertexBuffer::DOOR);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Wall.bin",		eVertexBuffer::WALL);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Pillar.bin",		eVertexBuffer::PILLAR);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile, this, "..//Assets//Meshes//Environment//Spike.bin", eVertexBuffer::SPIKE_TRAP);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//SpikeHole.bin",	eVertexBuffer::SPIKE_PLATFORM);
	//buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Spawn_Point.bin",eVertexBuffer::SPAWN);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Level5.bin",		eVertexBuffer::ROOM5);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Torch.bin",		eVertexBuffer::TORCH);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Effects//quad.bin",			eVertexBuffer::QUAD);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//firewall.bin",	eVertexBuffer::FIREWALL);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//New_OpenDoor.bin",	eVertexBuffer::OPENDOOR);

	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Misc//Projectile.bin",		eVertexBuffer::SPHERE);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Misc//AOEMesh.bin",			eVertexBuffer::AOE_MESH);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Misc//Arrow.bin",				eVertexBuffer::ARROW);

	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Barrel01.bin",	eVertexBuffer::BARREL01);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Flag.bin",		eVertexBuffer::FLAG);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Rock01.bin",		eVertexBuffer::ROCK01);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Rock02.bin",		eVertexBuffer::ROCK02);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Rock03.bin",		eVertexBuffer::ROCK03);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Statue.bin",		eVertexBuffer::STATUE);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Chair.bin",		eVertexBuffer::CHAIR);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Table.bin",		eVertexBuffer::TABLE);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Chest.bin",		eVertexBuffer::CHEST);

	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Logs.bin",		eVertexBuffer::LOGS);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Barrel02.bin",	eVertexBuffer::BARREL02);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Sack.bin",		eVertexBuffer::SACK);

	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Cart.bin",		eVertexBuffer::CART);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Platform_Circle.bin", eVertexBuffer::PLATFORM_CIRCLE);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Platform.bin",	eVertexBuffer::PLATFORM);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Altar.bin",		eVertexBuffer::ALTAR);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Arches.bin",		eVertexBuffer::ARCHES);

	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//BookCase.bin",	eVertexBuffer::BOOKCASE);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Environment//Book.bin",		eVertexBuffer::BOOKS);

	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Misc//Chest.bin",				eVertexBuffer::LOOTCHEST);
	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile,this, "..//Assets//Meshes//Misc//Potion.bin",			eVertexBuffer::POTION);

	buffThreads.emplace_back(&CRenderManager::LoadMeshFromFile, this, "..//Assets//Meshes//Player//Jet.bin", eVertexBuffer::JET);


	D3D11_BUFFER_DESC bDesc = { 0 };
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(TDebugLineMessage) * MAX_DEBUG_LINES;	

	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES]), L"CreateBuffer");

	bDesc.ByteWidth = sizeof(TModelConstantBuffer);
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::MODEL]), L"CreateBuffer");

	bDesc.ByteWidth = sizeof(TCameraConstantBuffer);
	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::CAMERA]), L"CreateBuffer");

	bDesc.ByteWidth = sizeof(TAnimationBuffer);
	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES]), L"CreateBuffer");

	bDesc.ByteWidth = sizeof(TLightsConstantBuffer);
	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::LIGHTS]), L"CreateBuffer");

	bDesc.ByteWidth = sizeof(TResourceConstantBuffer);
	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::RESOURCES]), L"CreateBuffer");

	bDesc.ByteWidth = sizeof(TColorConstantBuffer);
	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::COLOR]), L"CreateBuffer");

	bDesc.ByteWidth = sizeof(TVectorConstantBuffer);
	VALIDATE(m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]), L"Bightness Constant Buffer Creation Failed.");
	#pragma endregion

	#pragma region Shaders
	
	VALIDATE(m_pd3dDevice->CreateVertexShader(DefaultVS, sizeof(DefaultVS), nullptr, &m_pd3dVertexShader[eVertexShader::DEFAULT]),
			 L"DefaultVS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(DefaultPS, sizeof(DefaultPS), nullptr, &m_pd3dPixelShader[ePixelShader::DEFAULT]), 
			 L"DefaultPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreateVertexShader(DebugVS, sizeof(DebugVS), nullptr, &m_pd3dVertexShader[eVertexShader::DEBUG_SHADER]),
			 L"DebugVS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(DebugPS, sizeof(DebugPS), nullptr, &m_pd3dPixelShader[ePixelShader::DEBUG_SHADER]),
			 L"DebugPS Loading Error.");
	
	VALIDATE(m_pd3dDevice->CreateVertexShader(SkinnedVS, sizeof(SkinnedVS), nullptr, &m_pd3dVertexShader[eVertexShader::SKINNED]), 
			 L"SkinnedVS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(DissolvePS, sizeof(DissolvePS), nullptr, &m_pd3dPixelShader[ePixelShader::DISSOLVE]), 
			 L"DissolvePS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(SeeThroughPS, sizeof(SeeThroughPS), nullptr, &m_pd3dPixelShader[ePixelShader::SEE_THROUGH]), 
			 L"SeeThroughPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(SolidColorPS, sizeof(SolidColorPS), nullptr, &m_pd3dPixelShader[ePixelShader::SOLID_COLOR]), 
			 L"SolidColorPS Loading Error.");
	
	VALIDATE(m_pd3dDevice->CreateVertexShader(FullscreenQuadVS, sizeof(FullscreenQuadVS), nullptr, &m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD]),
			 L"FullscreenQuadVS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(Pass2DPS, sizeof(Pass2DPS), nullptr, &m_pd3dPixelShader[ePixelShader::PASS_2D]), 
			 L"Pass2DPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(HorizontalBlurPS, sizeof(HorizontalBlurPS), nullptr, &m_pd3dPixelShader[ePixelShader::HOR_BLUR]),
			 L"HorizontalBlurPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(VerticalBlurPS, sizeof(VerticalBlurPS), nullptr, &m_pd3dPixelShader[ePixelShader::VER_BLUR]),
			 L"VerticalBlurPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(BrightnessPS, sizeof(BrightnessPS), nullptr, &m_pd3dPixelShader[ePixelShader::BRIGHTNESS]),
			 L"BrightnessPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(TorchFirePS, sizeof(TorchFirePS), nullptr, &m_pd3dPixelShader[ePixelShader::TORCH_FIRE]),
			 L"TorchFirePS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(FirewallPS, sizeof(FirewallPS), nullptr, &m_pd3dPixelShader[ePixelShader::FIREWALL]),
			 L"FirewallPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(FadeOutPS, sizeof(FadeOutPS), nullptr, &m_pd3dPixelShader[ePixelShader::FADE_OUT]),
			 L"FadeOutPS Loading Error.");


	VALIDATE(m_pd3dDevice->CreatePixelShader(FlashPS, sizeof(FlashPS), nullptr, &m_pd3dPixelShader[ePixelShader::FLASH]),
			 L"FlashPS Loading Error.");


	VALIDATE(m_pd3dDevice->CreatePixelShader(ParticlesPS, sizeof(ParticlesPS), nullptr, &m_pd3dPixelShader[ePixelShader::PARTICLE]),
			 L"ParticlesPS Loading Error.");

	VALIDATE(m_pd3dDevice->CreatePixelShader(AOEAttackPS, sizeof(AOEAttackPS), nullptr, &m_pd3dPixelShader[ePixelShader::AoE]),
			 L"AOEAttackPS Loading Error.");

	#pragma endregion
	
	#pragma region Input Layout
	
	D3D11_INPUT_ELEMENT_DESC inputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VALIDATE(m_pd3dDevice->CreateInputLayout(inputDesc, 4, DefaultVS, sizeof(DefaultVS), &m_pd3dInputLayout[eInputLayout::DEFAULT]), L"CreateInputLayout");

	D3D11_INPUT_ELEMENT_DESC debugInputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	VALIDATE(m_pd3dDevice->CreateInputLayout(debugInputDesc, 2, DebugVS, sizeof(DebugVS), &m_pd3dInputLayout[eInputLayout::DEBUG]), L"CreateInputLayout");

	D3D11_INPUT_ELEMENT_DESC AnimationinputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINT_INDEX" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VALIDATE(m_pd3dDevice->CreateInputLayout(AnimationinputDesc, 6, SkinnedVS, sizeof(SkinnedVS), &m_pd3dInputLayout[eInputLayout::SKINNED]), L"CreateAnimationInputLayout");

	#pragma endregion
	
	#pragma region Blend State

	D3D11_BLEND_DESC blendDesc { 0 };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	float blendFactor[4] = { 0,0,0,0 };
	UINT sampleMask = 0xffffffff;

	VALIDATE(m_pd3dDevice->CreateBlendState(&blendDesc, &m_pd3dBlendStates[eBlendState::DEFAULT]), L"CreateBlendState");


	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	VALIDATE(m_pd3dDevice->CreateBlendState(&blendDesc, &m_pd3dBlendStates[eBlendState::ADDITIVE]), L"CreateBlendState");

	#pragma endregion

	#pragma region Subsystems

	m_pcGUIManager = new CGUIManager(m_pd3dDevice, m_pd3dSwapChain, m_pd3dTexture[eTexture::SCENE]);
	m_pcAnimatorManager = new CAnimatorManager(m_pd3dDevice);

	#pragma endregion

	for (auto& thread : srvThreads)
		thread.join();
	for (auto& thread : buffThreads)
		thread.join();
}

int CRenderManager::LoadMeshFromFile(const char * pchFilepath, int nType)
{
	std::fstream input;

	input.open(pchFilepath, std::ios_base::in | std::ios_base::binary);

	int nVertCount = 0;
	int nIndexCount = 0;;
	int* pnIndices;

	switch (nType)
	{
		//Animated
		case eVertexBuffer::ENEMY_VIKING:
		case eVertexBuffer::ENEMY_MAGE:
		case eVertexBuffer::ENEMY_ARCHER:
		case eVertexBuffer::ENEMY_KNIGHT:
		case eVertexBuffer::FIRE_DEMON:
		{
			
			TSkinnedVertex* ptVertices;

			if (input.is_open())
			{
				input.read((char*)&nVertCount, sizeof(int));

				
				ptVertices = new TSkinnedVertex[nVertCount];
				input.read((char*)ptVertices, sizeof(TSkinnedVertex) * nVertCount);

				input.read((char*)&nIndexCount, sizeof(int));

				pnIndices = new int[nIndexCount];
				input.read((char*)pnIndices, sizeof(int) * nIndexCount);

				input.close();

				HRESULT hr;

				D3D11_BUFFER_DESC bDesc = { 0 };
				bDesc.Usage = D3D11_USAGE_DYNAMIC;
				bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				
				bDesc.ByteWidth = sizeof(TSkinnedVertex) * nVertCount;
				bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = ptVertices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dVertexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateVertexBuffer", L"Error", MB_OK);
					exit(0);
				}

				bDesc.ByteWidth = sizeof(int) * nIndexCount;
				bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				data.pSysMem = pnIndices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dIndexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateIndexBuffer", L"Error", MB_OK);
					exit(0);
				}

				delete[] ptVertices;
				delete[] pnIndices;  


				m_pnVertexCount[nType] = nVertCount;
				m_pnIndexCount[nType] = nIndexCount;

				return 0;
			}
			else
			{
				return -1;
			}
		}

		//Static Meshes
		default:
		{
			if (input.is_open())
			{
				TVertex* ptVertices;

				input.read((char*)&nVertCount, sizeof(int));

				
				ptVertices = new TVertex[nVertCount];
				input.read((char*)ptVertices, sizeof(TVertex) * nVertCount);

				input.read((char*)&nIndexCount, sizeof(int));
				pnIndices = new int[nIndexCount];
				input.read((char*)pnIndices, sizeof(int) * nIndexCount);

				input.close();

				HRESULT hr;

				D3D11_BUFFER_DESC bDesc = { 0 };
				bDesc.Usage = D3D11_USAGE_DYNAMIC;
				bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				
				bDesc.ByteWidth = sizeof(TVertex) * nVertCount;
				bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = ptVertices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dVertexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateVertexBuffer", L"Error", MB_OK);
					exit(0);
				}

				bDesc.ByteWidth = sizeof(int) * nIndexCount;
				bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				data.pSysMem = pnIndices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dIndexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateIndexBuffer", L"Error", MB_OK);
					exit(0);
				}

				delete[] ptVertices;
				delete[] pnIndices;

				m_pnVertexCount[nType] = nVertCount;
				m_pnIndexCount[nType] = nIndexCount;

				return 0;
			}
			else
			{
				return -1;
			}

		}
	}

	
}

void CRenderManager::LoadTextureFromFile(const char * pchFilepath, int ntType)
{
	std::wstring temp(&pchFilepath[0], &pchFilepath[strlen(pchFilepath)]);

	VALIDATE(CreateWICTextureFromFile(m_pd3dDevice, temp.c_str(), nullptr, &m_pd3dSRV[ntType]), temp.c_str());
}

void CRenderManager::CreateResources(int nWidth, int nHeight, bool bReleaseResources)
{
	m_pd3dDeviceContext->OMSetRenderTargets(0, 0, 0);

#pragma region Viewport

	m_pd3dViewport[eViewport::DEFAULT].Width = (float)nWidth;
	m_pd3dViewport[eViewport::DEFAULT].Height = (float)nHeight;
	m_pd3dViewport[eViewport::DEFAULT].MinDepth = 0.0f;
	m_pd3dViewport[eViewport::DEFAULT].MaxDepth = 1.0f;
	m_pd3dViewport[eViewport::DEFAULT].TopLeftX = 0;
	m_pd3dViewport[eViewport::DEFAULT].TopLeftY = 0;
	m_pd3dDeviceContext->RSSetViewports(1, &m_pd3dViewport[eViewport::DEFAULT]);

#pragma endregion

#pragma region Releasing objects

	if(bReleaseResources)
	{
		const float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTarget[eRenderTargetView::BACKBUFFER], color);

		// Release stencil state
		for(int i = 0; i < eDepthStencilState::COUNT; i++)
		{
			SAFE_RELEASE(m_pd3dDepthStencilState[i]);
		}

		// Release Depth Stencil View's
		for(int i = 0; i < eDepthStencilView::COUNT; i++)
		{
			SAFE_RELEASE(m_pd3dDepthStencilView[i]);
		}

		// Release render target views
		for(int i = 0; i < eRenderTargetView::COUNT; i++)
		{
			SAFE_RELEASE(m_pd3dRenderTarget[i]);
		}

		// Release render target texture2D's
		for(size_t i = 0; i < eTexture::COUNT; i++)
		{
			SAFE_RELEASE(m_pd3dTexture[i]);
		}

		SAFE_RELEASE(m_pd3dSRV[eSRV::SCENE]);
		SAFE_RELEASE(m_pd3dSRV[eSRV::BACKBUFFER]);

		SAFE_RELEASE(m_pd3dSRV[eSRV::GLOW_1]);
		SAFE_RELEASE(m_pd3dSRV[eSRV::GLOW_2]);

		//SAFE_RELEASE(m_pd3dUAV[eUAV::SCENE]);
	}

#pragma endregion

	// Resize swap chain
#if _DEBUG
	VALIDATE(m_pd3dSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, D3D11_CREATE_DEVICE_DEBUG), L"SwapChainResize");
#else
	VALIDATE(m_pd3dSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0), L"SwapChainResize");
#endif

#pragma region Depth Stencil

	ID3D11Texture2D* pd3dDepthStencil = nullptr;
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = nWidth;
	depthDesc.Height = nHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	VALIDATE(m_pd3dDevice->CreateTexture2D(&depthDesc, nullptr, &pd3dDepthStencil), L"CreateTexture2D");

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;

	VALIDATE(m_pd3dDevice->CreateDepthStencilView(pd3dDepthStencil, &descDSV, &m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]), L"CreateDepthStencilView");

	pd3dDepthStencil->Release();

	VALIDATE(m_pd3dDevice->CreateTexture2D(&depthDesc, nullptr, &pd3dDepthStencil), L"CreateTexture2D");
	VALIDATE(m_pd3dDevice->CreateDepthStencilView(pd3dDepthStencil, &descDSV, &m_pd3dDepthStencilView[eDepthStencilView::GLOW]), L"CreateDepthStencilView");
	pd3dDepthStencil->Release();


	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;


	VALIDATE(m_pd3dDevice->CreateDepthStencilState(&dsDesc, &m_pd3dDepthStencilState[eDepthStencilState::DEFAULT]), L"CreateDepthStencilState");

	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	VALIDATE(m_pd3dDevice->CreateDepthStencilState(&dsDesc, &m_pd3dDepthStencilState[eDepthStencilState::DEPTH_ALWAYS]), L"CreateDepthStencilState");

	// eDepthStencilState::DRAW_OUTLINE
	dsDesc.DepthEnable = true;
	dsDesc.StencilEnable = true;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	VALIDATE(m_pd3dDevice->CreateDepthStencilState(&dsDesc, &m_pd3dDepthStencilState[eDepthStencilState::STENCIL_ALWAYS]), L"CreateDepthStencilState");

	// eDepthStencilState::APPLY_OUTLINE
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	VALIDATE(m_pd3dDevice->CreateDepthStencilState(&dsDesc, &m_pd3dDepthStencilState[eDepthStencilState::STENCIL_EQUAL]), L"CreateDepthStencilState");


#pragma endregion

#pragma region Texture2D

	// Scene textures

	D3D11_TEXTURE2D_DESC sceneTextureDesc = { 0 };
	sceneTextureDesc.Width = nWidth;
	sceneTextureDesc.Height = nHeight;
	sceneTextureDesc.MipLevels = 1;
	sceneTextureDesc.ArraySize = 1;
	sceneTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sceneTextureDesc.SampleDesc.Count = 1;
	sceneTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	sceneTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE ;

	VALIDATE(m_pd3dDevice->CreateTexture2D(&sceneTextureDesc, nullptr, &m_pd3dTexture[eTexture::SCENE]), L"Scene Texture2D Creation Failed.");

	// Glow textures
	D3D11_TEXTURE2D_DESC glowTextureDesc = { 0 };

	glowTextureDesc.Width = nWidth;
	glowTextureDesc.Height = nHeight;
	glowTextureDesc.MipLevels = 1;
	glowTextureDesc.ArraySize = 1;
	glowTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	glowTextureDesc.SampleDesc.Count = 1;
	glowTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	glowTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	VALIDATE(m_pd3dDevice->CreateTexture2D(&glowTextureDesc, nullptr, &m_pd3dTexture[eTexture::GLOW_1]), L"Glow Texture2D 1 Creation Failed.");

	VALIDATE(m_pd3dDevice->CreateTexture2D(&glowTextureDesc, nullptr, &m_pd3dTexture[eTexture::GLOW_2]), L"Glow Texture2D 2 Creation Failed.");

	// Backbuffer
	m_pd3dSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (LPVOID*)&m_pd3dTexture[eTexture::BACKBUFFER]);

#pragma endregion

#pragma region Render Target View

	// Scene rtv
	D3D11_RENDER_TARGET_VIEW_DESC sceneRtvDesc{ };
	sceneRtvDesc.Format = sceneTextureDesc.Format;
	sceneRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	sceneRtvDesc.Texture2D.MipSlice = 0;

	VALIDATE(m_pd3dDevice->CreateRenderTargetView(m_pd3dTexture[eTexture::SCENE],
												  &sceneRtvDesc,
												  &m_pd3dRenderTarget[eRenderTargetView::SCENE_TARGET]),
			 L"Scene RTV Creation Failed.");

	// Glow rtv
	D3D11_RENDER_TARGET_VIEW_DESC glowRtvDesc;

	glowRtvDesc.Format = glowTextureDesc.Format;
	glowRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	glowRtvDesc.Texture2D.MipSlice = 0;

	VALIDATE(m_pd3dDevice->CreateRenderTargetView(m_pd3dTexture[eTexture::GLOW_1],
												  &glowRtvDesc,
												  &m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_1]),
			 L"Glow RTV 1 Creation Failed.");


	VALIDATE(m_pd3dDevice->CreateRenderTargetView(m_pd3dTexture[eTexture::GLOW_2],
												  &glowRtvDesc,
												  &m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_2]),
			 L"Glow RTV 2 Creation Failed.");


	// final rtv 
	VALIDATE(m_pd3dDevice->CreateRenderTargetView(m_pd3dTexture[eTexture::BACKBUFFER],
												  NULL,
												  &m_pd3dRenderTarget[eRenderTargetView::BACKBUFFER]),
			 L"Final RTV Creation Failed.");

#pragma endregion

#pragma region SRV

	// Scene buffer srv
	D3D11_SHADER_RESOURCE_VIEW_DESC sceneSrvDesc = {  };
	sceneSrvDesc.Format = sceneTextureDesc.Format;
	sceneSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sceneSrvDesc.Texture2D.MipLevels = 1;
	sceneSrvDesc.Texture2D.MostDetailedMip = 0;
	VALIDATE(m_pd3dDevice->CreateShaderResourceView((ID3D11Resource*)m_pd3dTexture[eTexture::SCENE], &sceneSrvDesc, &m_pd3dSRV[eSRV::SCENE]),
			 L"Scene SRV creation failed.");

	// Glow buffer srv

	D3D11_SHADER_RESOURCE_VIEW_DESC glowSrvDesc;
	glowSrvDesc.Format = glowTextureDesc.Format;
	glowSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	glowSrvDesc.Texture2D.MipLevels = 1;
	glowSrvDesc.Texture2D.MostDetailedMip = 0;

	VALIDATE(m_pd3dDevice->CreateShaderResourceView(m_pd3dTexture[eTexture::GLOW_1], &glowSrvDesc, &m_pd3dSRV[eSRV::GLOW_1]),
			 L"Glow SRV 1 Creation Failed.");

	VALIDATE(m_pd3dDevice->CreateShaderResourceView(m_pd3dTexture[eTexture::GLOW_2], &glowSrvDesc, &m_pd3dSRV[eSRV::GLOW_2]),
			 L"Glow SRV 2 Creation Failed.");

	// Backbuffer srv

	D3D11_SHADER_RESOURCE_VIEW_DESC bBufferDesc;
	bBufferDesc.Format = sceneTextureDesc.Format;
	bBufferDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	bBufferDesc.Texture2D.MipLevels = 1;
	bBufferDesc.Texture2D.MostDetailedMip = 0;

	VALIDATE(m_pd3dDevice->CreateShaderResourceView(m_pd3dTexture[eTexture::BACKBUFFER], &bBufferDesc, &m_pd3dSRV[eSRV::BACKBUFFER]),
			 L"Backbuffer SRV Creation Failed.");

#pragma endregion

#pragma region UAV

	D3D11_UNORDERED_ACCESS_VIEW_DESC sceneUAVDesc{ };
	sceneUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	sceneUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	sceneUAVDesc.Texture2D.MipSlice = 0;

	//VALIDATE(m_pd3dDevice->CreateUnorderedAccessView(m_pd3dTexture[eTexture::SCENE], &sceneUAVDesc, &m_pd3dUAV[eUAV::SCENE]),
	//		 L"Scene UAV Creation Failed");

#pragma endregion
}

void CRenderManager::RenderMesh(CMeshRenderer * pcMesh)
{
	UINT strides;
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE data = { 0 };
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 0);

	#pragma region	Animations

	CAnimatorComponent* pcEntityAnimator = pcMesh->m_pcOwner->GetComponent<CAnimatorComponent>();
	if(pcEntityAnimator != nullptr)
	{
		strides = sizeof(TSkinnedVertex);

		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(TAnimationBuffer), m_pcAnimatorManager->GetAnimationData(pcEntityAnimator), sizeof(TAnimationBuffer));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES], 0);

		m_pd3dDeviceContext->VSSetConstantBuffers(2, 1, &m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES]);
		m_pd3dDeviceContext->VSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::ANIMATION_SAMPLER]);
		m_pd3dDeviceContext->VSSetShaderResources(0, 2, m_pcAnimatorManager->GetAnimationTexture(pcEntityAnimator->GetAnimator()));
	}
	else
	{
		strides = sizeof(TVertex);
	}

	#pragma endregion

	#pragma region MVP

	CTransform* pcEntityTransform = pcMesh->m_pcOwner->GetComponent<CTransform>();

	TModelConstantBuffer TModelBuff;
	TModelBuff.mWorldMatrix = XMMATRIX(pcEntityTransform->GetMatrix().mData);

	m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy_s(data.pData, sizeof(TModelBuff), &TModelBuff, sizeof(TModelBuff));
	m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0);
	m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::MODEL]);

	#pragma endregion

	#pragma region Lights

	if(pcMesh->m_pcOwner->m_nEntityType != eEntity::PARTICLE)
	{
		std::vector<CLightComponent*> ptRoomLights;

		if(pcMesh->m_pcOwner->m_nEntityType == eEntity::ROOM)
		{
			ptRoomLights = ( (CRoomEntity*)pcMesh->m_pcOwner )->GetLights();
		}
		else if(pcMesh->m_pcOwner->m_pcRoom)
		{
			ptRoomLights = pcMesh->m_pcOwner->m_pcRoom->GetLights();
		}

		TLightsConstantBuffer tLightsBuffer;

		tLightsBuffer.vBufferData[0] = (float)ptRoomLights.size();	//Light count
		tLightsBuffer.vBufferData[1] = pcMesh->m_pcOwner->m_nEntityType == eEntity::PLAYER || pcMesh->m_pcOwner->m_nEntityType == eEntity::ENEMY || 
									   pcMesh->m_pcOwner->m_nEntityType == eEntity::PROJECTILE ? 0.6f : 0.19f;

		XMMATRIX tempView = XMMatrixInverse(nullptr, m_tCameraBuffer.mViewMatrix);

		memcpy(tLightsBuffer.vCamPos, &tempView.r[3].m128_f32, sizeof(XMFLOAT4));


		//REMOVE THIS
		if(pcMesh->m_pcOwner->m_nEntityType == eEntity::ENVIRONMENT || pcMesh->m_pcOwner->m_nEntityType == eEntity::DOOR)
			tLightsBuffer.vPadding = bIsDissolveShaderEnabled ? 1.0f : 0.0f;
		else
			tLightsBuffer.vPadding = 0;


		if(pcMesh->m_pcOwner->m_nEntityType == eEntity::ENEMY)
		{
			IEnemyController* pEnemyController = pcMesh->m_pcOwner->GetComponent<IEnemyController>();

			if(pEnemyController)
			{
				tLightsBuffer.vPadding = pEnemyController->GetDeathTimer();
			}
		}

		if(tLightsBuffer.vBufferData[0] > 0)
		{
			TPointLight tLight;

			for(int i = 0; i < MAX_LIGHTS - ( MAX_LIGHTS - tLightsBuffer.vBufferData[0] ); i++)
			{
				if(ptRoomLights[i]->IsActive())
				{
					tLight.vColor = *(XMFLOAT4*)( &ptRoomLights[i]->GetColor() );
					tLight.vLightDesc = *(XMFLOAT4*)( &ptRoomLights[i]->GetWorldPosition() );
					tLight.vLightDesc.w = ptRoomLights[i]->GetRadius();
					tLightsBuffer.tLights[i] = tLight;
				}
			}
		}

		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::LIGHTS], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(tLightsBuffer), &tLightsBuffer, sizeof(TLightsConstantBuffer));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::LIGHTS], 0);
		m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::LIGHTS]);
	}

	#pragma endregion

	#pragma region Resources

		DXGI_SWAP_CHAIN_DESC desc;
		m_pd3dSwapChain->GetDesc(&desc);
	
		TResourceConstantBuffer tResource;
		tResource.vScreenResolution = XMFLOAT2((float)desc.BufferDesc.Width, (float)desc.BufferDesc.Height);

		for(int i = 0; i < pcMesh->GetTextureCount(); i++)
		{
			if(i > pcMesh->GetTextureCount())
				break;

			int nTextureID = pcMesh->GetTextureAt(i);

			if(nTextureID != eSRV::NONE)
			{
				m_pd3dDeviceContext->PSSetShaderResources(i, 1, &m_pd3dSRV[nTextureID]);
				tResource.bHasResource[i] = true;

			}
			else
			{
				tResource.bHasResource[i] = false;
			}
		}

		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::RESOURCES], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(tResource), &tResource, sizeof(tResource));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::RESOURCES], 0);
		m_pd3dDeviceContext->PSSetConstantBuffers(1, 1, &m_pd3dConstantBuffer[eConstantBuffer::RESOURCES]);

	#pragma endregion

	m_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer[pcMesh->GetVertexBuffer()], &strides, &offset);
	m_pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer[pcMesh->GetIndexBuffer()], DXGI_FORMAT_R32_UINT, 0);
	m_pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout[pcMesh->GetInputLayout()]);
	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[pcMesh->GetVertexShader()], nullptr, 0);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[pcMesh->GetPixelShader()], nullptr, 0);

	switch(pcMesh->GetPixelShader())
	{
		case ePixelShader::DISSOLVE:
		{
			m_pd3dDeviceContext->PSSetShaderResources(3, 1, &m_pd3dSRV[eSRV::SMOKE_NOISE]);
			m_pd3dDeviceContext->PSSetShaderResources(4, 1, &m_pd3dSRV[eSRV::LAVA_TEXTURE]);
			break;
		}

		case ePixelShader::SEE_THROUGH:
		{
			TVectorConstantBuffer effectBuffer;

			effectBuffer.vVector.x = fMaskValue;
			effectBuffer.vVector.y = 0.01807f;
			effectBuffer.vVector.z = (float)CTime::GetTotalTime();

			m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy(data.pData, &effectBuffer, sizeof(effectBuffer));
			m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

			m_pd3dDeviceContext->PSSetConstantBuffers(2, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);

			m_pd3dDeviceContext->PSSetShaderResources(3, 1, &m_pd3dSRV[eSRV::SMOKE_NOISE]);
			m_pd3dDeviceContext->PSSetShaderResources(4, 1, &m_pd3dSRV[eSRV::CLOUD_NOISE]);
			m_pd3dDeviceContext->PSSetShaderResources(5, 1, &m_pd3dSRV[eSRV::LAVA_TEXTURE]);

			break;
		}

		case ePixelShader::TORCH_FIRE:
		{
			m_pd3dDeviceContext->RSSetState(m_pd3dRasterizerState[eRasterizerState::NO_CULLING]);
		}
		
		case ePixelShader::AoE:
		{
			TVectorConstantBuffer effectBuffer;
			effectBuffer.vVector.x = (float)CTime::GetTotalTime();
			effectBuffer.vVector.y = pcMesh->m_pcOwner->GetComponent<CProjectileComponent>()->GetTimeLeft();

			m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy(data.pData, &effectBuffer, sizeof(effectBuffer));
			m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

			m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);
			
			break;
		}

		case ePixelShader::FADE_OUT:
		{
			TVectorConstantBuffer effectBuffer;
			effectBuffer.vVector.x = pcMesh->m_pcOwner->GetComponent<CIndicatorController>()->GetTime();

			m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy(data.pData, &effectBuffer, sizeof(effectBuffer));
			m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

			m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);

			break;
		}

		case ePixelShader::FLASH:
		{
			TVectorConstantBuffer effectBuffer;
			effectBuffer.vVector = XMFLOAT4(0.8f, 0.8f, 0.5f, 1);

			if(pcMesh->m_pcOwner->GetComponent<CFireDemonController>())
			{
				effectBuffer.vVector.w = pcMesh->m_pcOwner->GetComponent<CFireDemonController>()->GetFlashTime();
			}
			else
			{
				effectBuffer.vVector.w = pcMesh->m_pcOwner->GetComponent<IEnemyController>()->GetFlashTime();
			}

			m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy(data.pData, &effectBuffer, sizeof(effectBuffer));
			m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

			m_pd3dDeviceContext->PSSetConstantBuffers(2, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);

			break;
		}

	}

	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[pcMesh->GetSampler()]);

	float sampleMask[4]{0,0,0,0};

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::SCENE_TARGET], m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]);

	m_pd3dDeviceContext->OMSetBlendState(m_pd3dBlendStates[eBlendState::DEFAULT], sampleMask, 0xFFFFFFFF);

	m_pd3dDeviceContext->DrawIndexed(m_pnIndexCount[pcMesh->GetIndexBuffer()], 0, 0);

//	if(pcMesh->GetPixelShader() == ePixelShader::TORCH_FIRE)
	//	m_pd3dDeviceContext->RSSetState(m_pd3dRasterizerState[eRasterizerState::DEFAULT]);


	if(pcMesh->IsGlowing())
	{
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_1], m_pd3dDepthStencilView[eDepthStencilView::GLOW]);
		m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::STENCIL_ALWAYS], 1);

		TColorConstantBuffer tColorBuffer{ *(XMFLOAT4*)&pcMesh->GetGlowColor() };

		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::COLOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(tColorBuffer), &tColorBuffer, sizeof(tColorBuffer));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::COLOR], 0);
		m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::COLOR]);

		m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::SOLID_COLOR], nullptr, 0);

		m_pd3dDeviceContext->DrawIndexed(m_pnIndexCount[pcMesh->GetIndexBuffer()], 0, 0);

		pcMesh->SetGlowEffect(false);
	}

}

void CRenderManager::RenderParticles(XMVECTOR vCameraPos, CParticleEmitter * pcEmitter)
{
	UINT strides = sizeof(TVertex);
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE data = { 0 };
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 0);
	float sampleMask[4]{ 0,0,0,0 };

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::SCENE_TARGET], m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]);

	m_pd3dDeviceContext->OMSetBlendState(m_pd3dBlendStates[eBlendState::DEFAULT], sampleMask, 0xFFFFFFFF);


	int nParticleCount = (int)pcEmitter->m_pcParticles.size();
	float* fDistances = new float[nParticleCount];
	int* nIndices = new int[nParticleCount];

	for(int i = 0; i < nParticleCount; i++)
	{
		//Calculate Distances
		CMath::TVECTOR3 vTemp = pcEmitter->m_pcParticles[i]->GetComponent<CTransform>()->GetPosition();
		XMVECTOR particlePos;

		memcpy(&particlePos, &vTemp, sizeof(CMath::TVECTOR3));

		fDistances[i] = XMVector3Length(vCameraPos - particlePos).m128_f32[0];

		nIndices[i] = i;
	}

	if(nParticleCount > 0)
		for(int i = 0; i < nParticleCount - 1; i++)
		{
			int nMinIndex = i;

			for(int j = i + 1; j < nParticleCount; j++)
			{
				if(fDistances[nMinIndex] < fDistances[j])
					nMinIndex = j;
			}

			int nTemp = nIndices[i];
			float fTemp = fDistances[i];

			nIndices[i] = nMinIndex;
			nIndices[nMinIndex] = nTemp;

			fDistances[i] = fDistances[nMinIndex];
			fDistances[nMinIndex] = fTemp;
		}


	for(int i = 0; i < nParticleCount; i++)
	{
		D3D11_MAPPED_SUBRESOURCE data = { 0 };

		TVectorConstantBuffer effectBuffer;
		effectBuffer.vVector.y = pcEmitter->GetLife();

		m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);

		if(pcEmitter->m_pcParticles[nIndices[i]]->IsActive())
		{
			//MVP
			CTransform* pcEntityTransform = pcEmitter->m_pcParticles[nIndices[i]]->GetComponent<CTransform>();

			TModelConstantBuffer TModelBuff;
			TModelBuff.mWorldMatrix = XMMATRIX(pcEntityTransform->GetMatrix().mData);

			m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy_s(data.pData, sizeof(TModelBuff), &TModelBuff, sizeof(TModelBuff));
			m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0);
			m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::MODEL]);

			//PS Buffer

			effectBuffer.vVector.x = pcEmitter->m_pcParticles[nIndices[i]]->m_fLifetime;

			m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			memcpy(data.pData, &effectBuffer, sizeof(effectBuffer));
			m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);
			

			
			CMeshRenderer* pcMesh = pcEmitter->m_pcParticles[nIndices[i]]->GetComponent<CMeshRenderer>();

			m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[pcMesh->GetSampler()]);
			m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[pcMesh->GetTextureAt(0)]);

			m_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer[pcMesh->GetVertexBuffer()], &strides, &offset);
			m_pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer[pcMesh->GetIndexBuffer()], DXGI_FORMAT_R32_UINT, 0);
			m_pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout[pcMesh->GetInputLayout()]);
			m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[pcMesh->GetVertexShader()], nullptr, 0);
			m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[pcMesh->GetPixelShader()], nullptr, 0);

			m_pd3dDeviceContext->DrawIndexed(m_pnIndexCount[pcMesh->GetIndexBuffer()], 0, 0);
		}
	}

	delete[] fDistances;
	delete[] nIndices;
}

void CRenderManager::Draw()
{
	std::vector<CCameraController*> pcCameras = m_pcComponentManager->GetCameraController();

	const float color[] = { 0,0,0,0 };

	for(int i = 0; i < eRenderTargetView::COUNT; i++)
	{
		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTarget[i], color);
	}

	for(int i = 0; i < eDepthStencilView::COUNT; i++)
	{
		m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	if (pcCameras.size() > 0)
	{
		m_tCameraBuffer.mViewMatrix = XMMatrixInverse(nullptr, XMMATRIX(pcCameras[0]->m_pcOwner->GetComponent<CTransform>()->GetMatrix().mData));

		D3D11_MAPPED_SUBRESOURCE data = { 0 };
		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::CAMERA], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(m_tCameraBuffer), &m_tCameraBuffer, sizeof(m_tCameraBuffer));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::CAMERA], 0);
		m_pd3dDeviceContext->VSSetConstantBuffers(1, 1, &m_pd3dConstantBuffer[eConstantBuffer::CAMERA]);

		std::vector<CMeshRenderer*> meshes = m_pcComponentManager->GetMeshes();

		for (int i = 1; i < meshes.size(); i++)
		{
			if (meshes[i]->IsActive())
			{
				RenderMesh(meshes[i]);
			}
		}

		//Render player
		RenderMesh(meshes[0]);

		std::vector<CParticleEmitter*> particleEmitters = *m_pcComponentManager->GetParticleEmitters();
		
		float sampleMask[4]{ 0,0,0,0 };
		
		CMath::TVECTOR3 vTemp = pcCameras[0]->m_pcOwner->GetComponent<CTransform>()->GetPosition();
		XMVECTOR vCameraPos;
		
		memcpy(&vCameraPos, &vTemp, sizeof(CMath::TVECTOR3));
		
		for (CParticleEmitter* pcEmitter : particleEmitters)
		{
			RenderParticles(vCameraPos, pcEmitter);
		}

		//Debug lines
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::SCENE_TARGET], m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]);
		m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 0);

		m_pd3dDeviceContext->Map(m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

		memcpy(data.pData, CDebugManager::GetDebugLineVertices(), sizeof(TDebugLineMessage) * CDebugManager::GetDebugVertexCount());
		m_pd3dDeviceContext->Unmap(m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES], 0);

		TModelConstantBuffer buff;
		buff.mWorldMatrix = XMMatrixIdentity();

		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(buff), &buff, sizeof(buff));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0);


		UINT strides = sizeof(TDebugLineMessage);
		UINT offsets = 0;
		m_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES], &strides, &offsets);
		m_pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout[eInputLayout::DEBUG]);
		m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::MODEL]);

		m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::DEBUG_SHADER], 0, 0);
		m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::DEBUG_SHADER], 0, 0);
		m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		m_pd3dDeviceContext->Draw(CDebugManager::GetDebugVertexCount(), 0);
	}

	D3D11_MAPPED_SUBRESOURCE data = { 0 };

	// Post processing
	TVectorConstantBuffer brightnessBuffer;
	brightnessBuffer.vVector.x = 1;

	m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy_s(data.pData, sizeof(brightnessBuffer), &brightnessBuffer, sizeof(brightnessBuffer));
	m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

	ID3D11ShaderResourceView *const pNullSrv[1] = { NULL };
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, pNullSrv);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_2], m_pd3dDepthStencilView[eDepthStencilView::GLOW]);	
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 1);
	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::HOR_BLUR], nullptr, 0);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::GLOW_1]);

	m_pd3dDeviceContext->Draw(6, 0);

	float black[4] = { 0,0,0,0 };
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_1], black);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, pNullSrv);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_1], m_pd3dDepthStencilView[eDepthStencilView::GLOW]);
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 1);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);

	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::VER_BLUR], nullptr, 0);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::GLOW_2]);

	m_pd3dDeviceContext->Draw(6, 0);


	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::BACKBUFFER], m_pd3dDepthStencilView[eDepthStencilView::GLOW]);
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::STENCIL_EQUAL], 0);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::PASS_2D], nullptr, 0);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::GLOW_1]);
	m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);
	m_pd3dDeviceContext->Draw(6, 0);

	if(m_pcGUIManager)	m_pcGUIManager->Render();

	float sampleMask[4]{ 0,0,0,0 };
	m_pd3dDeviceContext->OMSetBlendState(m_pd3dBlendStates[eBlendState::ADDITIVE], sampleMask, 0xFFFFFFFF);

	// Brightness
	brightnessBuffer.vVector.x = m_fBrightness;
	m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy_s(data.pData, sizeof(brightnessBuffer), &brightnessBuffer, sizeof(brightnessBuffer));
	m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::BACKBUFFER], m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]);

	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::BRIGHTNESS], nullptr, 0);

	m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::SCENE]);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);

	m_pd3dDeviceContext->Draw(6, 0);

	m_pd3dSwapChain->Present(0, 0);
}

void CRenderManager::ResizeEvent(float fWidth, float fHeight)
{
	//int nCurrentMenu = m_pcGUIManager->GetMenuState();
	//delete m_pcGUIManager; m_pcGUIManager = nullptr;
	m_pcGUIManager->Release2D();

#ifdef USE_ADAPTER

	CreateResources((int)fWidth, (int)fHeight, true);


	m_pcGUIManager->ReInitialize2D(m_pd3dSwapChain, m_pd3dTexture[eTexture::SCENE]);
	m_pcGUIManager->WindowResizeEvent(m_pd3dSwapChain, m_pd3dTexture[eTexture::SCENE], fWidth, fHeight);

#endif // USE_ADAPTER
#ifndef USE_ADAPTER

	m_pd3dViewport[eViewport::DEFAULT].Width = fWidth;
	m_pd3dViewport[eViewport::DEFAULT].Height = fHeight;


	m_pd3dSwapChain->Release();

#if _DEBUG
	m_pd3dSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, D3D11_CREATE_DEVICE_DEBUG);
#else
	m_pd3dSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
#endif

	m_tCameraBuffer.mProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65), fWidth / fHeight, 0.1f, 1000.0f);

	m_pcGUIManager->WindowResizeEvent(m_pd3dSwapChain, fWidth, fHeight);

#endif // !USE_ADAPTER	
}

CRenderManager::~CRenderManager()
{

	for(int i = 0; i < eRenderTargetView::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dRenderTarget[i]);
	}

	for(int i = 0; i < eDepthStencilView::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dDepthStencilView[i]);
	}

	for(int i = 0; i < eDepthStencilState::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dDepthStencilState[i]);
	}

	for(int i = 0; i < eRasterizerState::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dRasterizerState[i]);
	}

	for(int i = 0; i < eVertexBuffer::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dVertexBuffer[i]);
	}

	for(int i = 0; i < eIndexBuffer::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dIndexBuffer[i]);
	}

	for(int i = 0; i < eConstantBuffer::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dConstantBuffer[i]);
	}

	for(int i = 0; i < eVertexShader::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dVertexShader[i]);
	}

	for(int i = 0; i < ePixelShader::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dPixelShader[i]);
	}

	for(int i = 0; i < eSRV::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dSRV[i]);
	}

	for(int i = 0; i < eUAV::COUNT; i++)
	{
		//SAFE_RELEASE(m_pd3dUAV[i]);
	}

	for (int i = 0; i < eTexture::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dTexture[i]);
	}

	for(int i = 0; i < eInputLayout::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dInputLayout[i]);
	}

	for(int i = 0; i < eSampler::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dSamplers[i]);
	}

	for (int i = 0; i < eBlendState::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dBlendStates[i]);
	}

	SAFE_RELEASE(m_pd3dDevice);

	SAFE_RELEASE(m_pd3dSwapChain);

	SAFE_RELEASE(m_pd3dDeviceContext);


	delete m_pcGUIManager;
	delete m_pcAnimatorManager;

#ifdef _DEBUG
	VALIDATE(DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL), L"ReportLiveObjects");

	SAFE_RELEASE(DebugDevice);
#endif // _DEBUG

}

void CRenderManager::DrawLogos()
{
	const float color[] = { 0,0,0,0 };

	for (int i = 0; i < eRenderTargetView::COUNT; i++)
	{
		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTarget[i], color);
	}

	for (int i = 0; i < eDepthStencilView::COUNT; i++)
	{
		m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	D3D11_MAPPED_SUBRESOURCE data = { 0 };

	// Post processing
	TVectorConstantBuffer brightnessBuffer;
	brightnessBuffer.vVector.x = 1;

	m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy_s(data.pData, sizeof(brightnessBuffer), &brightnessBuffer, sizeof(brightnessBuffer));
	m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

	float sampleMask[4]{ 0,0,0,0 };
	m_pd3dDeviceContext->OMSetBlendState(m_pd3dBlendStates[eBlendState::ADDITIVE], sampleMask, 0xFFFFFFFF);

	ID3D11ShaderResourceView *const pNullSrv[1] = { NULL };
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, pNullSrv);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_2], m_pd3dDepthStencilView[eDepthStencilView::GLOW]);
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 1);
	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::HOR_BLUR], nullptr, 0);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::GLOW_1]);

	m_pd3dDeviceContext->Draw(6, 0);

	float black[4] = { 0,0,0,0 };

	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_1], black);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, pNullSrv);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::GLOW_TARGET_1], m_pd3dDepthStencilView[eDepthStencilView::GLOW]);
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 1);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);

	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::VER_BLUR], nullptr, 0);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::GLOW_2]);

	m_pd3dDeviceContext->Draw(6, 0);


	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::BACKBUFFER], m_pd3dDepthStencilView[eDepthStencilView::GLOW]);
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::STENCIL_EQUAL], 0);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::PASS_2D], nullptr, 0);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::GLOW_1]);
	m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);

	m_pd3dDeviceContext->Draw(6, 0);


	if (m_pcGUIManager)	m_pcGUIManager->RenderLogos();

	// Brightness
	brightnessBuffer.vVector.x = m_fBrightness;
	m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy_s(data.pData, sizeof(brightnessBuffer), &brightnessBuffer, sizeof(brightnessBuffer));
	m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::VECTOR], 0);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::BACKBUFFER], m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]);

	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::FULLSCREEN_QUAD], nullptr, 0);
	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::PASS_2D], nullptr, 0);

	m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::VECTOR]);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[eSRV::SCENE]);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::CLAMP]);

	m_pd3dDeviceContext->Draw(6, 0);

	m_pd3dSwapChain->Present(0, 0);
}

void CRenderManager::AddButtonToMenu(int menu, void(*pFunction)(), const WCHAR* buttonText, int width, int height, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	
	m_pcGUIManager->AddButtonToMenu(menu, pFunction, buttonText, sd.BufferDesc.Width, sd.BufferDesc.Height, (float)width, (float)height, left, right, top, bottom);
}

void CRenderManager::AddSliderToMenu(int menu, int nVary, int nType, int width, int height, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	m_pcGUIManager->AddSliderToMenu(menu, nVary, nType, sd.BufferDesc.Width, sd.BufferDesc.Height, (float)width, (float)height, left, right, top, bottom);
}

void CRenderManager::SetTitleToMenu(int menu, const WCHAR* title_text, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	m_pcGUIManager->SetTitleToMenu(menu, title_text, sd.BufferDesc.Width, sd.BufferDesc.Height, left, right, top, bottom);
}

void CRenderManager::AddDescriptionToMenu(int menu, const WCHAR * text, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	m_pcGUIManager->AddDescriptionToMenu(menu, text, sd.BufferDesc.Width, sd.BufferDesc.Height, left, right, top, bottom);
}

void CRenderManager::ChangeCurrentMenu(int menu)
{
	m_pcGUIManager->ChangeCurrentMenu(menu);
}

int CRenderManager::GetMenuState()
{
	return m_pcGUIManager->GetMenuState();
}

float CRenderManager::GetBrightness()
{
	return (m_fBrightness - 0.2f) / 5.0f;
}

void CRenderManager::SetBrightness(float fBrightness)
{
	m_fBrightness = 5.0f * fBrightness + 0.2f;
}
