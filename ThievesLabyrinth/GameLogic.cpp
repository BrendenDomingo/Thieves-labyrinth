#include "GameLogic.h"
#include "LevelManager.h"
#include "AudioManager.h"
#include "PhysicsManager.h"
#include "AIManager.h"
#include "DebugManager.h"
#include "MeshRenderer.h"
#include "RenderManager.h"
#include "GUIManager.h"
#include "EventManager.h"

#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "AnimatorComponent.h"
#include "Entity.h"
#include "Stats.h"
#include "Transform.h"
#include "FireDemonController.h"
#include "PathSurface.h"
#include "PathAgent.h"
#include "ParticleEffect.h"
#include "LightComponent.h"
#include "Rigidbody.h"

#include "Time.h"

#include <string>

int CGameLogic::floor;
int CGameLogic::m_nCurrentState, CGameLogic::m_nSlain, CGameLogic::m_nItemsCollected;
bool CGameLogic::m_bStateInitialized;
bool CGameLogic::m_bMouseControls;
float CGameLogic::m_fTimeoutTimer;

CRenderManager* CGameLogic::m_pcRenderManager;
CLevelManager* CGameLogic::m_pcLevelManager;
CAudioManager* CGameLogic::m_pcAudioManager;
CEntityManager* CGameLogic::m_pcEntityManager;
CEventManager* CGameLogic::m_pcEventManager;

void CGameLogic::LoadSettings()
{
	std::ifstream file("Settings.txt", std::ios_base::in);
	if (file.is_open())
	{
		// Read in Audio settings
		float fInput;
		file >> fInput;
		m_pcAudioManager->ChangeMasterVol(fInput);
		file >> fInput;
		m_pcAudioManager->ChangeMusicVol(fInput);
		file >> fInput;
		m_pcAudioManager->ChangeSFXVol(fInput);

		// Read in control settings
		file >> m_bMouseControls;

		// Update options menu
		CGUIManager::SetControls(m_bMouseControls);

		//	Set gamma
		file >> fInput;
		m_pcRenderManager->SetBrightness(fInput);
	}
	else
	{
		// Defualt audio settings
		m_pcAudioManager->ChangeMusicVol(1.0f);
		m_pcAudioManager->ChangeMasterVol(1.0f);
		m_pcAudioManager->ChangeSFXVol(1.0f);

		// Default control settings
		m_bMouseControls = true;

		// Update options menu
		CGUIManager::SetControls(m_bMouseControls);

		//	Set defualt gamma // Works without this so it is not needed
		m_pcRenderManager->SetBrightness(0.128472f);
	}
	file.close();
}

void CGameLogic::SaveSettings()
{
	std::ofstream file("Settings.txt");
	if (file.is_open())
	{
		// Write out audio settings
		file << std::to_string(m_pcAudioManager->GetMasterVol()) + '\n';
		file << std::to_string(m_pcAudioManager->GetMusicVol()) + '\n';
		file << std::to_string(m_pcAudioManager->GetSFXVol()) + '\n';

		// Write out controls
		file << std::to_string(m_bMouseControls) + '\n';

		// Write out Gamma settings
		file << std::to_string(m_pcRenderManager->GetBrightness()) + '\n';
	}
	file.close();
}

void CGameLogic::OutLine()
{
	CMath::TVECTOR2 vMouse;
	CMath::TVECTOR3 vStart, vEnd;
	CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
	CInputManager::GetMouseWorldSpace(vMouse, vStart, vEnd);

	TRayHit tHit;

	if (CPhysicsManager::Raycast(vStart, CMath::Vector3Normalize(vEnd - vStart), tHit, CMath::Vector3Magnitude(vEnd - vStart), nullptr))
	{
		if (tHit.tEntity->m_nEntityType == eEntity::PLAYER || tHit.tEntity->m_nEntityType == eEntity::ENEMY || tHit.tEntity->m_nEntityType == eEntity::ITEM)
		{
			tHit.tEntity->GetComponent<CMeshRenderer>()->SetGlowEffect(true);
			CHUD::m_pcHoveredEntity = tHit.tEntity;
		}
		else
		{
			CHUD::m_pcHoveredEntity = nullptr;
		}
}
	else
	{
		CHUD::m_pcHoveredEntity = nullptr;
	}
}

void CGameLogic::CameraToPlayer()
{
	CMath::TVECTOR3 vStart, vEnd;
	CInputManager::GetMouseWorldSpace({0,0}, vStart, vEnd);
	TRayHit tHit;


	if(CPhysicsManager::Raycast(vStart, CMath::Vector3Normalize(vEnd - vStart), tHit, CMath::Vector3Magnitude(vEnd - vStart), nullptr))
	{
		if(tHit.tEntity->m_nEntityType == eEntity::ENVIRONMENT || tHit.tEntity->m_nEntityType == eEntity::DOOR)
		{
			CEntityManager::GetEntity(0)->GetComponent<CMeshRenderer>()->SetGlowEffect(true);
		}
		else
		{
			CEntityManager::GetEntity(0)->GetComponent<CMeshRenderer>()->SetGlowEffect(false);
		}
	}

	//if(bWallHit)
	//{
	//	if(m_pcRenderManager->fMaskValue > 1.0f)
	//		m_pcRenderManager->fMaskValue -= (float)CTime::GetDelta() * 2;
	//}
	//else if(m_pcRenderManager->fMaskValue < 2.0f)
	//{
	//	m_pcRenderManager->fMaskValue += (float)CTime::GetDelta() * 2;
	//}
}

void CGameLogic::SpawnItem(int nItemClass, int nItemType)
{
	CItemEntity* pcDrop = (CItemEntity*)CEntityManager::CreateEntity(eEntity::ITEM);

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcDrop, eComponent::MESH_RENDERER);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eSRV::ENEMY_ARCHER_DIFFUSE, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::CLAMP);
	pcMesh->SetGlowColor({ 1.0f, 0.9f, 0.0f, 1.0f });
	CTransform* pcTransform = (CTransform*)CEntityManager::AddComponentToEntity(pcDrop, eComponent::TRANSFORM);
	CCapsuleCollider* pcCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pcDrop, eComponent::CAPSULE_COLLIDER);
	CMath::TVECTOR3 tStart, tEnd;
	CMath::TVECTOR2 vMouse;
	CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
	CInputManager::GetMouseWorldSpace(vMouse, tStart, tEnd);

	CMath::TVECTOR3 tFloorPosition;
	CMath::TVECTOR3 tDirection = CMath::Vector3Normalize(tEnd - tStart);

	m_pcPhysicsManager->RaycastMouseToFloor(tStart, tDirection, OUT tFloorPosition);
	pcTransform->SetPosition(tFloorPosition);
	pcCollider->SetTrigger(true);
	pcCollider->SetCenter(CMath::TVECTOR3(0, 2.0f, 0));
	pcCollider->SetRadius(2.0f);
	pcCollider->SetHeight(4.0f);
	pcDrop->m_pcRoom = m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom;
	CLightComponent* pcLight = (CLightComponent*)CEntityManager::AddComponentToEntity(pcDrop, eComponent::LIGHT);
	pcDrop->m_nItemClass = nItemClass;
	switch (pcDrop->m_nItemClass)
	{
	case eItemClass::CONSUMABLE:
	{
		pcMesh->SetVertexBuffer(eVertexBuffer::SPHERE);
		pcMesh->SetIndexBuffer(eIndexBuffer::SPHERE);
		pcDrop->m_nItemType = nItemType;
		switch (nItemType)
		{
		case eConsumableItems::HEALTH:
		{
			pcLight->SetColor({ 1, 0, 0, 1 });
			break;
		}
		case eConsumableItems::MANA:
		{
			pcLight->SetColor({ 0, 0, 1, 1 });
			break;
		}
		case eConsumableItems::DAMAGE:
		{
			pcLight->SetColor({ 1, 0.5f, 0, 1 });
			break;
		}
		case eConsumableItems::HASTE:
		{
			pcLight->SetColor({ 1, 0, 1, 1 });
			break;
		}
		case eConsumableItems::RESIST:
		{
			pcLight->SetColor({ 0, 1, 0, 1 });
			break;
		}
		default:
			pcLight->SetColor({ 1, 1, 1, 1 });
			break;
		}
		break;
	}
	case eItemClass::PASSIVE:
	{
		pcMesh->SetVertexBuffer(eVertexBuffer::TORCH);
		pcMesh->SetIndexBuffer(eIndexBuffer::TORCH);
		pcDrop->m_nItemType = nItemType;
		switch (nItemType)
		{
		case ePassiveItems::RING:
		{
			pcLight->SetColor({ 1, 0, 0, 1 });
			break;
		}
		case ePassiveItems::THIMBLE:
		{
			pcLight->SetColor({ 1, 1, 0, 1 });
			break;
		}
		case ePassiveItems::BELT:
		{
			pcLight->SetColor({ 139 / 255.0f, 69 / 255.0f, 69 / 255.0f, 1 });
			break;
		}
		case ePassiveItems::GAUNTLET:
		{
			pcLight->SetColor({ 0, 1, 1, 1 });
			break;
		}
		case ePassiveItems::BOOTS:
		{
			pcLight->SetColor({ 0, 1, 0, 1 });
			break;
		}
		case ePassiveItems::HASTE:
		{
			pcLight->SetColor({ 1, 0, 1, 1 });
			break;
		}
		case ePassiveItems::HAT:
		{
			pcLight->SetColor({ 0, 0, 1, 1 });
			break;
		}
		default:
			pcLight->SetColor({ 1, 1, 1, 1 });
			break;
		}
		break;
	}
	default:
		break;
	}
	pcLight->SetPosition({ 0, 2, 0 });
	pcLight->SetRadius(8.0f);

	pcDrop->SetActiveState(true);
	m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom->AddItem(pcDrop);
}

CGameLogic::CGameLogic(HWND cWindow)
{
	m_pcComponentManager = new CComponentManager();

	m_pcEntityManager = new CEntityManager(m_pcComponentManager);

	m_pcRenderManager = new CRenderManager(cWindow, m_pcComponentManager);

	m_pcAudioManager = new CAudioManager();

	m_pcPhysicsManager = new CPhysicsManager(m_pcComponentManager);


	m_pcLevelManager = new CLevelManager();

	m_pcAIManager = new CAIManager(m_pcComponentManager);

	m_pcEventManager = new CEventManager(m_pcRenderManager, m_pcAudioManager, m_pcEntityManager);
	m_pcAudioManager->Initialize();
	m_nCurrentState = eGameState::MAIN_MENU;

	m_bStateInitialized = false;
	
	BuildOptionsMenus();
	LoadSettings();

	m_pcTime->Update();

	m_nSlain = m_nItemsCollected = 0;

}

CGameLogic::~CGameLogic()
{
	SaveSettings();

	m_pcAudioManager->ShutDown();

	delete m_pcAIManager;
	delete m_pcLevelManager;
	delete m_pcRenderManager;
	delete m_pcPhysicsManager;
	delete m_pcEventManager;
	delete m_pcEntityManager;
	delete m_pcComponentManager;
	delete m_pcAudioManager;
}

void CGameLogic::Update()
{
	if (!CGUIManager::FadingDone())
	{
		m_pcRenderManager->DrawLogos();
		m_pcTime->Update();
	}
	else
	{
	switch (m_nCurrentState)
	{
	case eGameState::MAIN_MENU:
	{
		CGUIManager::Update();
		m_pcEventManager->Notify();
		m_pcRenderManager->Draw();
		m_pcTime->Update();
		if (m_pcAudioManager->GetCurrentMusic() != eMusic::TOMB_OF_THIEVES)
		{
			std::thread(CAudioManager::ChangeCurrentMusic,eMusic::TOMB_OF_THIEVES).detach();
		}
		break;
	}

	case eGameState::LOADING_LEVEL:
	{
		//	This is because when you "CheckWin" and the player dies they go to main menu
		//	So the level needs to be deleted but you cant do it in checkwin otherwise 
		//	the other colliding entity will break the game.
		if (m_pcLevelManager->m_pcCurrentLevel)
			EndLevel();

		m_nSlain = m_nItemsCollected = 0;

		CPlayerEntity* pPlayer = (CPlayerEntity*)CEntityManager::CreateEntity(eEntity::PLAYER);
		CTransform* pPlayerTransform = (CTransform*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::TRANSFORM);

#pragma region Player
		CEntityManager::AddComponentToEntity(pPlayer, eComponent::RIGIDBODY);
		CCapsuleCollider* pPlayerCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::CAPSULE_COLLIDER);
		CMeshRenderer* pPlayerMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::MESH_RENDERER);
		CStats* pPlayerStats = (CStats*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::STATS);
		//CAnimatorComponent*	pPlayerAnimator = (CAnimatorComponent*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::ANIMATOR);
		CInventory* pPlayerInventory = (CInventory*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::INVENTORY);
		CEntityManager::AddComponentToEntity(pPlayer, eComponent::PATH_AGENT);

		CParticleEmitter* pParticleEffect = (CParticleEmitter*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::PARTICLE_EMITTER);

		pParticleEffect->SetMaxParticles(0);
		pParticleEffect->SetVertexBuffer(eVertexBuffer::QUAD);
		pParticleEffect->SetIndexBuffer(eIndexBuffer::QUAD);
		pParticleEffect->SetTexture(eSRV::FIREBALL_PARTICLE_EFFECT, 0);
		pParticleEffect->SetTexture(eSRV::NONE, 1);
		pParticleEffect->SetTexture(eSRV::NONE, 2);
		pParticleEffect->SetInputLayout(eInputLayout::DEFAULT);
		pParticleEffect->SetVertexShader(eVertexShader::DEFAULT);
		pParticleEffect->SetPixelShader(ePixelShader::PASS_2D);
		pParticleEffect->SetSampler(eSampler::WRAP);

		pParticleEffect->SetEmitterShape(eEmitter::RADIAL);
		pParticleEffect->SetMaxSpeed(2.0f);
		pParticleEffect->SetMinSpeed(1.0f);
		pParticleEffect->SetLife(3.0f);
		pParticleEffect->SetOffset({ 0, 4, 0 });
		pParticleEffect->SetLoop(false);
		pParticleEffect->SetMaxRotationSpeed(5);
		pParticleEffect->SetMinRotationSpeed(-5);

		pParticleEffect->SetParticleStartScale(CMath::TVECTOR3(.5f, 0.5f, .5f));
		pParticleEffect->SetParticleEndScale(CMath::TVECTOR3(1.5f, 1.5f, .5f));
		pParticleEffect->SetParticlesPerEmission(15);
		pParticleEffect->SetEmissionRate(1.0f);

		pPlayerCollider->SetCenter({ 0, 3.5f, 0 });
		pPlayerCollider->SetHeight(7.0f);
		pPlayerCollider->SetRadius(1.7f);

		pPlayerMesh->SetVertexBuffer(eVertexBuffer::JET);
		pPlayerMesh->SetIndexBuffer(eIndexBuffer::JET);
		pPlayerMesh->SetTextureCount(3);
		pPlayerMesh->SetTexture(eSRV::JET_DIFFUSE, 0);
		pPlayerMesh->SetTexture(eSRV::NONE, 1);
		pPlayerMesh->SetTexture(eSRV::NONE, 2);
		pPlayerMesh->SetInputLayout(eInputLayout::DEFAULT);
		pPlayerMesh->SetVertexShader(eVertexShader::DEFAULT);
		pPlayerMesh->SetPixelShader(ePixelShader::DEFAULT);
		pPlayerMesh->SetSampler(eSampler::CLAMP);
		pPlayerMesh->SetGlowColor({ 242.0f / 255.0f, 116.0f / 255.0f , 11.0f / 255.0f, 1.0f });

		pPlayerStats->SetMaxHP(15.0f);
		pPlayerStats->SetHP(pPlayerStats->GetMaxHP());
		pPlayerStats->SetMaxMP(15.0f);
		pPlayerStats->SetMP(pPlayerStats->GetMaxMP());
		pPlayerStats->SetAttackSpeed(0.5f);
		pPlayerStats->SetMovementSpeed(20.0f);
		pPlayerStats->SetBaseDamage(1.0f);

		/*pPlayerAnimator->SetAnimator(eAnimator::FIRE_DEMON);
		pPlayerAnimator->SetAnimation(ePlayerAnimation::IDLE);
		pPlayerAnimator->SetAnimationTime(0.0);*/

		CFireDemonController* pPlayerController = (CFireDemonController*)CEntityManager::AddComponentToEntity(pPlayer, eComponent::FIRE_DEMON_CONTROLLER);
		pPlayerController->UseMouseControls(m_bMouseControls);

#pragma endregion

#pragma region Camera Creation

		CCameraEntity* pMainCamera = (CCameraEntity*)m_pcEntityManager->CreateEntity(eEntity::CAMERA);
		m_pcEntityManager->AddComponentToEntity(pMainCamera, eComponent::TRANSFORM);
		m_pcEntityManager->AddComponentToEntity(pMainCamera, eComponent::RIGIDBODY);
		m_pcEntityManager->AddComponentToEntity(pMainCamera, eComponent::CAMERA_CONTROLLER);

		pParticleEffect->RefreshEmitterSettings();

	#pragma endregion
		


		// swap music for the floor
		if (rand() % 2 == 0)
		{
			std::thread(CAudioManager::ChangeCurrentMusic,eMusic::THIEVES_PLUNDER).detach();
		}
		else
		{
			std::thread(CAudioManager::ChangeCurrentMusic,eMusic::THIEVES_BOUNTY).detach();
		}
		///////////////////////

		m_nCurrentState = eGameState::PLAYING;
		m_pcRenderManager->ChangeCurrentMenu(eMenu::GAME);
		m_pcAIManager->Reset();
		//CGUIManager::NewGame();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		m_pcTime->Restart();		

		break;
	}

	case eGameState::PLAYING:
	{
		m_pcTime->Update();	

		if (!m_bStateInitialized)
		{
			// TODO Set Music to normal volume
			// TODO Play FootSteps
			// TODO Close pause menu
			m_pcRenderManager->ChangeCurrentMenu(eMenu::GAME);
			m_bStateInitialized = true;
		}
		float fDeltaTime = m_pcTime->GetDeltaF();
		
		//CGUIManager::Update();
		m_pcPhysicsManager->Update(fDeltaTime);
		//m_pcAIManager->Update(fDeltaTime);
		m_pcComponentManager->UpdateControllers(fDeltaTime);
		m_pcEventManager->Notify();

		OutLine();
		CameraToPlayer();

		m_pcRenderManager->Draw();

		CDebugManager::ClearDebugLines();
		if (CInputManager::GetKeyDown(VK_ESCAPE))
		{
			TogglePause();
		}

		m_pcEntityManager->ProcessDeletionQueue();


		// MOVED TO CHECK WIN
	/*	if (CLevel::nTotalEnemyCount == 0)
		{
			CGUIManager::UpdateEndScenario(m_nSlain, m_nItemsCollected);
			m_nCurrentState = eGameState::PAUSED;
			m_pcRenderManager->ChangeCurrentMenu(eMenu::WIN);
			EndLevel();
			PlayButtonSound();

			break;
		}*/

#if CHEATCODES

		if (CInputManager::GetKeyDown('O'))
		{			

			CGUIManager::UpdateEndScenario(m_nSlain, m_nItemsCollected);
			m_nCurrentState = eGameState::PAUSED;
			m_pcRenderManager->ChangeCurrentMenu(eMenu::WIN);
			EndLevel();
			PlayButtonSound();

			break;
		}

		if (CInputManager::GetKeyDown('N'))
		{
			bAiGrid = !bAiGrid;
		}
		if (CInputManager::GetKeyDown('L'))
		{
			CLevel::m_pcCurrentRoom->GetComponent<CPathSurface>()->CalculateWeights();
		}

		if (CInputManager::GetKeyDown('G'))
		{
			bGodMode = !bGodMode;

		}
		if (CInputManager::GetKeyDown('H'))
		{
			bInfiniteMana = !bInfiniteMana;

		}
		if (CInputManager::GetKeyDown('J'))
		{
			CEntityManager::GetEntity(0)->GetComponent<CStats>()->SetHP(CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetMaxHP());
		}

		if (bGodMode)
		{
			CEntityManager::GetEntity(0)->GetComponent<CStats>()->SetHP(CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetMaxHP());
			CEntityManager::GetEntity(0)->GetComponent<CStats>()->SetMP(CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetMaxMP());
		}
		if (bInfiniteMana)
		{
			CEntityManager::GetEntity(0)->GetComponent<CStats>()->SetMP(CEntityManager::GetEntity(0)->GetComponent<CStats>()->GetMaxMP());
		}

		if (bAiGrid)
		{
			CPathSurface* surface = CLevel::m_pcCurrentRoom->GetComponent<CPathSurface>();
			if (surface)
			{
				surface->DrawNodes();
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 0x1)
		{
			SpawnEnemy(eEnemyType::MAGE);
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 0x1)
		{
			SpawnEnemy(eEnemyType::CHICKEN);
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 0x1)
		{
			SpawnEnemy(eEnemyType::VIKING);
		}
		if (GetAsyncKeyState(VK_NUMPAD4) & 0x1)
		{
			SpawnEnemy(eEnemyType::ARCHER);
		}
		if (GetAsyncKeyState(VK_NUMPAD5) & 0x1)
		{
			SpawnEnemy(eEnemyType::KNIGHT);
		}
		if (GetAsyncKeyState(VK_NUMPAD9) & 0x1)
		{
			int nRoomEnemyCount = m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom->m_nEnemyCount;
			m_pcLevelManager->m_pcCurrentLevel->nTotalEnemyCount -= nRoomEnemyCount;
			for (int i = 0; i < nRoomEnemyCount; i++)
			{
				m_pcEntityManager->AddEntityToDeletionQueue(m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom->m_pcEnemies[i]);
			}
			m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom->m_nEnemyCount-= nRoomEnemyCount;
			m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom->m_pcEnemies.clear();
			m_pcLevelManager->m_pcCurrentLevel->UpdateDoors(m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom);
			m_pcEntityManager->ProcessDeletionQueue();
		}
		// Spawn a Consumable Item
		if (CInputManager::GetKey('Z'))
		{
			// Health
			if (CInputManager::GetKeyDown(VK_F1))
			{
				SpawnItem(eItemClass::CONSUMABLE, eConsumableItems::HEALTH);
			}
			// Mana
			if (CInputManager::GetKeyDown(VK_F2))
			{
				SpawnItem(eItemClass::CONSUMABLE, eConsumableItems::MANA);
			}
			// Damage
			if (CInputManager::GetKeyDown(VK_F3))
			{
				SpawnItem(eItemClass::CONSUMABLE, eConsumableItems::DAMAGE);
			}
			// Haste
			if (CInputManager::GetKeyDown(VK_F4))
			{
				SpawnItem(eItemClass::CONSUMABLE, eConsumableItems::HASTE);
			}
			// Resist
			if (CInputManager::GetKeyDown(VK_F5))
			{
				SpawnItem(eItemClass::CONSUMABLE, eConsumableItems::RESIST);
			}
		}
		// Spawn a passive item
		if (CInputManager::GetKey('M'))
		{
			// Ring
			if (CInputManager::GetKeyDown(VK_F1))
			{
				SpawnItem(eItemClass::PASSIVE, ePassiveItems::RING);
			}
			// Thimble
			if (CInputManager::GetKeyDown(VK_F2))
			{
				SpawnItem(eItemClass::PASSIVE, ePassiveItems::THIMBLE);
			}
			// Belt
			if (CInputManager::GetKeyDown(VK_F3))
			{
				SpawnItem(eItemClass::PASSIVE, ePassiveItems::BELT);
			}
			// Gauntlet
			if (CInputManager::GetKeyDown(VK_F4))
			{
				SpawnItem(eItemClass::PASSIVE, ePassiveItems::GAUNTLET);
			}
			// Boots
			if (CInputManager::GetKeyDown(VK_F5))
			{
				SpawnItem(eItemClass::PASSIVE, ePassiveItems::BOOTS);
			}
			// Rune
			if (CInputManager::GetKeyDown(VK_F6))
			{
				SpawnItem(eItemClass::PASSIVE, ePassiveItems::HASTE);
			}
			// Hat
			if (CInputManager::GetKeyDown(VK_F7))
			{
				SpawnItem(eItemClass::PASSIVE, ePassiveItems::HAT);
			}
		}

#endif // CHEATCODES


		break;
	}

	case eGameState::PAUSED:
	{
		if (!m_bStateInitialized)
		{
			// TODO Set music to play lower
			// TODO Stop Playing Footsteps
			// TODO Open Pause Menu
			ToPause();
			m_bStateInitialized = true;
		}
		CGUIManager::Update();
		m_pcRenderManager->Draw();
		m_pcEventManager->Notify();
		if (CInputManager::GetKeyDown(VK_ESCAPE))
		{
			if (m_pcRenderManager->GetMenuState() == eMenu::PAUSE)
			{
				TogglePause();
			}
			else if(m_pcRenderManager->GetMenuState() != eMenu::WIN && m_pcRenderManager->GetMenuState() != eMenu::LOSE)
			{
				ToPause();
			}
		}

		break;
	}

	case eGameState::END_TIMER:
	{
		m_pcTime->Update();
		m_pcRenderManager->Draw();
		float fDeltaTime = m_pcTime->GetDeltaF();

		m_pcEventManager->ClearMessages();
		m_pcComponentManager->UpdateControllers(fDeltaTime);
		m_pcAIManager->Update(fDeltaTime);
		m_pcPhysicsManager->Update(fDeltaTime);
		m_pcEventManager->ClearMessages();

		m_fTimeoutTimer -= fDeltaTime;
		
		if (m_fTimeoutTimer <= 0.0f)
		{

			if (CLevel::nTotalEnemyCount == 0)
				m_pcRenderManager->ChangeCurrentMenu(eMenu::WIN);
			else
				m_pcRenderManager->ChangeCurrentMenu(eMenu::LOSE);
			
			//m_nCurrentState = eGameState::MAIN_MENU;
		}
		CGUIManager::Update();

		break;
	}

	case eGameState::QUIT:
	{
	
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}
	}
}

void CGameLogic::BuildOptionsMenus()
{
	int x, y;
	CInputManager::GetScreenWH(x, y);

	// Create Start Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::START, StartGame, L"Play Test", x, y, 0.4f, 0.6f, 0.41f, 0.51f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START, StartOptions, L"Start Live", x, y, 0.4f, 0.6f, 0.54f, 0.64f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START, ToCredits, L"Start From File", x, y, 0.4f, 0.6f, 0.67f, 0.77f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START, ExitGame, L"Quit", x, y, 0.4f, 0.6f, 0.8f, 0.9f);
	//m_pcRenderManager->SetTitleToMenu(eMenu::START, L"Thieves Labyrinth", 0.3f, 0.7f, 0.1f, 0.3f);
	// Create Pause Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE, TogglePause, L"Resume", x, y, 0.4f, 0.6f, 0.4f, 0.5f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE, PauseOptions, L"Options", x, y, 0.4f, 0.6f, 0.6f, 0.7f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE, GoBackToMain, L"Exit To Main Menu", x, y, 0.4f, 0.6f, 0.8f, 0.9f);
	m_pcRenderManager->SetTitleToMenu(eMenu::PAUSE, L"Pause", 0.3f, 0.7f, 0.1f, 0.3f);

	// Hand the back buttons to the options menus
	m_pcRenderManager->AddButtonToMenu(eMenu::START_OPTIONS, ToStart, L"Back", x, y, 0.4f, 0.6f, 0.8f, 0.9f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE_OPTIONS, ToPause, L"Back", x, y, 0.4f, 0.6f, 0.8f, 0.9f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START_OPTIONS, StartControls, L"Controls", x, y, 0.65f, 0.85f, 0.15f, 0.25f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE_OPTIONS, PauseControls, L"Controls", x, y, 0.65f, 0.85f, 0.15f, 0.25f);
	// Set the slider stuff for the options menu linked to start
	// MASTER OF VOLUMES I'M PULLING YOUR STRINGS!!
	m_pcRenderManager->AddSliderToMenu(eMenu::START_OPTIONS, 0, eAudio::COUNT, x, y, 0.4f, 0.8f, 0.3f, 0.38f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::START_OPTIONS, L"Master Volume", 0.15f, 0.35f, 0.3f, 0.38f);
	// Please have this off, don't toggle it anymore
	m_pcRenderManager->AddSliderToMenu(eMenu::START_OPTIONS, 0, eAudio::MUSIC, x, y, 0.4f, 0.8f, 0.43f, 0.51f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::START_OPTIONS, L"Music Volume", 0.15f, 0.35f, 0.43f, 0.51f);
	// I honestly don't care what volume you have this at
	m_pcRenderManager->AddSliderToMenu(eMenu::START_OPTIONS, 0, eAudio::SFX, x, y, 0.4f, 0.8f, 0.56f, 0.64f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::START_OPTIONS, L"SFX Volume", 0.15f, 0.35f, 0.56f, 0.64f);

	m_pcRenderManager->AddSliderToMenu(eMenu::START_OPTIONS, 1, -1, x, y, 0.4f, 0.8f, 0.69f, 0.77f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::START_OPTIONS, L"Gamma", 0.15f, 0.35f, 0.69f, 0.77f);
	// GIVE IT A NAME!!
	m_pcRenderManager->SetTitleToMenu(eMenu::START_OPTIONS, L"Audio/Graphics", 0.3f, 0.7f, 0.1f, 0.3f);
	// Second verse, same as the first.  This time for the menu linked to pause
	m_pcRenderManager->AddSliderToMenu(eMenu::PAUSE_OPTIONS, 0, eAudio::COUNT, x, y, 0.4f, 0.8f, 0.3f, 0.38f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::PAUSE_OPTIONS, L"Master Volume", 0.15f, 0.35f, 0.3f, 0.38f);
	m_pcRenderManager->AddSliderToMenu(eMenu::PAUSE_OPTIONS, 0, eAudio::MUSIC, x, y, 0.4f, 0.8f, 0.43f, 0.51f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::PAUSE_OPTIONS, L"Music Volume", 0.15f, 0.35f, 0.43f, 0.51f);
	m_pcRenderManager->AddSliderToMenu(eMenu::PAUSE_OPTIONS, 0, eAudio::SFX, x, y, 0.4f, 0.8f, 0.56f, 0.64f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::PAUSE_OPTIONS, L"SFX Volume", 0.15f, 0.35f, 0.56f, 0.64f);
	m_pcRenderManager->AddSliderToMenu(eMenu::PAUSE_OPTIONS, 1, -1, x, y, 0.4f, 0.8f, 0.69f, 0.77f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::PAUSE_OPTIONS, L"Gamma", 0.15f, 0.35f, 0.69f, 0.77f);
	m_pcRenderManager->SetTitleToMenu(eMenu::PAUSE_OPTIONS, L"Audio/Graphics", 0.3f, 0.7f, 0.15f, 0.35f);
	// Description text for the controls menus will be created automatically
	m_pcRenderManager->SetTitleToMenu(eMenu::START_CONTROLS, L"Controls", 0.3f, 0.7f, 0.1f, 0.3f);
	m_pcRenderManager->SetTitleToMenu(eMenu::PAUSE_CONTROLS, L"Controls", 0.3f, 0.7f, 0.1f, 0.3f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START_CONTROLS, ToStart, L"Back", x, y, 0.25f, 0.45f, 0.8f, 0.9f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE_CONTROLS, ToPause, L"Back", x, y, 0.25f, 0.45f, 0.8f, 0.9f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START_CONTROLS, SwapControls, L"Change Controls", x, y, 0.55f, 0.75f, 0.8f, 0.9f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE_CONTROLS, SwapControls, L"Change Controls", x, y, 0.55f, 0.75f, 0.8f, 0.9f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START_CONTROLS, StartOptions, L"Audio/Graphics", x, y, 0.15f, 0.35f, 0.15f, 0.25f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE_CONTROLS, PauseOptions, L"Audio/Graphics", x, y, 0.15f, 0.35f, 0.15f, 0.25f);
	// This is to set up the text and buttons for the credits menu
	m_pcRenderManager->AddDescriptionToMenu(eMenu::CREDITS, L"Juan Alzate\nPhysics Programmer and AI Programmer");
	m_pcRenderManager->AddDescriptionToMenu(eMenu::CREDITS, L"Thian Bierschbach\nUI Development/Design, Input Detection and Item/Inventory System");
	m_pcRenderManager->AddDescriptionToMenu(eMenu::CREDITS, L"Brenden Domingo\nLevel Programmer, Level Designer and Particle Effects Programmer");
	m_pcRenderManager->AddDescriptionToMenu(eMenu::CREDITS, L"Tylor Ford\nModel Exporter, Math Engineer, Audio Programmer and Animation Programmer");
	m_pcRenderManager->AddDescriptionToMenu(eMenu::CREDITS, L"Daniel Ortuno\nGraphics Engineer, Base Architecture, Animation Programmer and Particle Effects Programmer");
	m_pcRenderManager->AddDescriptionToMenu(eMenu::CREDITS, L"Music\nJuan Garcia\njcgarciamatos@student.fullsail.edu\nhttps://jcgarciamatos.wixsite.com/juangarcia");
	m_pcRenderManager->AddDescriptionToMenu(eMenu::CREDITS, L"UI Art\nLunarCatX\nhttps://www.instagram.com/lunarcatx/");
	m_pcRenderManager->AddButtonToMenu(eMenu::CREDITS, ToStart, L"Back", x, y, 0.4f, 0.6f, 0.8f, 0.9f);
	m_pcRenderManager->SetTitleToMenu(eMenu::CREDITS, L"Thieves Labyrinth Credits", 0.3f, 0.7f, 0.0f, 0.2f);

	// Create Win Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::WIN, GoBackToMain, L"Exit To Main Menu", x, y, 0.25f, 0.45f, 0.75f, 0.85f);
	m_pcRenderManager->AddButtonToMenu(eMenu::WIN, StartGame, L"Continue to next floor", x, y, 0.55f, 0.75f, 0.75f, 0.85f);
	m_pcRenderManager->SetTitleToMenu(eMenu::WIN, L"Floor Cleared", 0.3f, 0.7f, 0.1f, 0.3f);
	// Create Lose Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::LOSE, GoBackToMain, L"Exit To Main Menu", x, y, 0.25f, 0.45f, 0.75f, 0.85f);
	m_pcRenderManager->AddButtonToMenu(eMenu::LOSE, StartGame, L"Restart", x, y, 0.55f, 0.75f, 0.75f, 0.85f);
	m_pcRenderManager->SetTitleToMenu(eMenu::LOSE, L"Game Over", 0.3f, 0.7f, 0.1f, 0.3f);
	// Setup for text on end menus (win/lose)
	m_pcRenderManager->AddDescriptionToMenu(eMenu::WIN, L"", 0.4f, 0.6f, 0.4f, 0.5f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::WIN, L"", 0.4f, 0.6f, 0.5f, 0.6f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::WIN, L"", 0.4f, 0.6f, 0.6f, 0.7f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::LOSE, L"", 0.4f, 0.6f, 0.4f, 0.5f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::LOSE, L"", 0.4f, 0.6f, 0.5f, 0.6f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::LOSE, L"", 0.4f, 0.6f, 0.6f, 0.7f);
}

void CGameLogic::StartGame()
{
	m_nCurrentState = eGameState::LOADING_LEVEL;
	PlayButtonSound();
}

void CGameLogic::ExitGame()
{
	m_nCurrentState = eGameState::QUIT;
	PlayButtonSound();
}

void CGameLogic::TogglePause()
{
	m_pcAudioManager->TogglePause();
	if (m_nCurrentState == eGameState::PLAYING)
	{
		m_nCurrentState = eGameState::PAUSED;
		CTime::Pause();
		m_bStateInitialized = false;
	}

	else if (m_nCurrentState == eGameState::PAUSED)
	{
		m_nCurrentState = eGameState::PLAYING;
		CTime::Unpause();
		m_bStateInitialized = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}
}

void CGameLogic::GoBackToMain()
{
	/*for (int i = (int)m_pcParticleSystem->ParticleEffects.size() - 1; i >= 0; i--)
	{
		m_pcParticleSystem->RemoveSystem(m_pcParticleSystem->ParticleEffects[i]);
	}*/
	delete m_pcLevelManager->m_pcCurrentLevel;
	m_pcLevelManager->m_pcCurrentLevel = nullptr;
	
	//m_pcEntityManager->ProcessDeletionQueue();
	m_pcEntityManager->DeleteAllEntities();
	m_nCurrentState = eGameState::MAIN_MENU;
	m_pcRenderManager->ChangeCurrentMenu(eMenu::START);
	
	PlayButtonSound();
}

void CGameLogic::PlayButtonSound()
{
	TAudioMessage playSound = TAudioMessage(true, eAudio::SFX, eSFX::BUTTON);
	CEventManager::SendAudioMessage(playSound);
}

void CGameLogic::StartOptions()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::START_OPTIONS);
	PlayButtonSound();
}

void CGameLogic::PauseOptions()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::PAUSE_OPTIONS);
	PlayButtonSound();
}

void CGameLogic::StartControls()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::START_CONTROLS);
	PlayButtonSound();
}

void CGameLogic::PauseControls()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::PAUSE_CONTROLS);
	PlayButtonSound();
}

void CGameLogic::ToPause()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::PAUSE);
	PlayButtonSound();
}

void CGameLogic::ToStart()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::START);
	PlayButtonSound();
}

void CGameLogic::CheckLose()
{
	CGUIManager::UpdateEndScenario(m_nSlain, m_nItemsCollected);


	m_pcEventManager->SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::LOSE));
	CPlayerEntity* player = (CPlayerEntity*)m_pcEntityManager->GetEntity(eEntity::PLAYER);
	player->GetComponent<CAnimatorComponent>()->SetAnimation(ePlayerAnimation::DEATH);
	player->GetComponent<CAnimatorComponent>()->SetAnimationTime(0.0);
	player->GetComponent<CAnimatorComponent>()->SetSpeed(1.0f);
	player->GetComponent<CAnimatorComponent>()->SetLoop(false);
	player->GetComponent<CFireDemonController>()->SetActiveState(false);

	player->GetComponent<CMeshRenderer>()->SetTexture(eSRV::FIRE_DEMON_EXTINGUISHED, 0);
	player->GetComponent<CRigidbody>()->SetVelocity({ 0,0,0 });

	m_nCurrentState = eGameState::END_TIMER;
	m_fTimeoutTimer = 2.8f;
}

void CGameLogic::CheckWin()
{
	if (CLevel::nTotalEnemyCount == 0)
	{
		CGUIManager::UpdateEndScenario(m_nSlain, m_nItemsCollected);
		m_nCurrentState = eGameState::END_TIMER;
		CPlayerEntity* player = (CPlayerEntity*)m_pcEntityManager->GetEntity(eEntity::PLAYER);
		player->GetComponent<CFireDemonController>()->SetActiveState(false);
		player->GetComponent<CRigidbody>()->SetVelocity({ 0,0,0 });

		m_fTimeoutTimer = 1.0f;
	}
}

void CGameLogic::ToCredits()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::CREDITS);
	CGUIManager::RestartScroll();
	PlayButtonSound();
}

void CGameLogic::EndLevel()
{
	m_pcEventManager->ClearMessages();
	delete m_pcLevelManager->m_pcCurrentLevel;
	m_pcLevelManager->m_pcCurrentLevel = nullptr;
	m_pcEntityManager->ProcessDeletionQueue();
	m_pcEntityManager->DeleteAllEntities();
}

void CGameLogic::SwapControls()
{
	m_bMouseControls = !m_bMouseControls;
	CGUIManager::SetControls(m_bMouseControls);
	if (CEntityManager::GetEntityCount(eEntity::PLAYER) > 0)
	{
		IEntity* player = CEntityManager::GetEntity(0);
		if (player)
		{
			CFireDemonController* flames = player->GetComponent<CFireDemonController>();
			flames->UseMouseControls(m_bMouseControls);
		}
	}
	PlayButtonSound();
}

void CGameLogic::SpawnEnemy(int nEnemyType)
{
	if (nEnemyType >= eEnemyType::COUNT)
	{
		m_pcEventManager->SendDebugMessage(TDebugMessage("Enemy type does not exist."));
		return;
	}
	CEnemyEntity* pEnemy = (CEnemyEntity*)CEntityManager::CloneEntity(CLevelManager::m_pcEnemies[nEnemyType]);
	CMath::TVECTOR3 tStart, tEnd;
	CMath::TVECTOR2 vMouse;
	CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
	CInputManager::GetMouseWorldSpace(vMouse, tStart, tEnd);

	CMath::TVECTOR3 tFloorPosition;
	CMath::TVECTOR3 tDirection = CMath::Vector3Normalize(tEnd - tStart);

	m_pcPhysicsManager->RaycastMouseToFloor(tStart, tDirection, OUT tFloorPosition);

	pEnemy->GetComponent<CTransform>()->SetPosition(tFloorPosition);
	pEnemy->GetComponent<IEnemyController>()->GetPlayerReference();
	pEnemy->m_pcRoom = m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom;
	CPathAgent* agent = pEnemy->GetComponent<CPathAgent>();
	if(agent) agent->SetSurface(pEnemy->m_pcRoom->GetComponent<CPathSurface>());

	m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom->m_nEnemyCount++;
	m_pcLevelManager->m_pcCurrentLevel->nTotalEnemyCount++;
	m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom->m_pcEnemies.push_back(pEnemy);
	m_pcLevelManager->m_pcCurrentLevel->SetDoorsTrue(m_pcLevelManager->m_pcCurrentLevel->m_pcCurrentRoom);
	pEnemy->SetActiveState(true);
}
