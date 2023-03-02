#include "AudioManager.h"
#include "EventManager.h"

#include <fstream>

GW::AUDIO::GSound* CAudioManager::m_cSoundList[SOUND_INSTANCES][eSFX::COUNT];
GW::AUDIO::GMusic* CAudioManager::m_cMusicList[eMusic::COUNT];
GW::AUDIO::GAudio* CAudioManager::m_pAudio;


float CAudioManager::m_nMasterVol, CAudioManager::m_nMusicVol, CAudioManager::m_nSFXVol;

int CAudioManager::nCurrentMusic;

CAudioManager::CAudioManager()
{

	m_bPaused = false;
	if (GW::GReturn::FAILURE == GW::AUDIO::CreateGAudio(&m_pAudio))
	{
		exit(-1);
	}
	m_pAudio->SetMasterVolume(m_nMasterVol);
}

CAudioManager::~CAudioManager()
{
	
}

void CAudioManager::Initialize()
{
	AddMusic("..//Assets//Sounds//Thieves_Plunder(Compressed).wav", eMusic::THIEVES_PLUNDER);
	AddMusic("..//Assets//Sounds//Tomb of Thieve's(compressed).wav", eMusic::TOMB_OF_THIEVES);
	AddMusic("..//Assets//Sounds//Thieve's Bounty(compressed).wav", eMusic::THIEVES_BOUNTY);
	AddSFX("..//Assets//Sounds//Switch.wav", eSFX::BUTTON);
	AddSFX("..//Assets//Sounds//footsteps.wav", eSFX::PLAYERMOVE);
	AddSFX("..//Assets//Sounds//Hit-1.wav", eSFX::HIT);
	AddSFX("..//Assets//Sounds//Hit-2.wav", eSFX::HIT2);
	AddSFX("..//Assets//Sounds//Hit-3.wav", eSFX::HIT3);
	AddSFX("..//Assets//Sounds//Hit-4.wav", eSFX::HIT4);
	AddSFX("..//Assets//Sounds//FireBall-1.wav", eSFX::FIREBALL1);
	AddSFX("..//Assets//Sounds//FireBall-2.wav", eSFX::FIREBALL2);
	AddSFX("..//Assets//Sounds//Firewall.wav", eSFX::FIREWALL);
	AddSFX("..//Assets//Sounds//Block.wav", eSFX::BLOCK);
	AddSFX("..//Assets//Sounds//Stone-1.wav", eSFX::STONE_IMPACT_1);
	AddSFX("..//Assets//Sounds//Stone-2.wav", eSFX::STONE_IMPACT_2);
	AddSFX("..//Assets//Sounds//Stone-3.wav", eSFX::STONE_IMPACT_3);
	AddSFX("..//Assets//Sounds//Stone-4.wav", eSFX::STONE_IMPACT_4);
	AddSFX("..//Assets//Sounds//Stone-5.wav", eSFX::STONE_IMPACT_5);
	AddSFX("..//Assets//Sounds//DoorOpen.wav", eSFX::DOOR_OPEN);
	AddSFX("..//Assets//Sounds//DoorClose.wav", eSFX::DOOR_CLOSE);
	AddSFX("..//Assets//Sounds//Projectile_Sound.wav", eSFX::ARROW_WOOSH);
	AddSFX("..//Assets//Sounds//Explosion.wav", eSFX::EXPLOSION);
	AddSFX("..//Assets//Sounds//Potion1.wav", eSFX::POTION);
	AddSFX("..//Assets//Sounds//Spikes.wav", eSFX::SPIKES);
	AddSFX("..//Assets//Sounds//Fuse.wav", eSFX::FUSE);
	AddSFX("..//Assets//Sounds//Death.wav", eSFX::LOSE);
	AddSFX("..//Assets//Sounds//Teleport.wav", eSFX::TELEPORT);
	AddSFX("..//Assets//Sounds//Sword.wav", eSFX::KNIGHT_ATTACK);
}

void CAudioManager::ShutDown()
{	
	m_cMusicList[nCurrentMusic]->SetVolume(0);
	m_cMusicList[nCurrentMusic]->ResumeStream();
	m_pAudio->DecrementCount();
}

void CAudioManager::AddMusic(const char* _pchFilepath, int nType, bool bPlay)
{
	GW::AUDIO::GMusic* pMusic;

	if (GW::GReturn::FAILURE == m_pAudio->CreateMusicStream(_pchFilepath, &pMusic))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music failed to initialize from: " + std::string(_pchFilepath)));
	}

	if (GW::GReturn::FAILURE == pMusic->SetVolume(m_nMusicVol))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music Vol failed to set from: " + std::string(_pchFilepath)));
	}

	/*if (GW::GReturn::FAILURE == pMusic->StreamStart(true))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music stream failed to start from: " + std::string(_pchFilepath)));
	}

	if (!bPlay)
	{
		pMusic->PauseStream();
	}*/

	m_cMusicList[nType] = pMusic;
}

void CAudioManager::ChangeMusicVol(float fVal)
{
	m_nMusicVol = fVal;
	for (size_t i = 0; i < eMusic::COUNT; i++)
	{
		if (GW::GReturn::FAILURE == m_cMusicList[i]->SetVolume(m_nMusicVol))
		{
			CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music Vol failed to change."));
		}
	}
}

void CAudioManager::AddSFX(const char * _pchFilepath, int nType)
{
	GW::AUDIO::GSound* pSFX = nullptr;
	for (size_t i = 0; i < SOUND_INSTANCES; i++)
	{
		if (GW::GReturn::FAILURE == m_pAudio->CreateSound(_pchFilepath, &pSFX))
		{
			CEventManager::SendDebugMessage(TDebugMessage("ERROR. SFX stream failed to start from: " + std::string(_pchFilepath)));
		}

		if (GW::GReturn::FAILURE == pSFX->SetVolume(m_nSFXVol))
		{
			CEventManager::SendDebugMessage(TDebugMessage("ERROR. SFX failed to set vol from: " + std::string(_pchFilepath)));
		}
		m_cSoundList[i][nType] = pSFX;
	}
}

void CAudioManager::ReceiveSoundRequest(bool bTrue, int nType, int nSound)
{
	switch (nType)
	{
	case eAudio::SFX:
	{
		bool bTemp = false;
		for (size_t i = 0; i < SOUND_INSTANCES; i++)
		{
			m_cSoundList[i][nSound]->isSoundPlaying(bTemp);
		
			//	Turn a sound on
			if (bTrue)
			{
				// Find a sound that is not playing to turn on
				if (bTemp)
					continue;

				m_cSoundList[i][nSound]->Play();
				break;
			}
			//	Turn a sound off
			else
			{
				// find a sound that is playing to turn off
				if (!bTemp)
					continue;

				m_cSoundList[i][nSound]->StopSound();
				break;
			}
		}
	}
	break;
	// not really using this one anymore, using ChangeCurrentMusic
	case eAudio::MUSIC:
	{
		if (bTrue)
		{
			m_cMusicList[nSound]->ResumeStream();
			nCurrentMusic = nSound;
		}
		else
		{
			m_cMusicList[nSound]->PauseStream();
		}
	}
	break;

	default:
		break;
	}
}

void CAudioManager::ChangeSFXVol(float fVal)
{
	m_nSFXVol = fVal;
	for (size_t j = 0; j < SOUND_INSTANCES; j++)
		for (size_t i = 0; i < eSFX::COUNT; i++)
		{
			m_cSoundList[j][i]->SetVolume(m_nSFXVol);
		}
}

void CAudioManager::ChangeMasterVol(float fVal)
{
	m_nMasterVol = fVal;
	m_pAudio->SetMasterVolume(fVal);
	ChangeMusicVol(m_nMusicVol);
	ChangeSFXVol(m_nSFXVol);
}

void CAudioManager::ChangeCurrentMusic(int nMusic)
{
	//	Store previous music played to turn off
	int nLastMusic = nCurrentMusic;
	nCurrentMusic = nMusic;

	//	Turning off presvious music then starting passed in music with looping
	m_cMusicList[nLastMusic]->StopStream();
	m_cMusicList[nCurrentMusic]->StreamStart(true);
}

int CAudioManager::GetCurrentMusic()
{
	return nCurrentMusic;
}

void CAudioManager::TogglePause()
{
	m_bPaused = !m_bPaused;
	if (!m_bPaused)
	{
		for (short i = 0; i < SOUND_INSTANCES; i++)
		{
			for (short j = 0; j < eSFX::COUNT; j++)
			{
				if (m_pbPlayingSFX[i][j])
					m_cSoundList[i][j]->Resume();
				m_pbPlayingSFX[i][j] = false;
			}
		}
	}
	else
	{
		bool bIsPlaying = false;
		for (short i = 0; i < SOUND_INSTANCES; i++)
		{
			for (short j = 0; j < eSFX::COUNT; j++)
			{
				m_cSoundList[i][j]->isSoundPlaying(bIsPlaying);
				m_pbPlayingSFX[i][j] = bIsPlaying;
				m_cSoundList[i][j]->Pause();
			}
		}
	}
}

float CAudioManager::GetMasterVol()
{
	return m_nMasterVol;
}

float CAudioManager::GetMusicVol()
{
	return m_nMusicVol;
}

float CAudioManager::GetSFXVol()
{
	return m_nSFXVol;
}
