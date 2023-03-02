#include "EventManager.h"
#include "DebugManager.h"
#include "GUIManager.h"
#include "AudioManager.h"
#include "RenderManager.h"
#include "EntityManager.h"

#include "Entity.h"

#include <iostream>
#include <string>


std::queue<TDebugMessage>*				CEventManager::m_cDebugMessages;
std::queue<TDebugLineMessage>*			CEventManager::m_cDebugLineMessages;
std::queue<TAudioMessage>*				CEventManager::m_cAudioMessages;
std::queue<TWindowResizeMessage>*		CEventManager::m_cResizeMessages;
std::queue<TCollisionMessage>*			CEventManager::m_cCollisionMessages;
std::queue<THudMessage>*				CEventManager::m_cHudMessages;




#pragma region DebugMessage

//Debug Message
TDebugMessage::TDebugMessage(std::string szMessage)
{
	m_szMessageString = szMessage;
}

std::string TDebugMessage::GetDebugMessage()
{
	return m_szMessageString;
}

#pragma endregion

#pragma region DebugLine
TDebugLineMessage::TDebugLineMessage()
{
}
//DebugLineMessage
TDebugLineMessage::TDebugLineMessage(CMath::TVECTOR3 _tPosition, CMath::TVECTOR4 _tColor)
{
	m_tPosition = _tPosition;
	m_tColor = _tColor;
}

#pragma endregion

#pragma region Audio	

//Audio Message
TAudioMessage::TAudioMessage(bool _bTrue, int _nType, int _nSound)
{
	bTrue = _bTrue;
	nType = _nType;
	nSound = _nSound;
}

#pragma endregion

#pragma region EntityMessage

//Entity Message
TEntityMessage::TEntityMessage(int _nEntityID)
{
	nEntityID = _nEntityID;
}

#pragma endregion

#pragma region Collision

TCollisionMessage::TCollisionMessage(int _nCollider, int _nCollidingWith, int _nCollisionType)
{
	nCollider = _nCollider;
	nCollidingWidth = _nCollidingWith;
	nCollisionType = _nCollisionType;
}

#pragma endregion

#pragma region EventManager

CEventManager::CEventManager(CRenderManager* pcRenderer, CAudioManager* pcAudio, CEntityManager* pcEntity)
{
	m_cDebugMessages = new std::queue<TDebugMessage>;
	m_cDebugLineMessages = new std::queue<TDebugLineMessage>;
	m_cAudioMessages = new std::queue<TAudioMessage>;
	m_cResizeMessages = new std::queue<TWindowResizeMessage>;
	m_cCollisionMessages = new std::queue<TCollisionMessage>;
	m_cHudMessages = new std::queue<THudMessage>;
	m_pcRenderManager = pcRenderer;
	m_pcAudioManager = pcAudio;
	m_pcEntityManager = pcEntity;

#ifdef MULTI_THREADING

	m_nThreadCount = 0;
	m_nWorkingThreads = 0;
	m_bShutDownFlag = false;
	
	m_Threads.emplace_back(&CEventManager::ParseDebugMessage, this);
	m_Threads.emplace_back(&CEventManager::ParseDebugLineMessage, this);
	m_Threads.emplace_back(&CEventManager::ParseAudioMessage, this);
	m_Threads.emplace_back(&CEventManager::ParseCollissionMessage, this);
	m_Threads.emplace_back(&CEventManager::ParseResizeMessage, this);
	//m_Threads.emplace_back(&CEventManager::ParseHUDMessage, this);
	m_nThreadCount += 5;
	

#endif // MULTI_THREADING



}
CEventManager::~CEventManager()
{
	delete m_cDebugMessages;
	delete m_cDebugLineMessages;
	delete m_cAudioMessages;
	delete m_cResizeMessages;
	delete m_cCollisionMessages;
	delete m_cHudMessages;


#ifdef MULTI_THREADING

	m_cMainMutex.lock();
	m_bShutDownFlag = true;
	m_cGunCondition.notify_all();
	m_cMainMutex.unlock();

	for (auto& thread : m_Threads) thread.join();

#endif // MULTI_THREADING


}

//EventManager
void CEventManager::SendDebugMessage(TDebugMessage tMessage)
{
	m_cDebugMessages->push(tMessage);
}

void CEventManager::SendDebugLineMessage(TDebugLineMessage tMessage)
{
	m_cDebugLineMessages->push(tMessage);
}

void CEventManager::SendAudioMessage(TAudioMessage tMessage)
{
	m_cAudioMessages->push(tMessage);
}

void CEventManager::SendResizeMessage(TWindowResizeMessage tMessage)
{
	if (m_cResizeMessages)
	{
		m_cResizeMessages->push(tMessage);
	}
}

void CEventManager::SendCollisionMessage(TCollisionMessage tMessage)
{
	//std::cout << "Entity " << tMessage.GetCollider() << " and " << tMessage.GetCollidingWith() << " are in collision type " << tMessage.GetCollisionType() << std::endl;
	m_cCollisionMessages->push(tMessage);
}

IEntity* CEventManager::SendEntityMessage(TEntityMessage tMessage)
{
	//SendDebugMessage(TDebugMessage(std::string(std::to_string(CEntityManager::GetEntity(tMessage.GetID())->m_nEntityType) + "Returned")));
	return CEntityManager::GetEntity(tMessage.nEntityID);
}

void CEventManager::ClearMessages()
{
	while (!m_cAudioMessages->empty())
	{
		m_cAudioMessages->pop();
	}
	while (!m_cCollisionMessages->empty())
	{
		m_cCollisionMessages->pop();
	}
	while (!m_cDebugLineMessages->empty())
	{
		m_cDebugLineMessages->pop();
	}
	while (!m_cDebugMessages->empty())
	{
		m_cDebugMessages->pop();
	}
	while (!m_cResizeMessages->empty())
	{
		m_cResizeMessages->pop();
	}
}

#ifdef MULTI_THREADING

void CEventManager::ParseDebugMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);
	while (!m_bShutDownFlag)
	{
		while (!m_cDebugMessages->empty())
		{
			CDebugManager::DebugLog(
				m_cDebugMessages->front());

			m_cDebugMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseDebugLineMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);
	while (!m_bShutDownFlag)
	{
		while (!m_cDebugLineMessages->empty())
		{
			CDebugManager::AddDebugVertex(
				m_cDebugLineMessages->front());

			m_cDebugLineMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseAudioMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);
	while (!m_bShutDownFlag)
	{
		while (!m_cAudioMessages->empty())
		{
			m_pcAudioManager->ReceiveSoundRequest(
				m_cAudioMessages->front().bTrue, 
				m_cAudioMessages->front().nType, 
				m_cAudioMessages->front().nSound
			);

			m_cAudioMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseCollissionMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);
	srand((unsigned)time(NULL));
	while (!m_bShutDownFlag)
	{
		while (!m_cCollisionMessages->empty())
		{
			m_pcEntityManager->NotifyCollisionMessage(
				m_cCollisionMessages->front().nCollider, 
				m_cCollisionMessages->front().nCollidingWidth, 
				m_cCollisionMessages->front().nCollisionType
			);

			m_cCollisionMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseResizeMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);
	while (!m_bShutDownFlag)
	{
		while (!m_cResizeMessages->empty())
		{
			m_pcRenderManager->ResizeEvent(
				(float)m_cResizeMessages->front().nWidth, 
				(float)m_cResizeMessages->front().nHeight);

			m_cResizeMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseHUDMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);
	while (!m_bShutDownFlag)
	{
		while (!m_cHudMessages->empty())
		{
			CGUIManager::SetPlayerValues(
				m_cHudMessages->front().fCurrHealth, 
				m_cHudMessages->front().fMaxHealth, 
				m_cHudMessages->front().fCurrMana, 
				m_cHudMessages->front().fMaxMana
			);

			m_cHudMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

#endif // MULTI_THREADING

#pragma endregion

#pragma region WindowResize

TWindowResizeMessage::TWindowResizeMessage(unsigned int _nWidth, unsigned int _nHeight)
{
	nWidth = _nWidth;
	nHeight = _nHeight;
}

#pragma endregion

#pragma region HUD

THudMessage::THudMessage(float _fCurrHealth, float _fMaxHealth, float _fCurrMana, float _fMaxMana)
{
	fCurrHealth = _fCurrHealth;	fMaxHealth = _fMaxHealth;
	fCurrMana = _fCurrMana;		fMaxMana = _fMaxMana;
}

void CEventManager::SendHudMessage(THudMessage cMessage)
{
	m_cHudMessages->push(cMessage);
}

#pragma endregion

void CEventManager::Notify()
{
#ifdef  MULTI_THREADING

	std::unique_lock<std::mutex> cULock(m_cMainMutex);

	m_cGunCondition.notify_all();
	m_cMainWait.wait(cULock, [&](){ return m_nThreadCount == m_nWorkingThreads; });
	cULock.unlock();
	m_nWorkingThreads = 0;

#else

	while (!m_cDebugMessages->empty())
	{
		CDebugManager::DebugLog(m_cDebugMessages->front());

		m_cDebugMessages->pop();
	}

	while (!m_cDebugLineMessages->empty())
	{
		CDebugManager::AddDebugVertex(m_cDebugLineMessages->front());

		m_cDebugLineMessages->pop();
	}

	while (!m_cAudioMessages->empty())
	{

		CAudioManager::ReceiveSoundRequest(m_cAudioMessages->front().GetTruth(), m_cAudioMessages->front().GetType(), m_cAudioMessages->front().GetSound());


		m_cAudioMessages->pop();
	}

	while (!m_cResizeMessages->empty())
	{
		m_pcRenderManager->ResizeEvent(
			(float)m_cResizeMessages->front().GetWidth(),
			(float)m_cResizeMessages->front().GetHeight());

		m_cResizeMessages->pop();
	}

	while (!m_cCollisionMessages->empty())
	{
		TCollisionMessage m_tMessage = m_cCollisionMessages->front();

		CEntityManager::NotifyCollisionMessage(m_tMessage.GetCollider(), m_tMessage.GetCollidingWith(), m_tMessage.GetCollisionType());

		m_cCollisionMessages->pop();
	}

	while (!m_cHudMessages->empty())
	{
		THudMessage m_tMessage = m_cHudMessages->front();

		CGUIManager::SetPlayerValues(m_tMessage.GetCurrentHealth(), m_tMessage.GetMaxHealth(), m_tMessage.GetCurrentMana(), m_tMessage.GetMaxMana());

		m_cHudMessages->pop();
	}
#endif // !MULTI_THREADING
}