#include "AnimationManager.h"
//#include <iostream>
#include "EnumTypes.h"
#include "AnimatorComponent.h"
#include "Time.h"
//#include "InputManager.h"
#include "DDSTextureLoader.h"
//#include "iostream"

CAnimator::CAnimator()
{
	m_nAnimationCount = 0;
	m_nJointCount = 0;
	m_nMaxFrameCount = 0;
	m_ptAnimationClips = nullptr;
	m_ptBindPoseMatrices = nullptr;
}

CAnimator::~CAnimator()
{
	delete[] m_ptBindPoseMatrices;
	delete[] m_ptAnimationClips;

	m_pd3dSRV[0]->Release();
	m_pd3dSRV[1]->Release();

}

void CAnimator::LoadAnimator(const char * pchFilepath)
{
	std::fstream input;
	input.open(pchFilepath, std::ios_base::in | std::ios_base::binary);

	if(input.is_open())
	{
		input.read((char*)&m_nJointCount, sizeof(size_t));
		m_ptBindPoseMatrices = new XMMATRIX[m_nJointCount];
		input.read((char*)m_ptBindPoseMatrices, sizeof(XMMATRIX) * m_nJointCount);
		input.read((char*)&m_nAnimationCount, sizeof(int));

		m_ptAnimationClips = new TAnimationClip[m_nAnimationCount];
		for(int i = 0; i < m_nAnimationCount; i++)
		{
			input.read((char*)&m_ptAnimationClips[i], sizeof(double) + sizeof(int));
		}
	}


	input.close();

	for(int i = 0; i < m_nAnimationCount; i++)
	{
		if(m_nMaxFrameCount < m_ptAnimationClips[i].m_nFrameCount)
		{
			m_nMaxFrameCount = m_ptAnimationClips[i].m_nFrameCount;
		}
	}

}


CAnimatorManager::CAnimatorManager(ID3D11Device* pd3dDevice)
{
	m_pcAnimators = new CAnimator*[eAnimator::COUNT];

	m_pcAnimators[eAnimator::MAGE] = new CAnimator();
	m_pcAnimators[eAnimator::MAGE]->LoadAnimator("..//Assets//Animations//Enemies//Mage//MageData.bin");
	m_pcAnimators[eAnimator::MAGE]->LoadAnimationTexture(pd3dDevice, L"..//Assets//Animations//Enemies//Mage//MagePositionTexture.DDS", 
																	 L"..//Assets//Animations//Enemies//Mage//MageRotationTexture.DDS");

	m_pcAnimators[eAnimator::FIRE_DEMON] = new CAnimator();
	m_pcAnimators[eAnimator::FIRE_DEMON]->LoadAnimator("..//Assets//Animations//Player//FireGolemData.bin");
	m_pcAnimators[eAnimator::FIRE_DEMON]->LoadAnimationTexture(pd3dDevice, L"..//Assets//Animations//Player//FireGolemPositionTexture.DDS",
														 L"..//Assets//Animations//Player//FireGolemRotationTexture.DDS");

	m_pcAnimators[eAnimator::VIKING] = new CAnimator();
	m_pcAnimators[eAnimator::VIKING]->LoadAnimator("..//Assets//Animations//Enemies//Viking//VikingData.bin");
	m_pcAnimators[eAnimator::VIKING]->LoadAnimationTexture(pd3dDevice, L"..//Assets//Animations//Enemies//Viking//VikingPositionTexture.DDS",
																	   L"..//Assets//Animations//Enemies//Viking//VikingRotationTexture.DDS");

	m_pcAnimators[eAnimator::ARCHER] = new CAnimator();
	m_pcAnimators[eAnimator::ARCHER]->LoadAnimator("..//Assets//Animations//Enemies//Archer//ArcherData.bin");
	m_pcAnimators[eAnimator::ARCHER]->LoadAnimationTexture(pd3dDevice, L"..//Assets//Animations//Enemies//Archer//ArcherPositionTexture.DDS",
																	   L"..//Assets//Animations//Enemies//Archer//ArcherRotationTexture.DDS");

	m_pcAnimators[eAnimator::KNIGHT] = new CAnimator();
	m_pcAnimators[eAnimator::KNIGHT]->LoadAnimator("..//Assets//Animations//Enemies//Knight//KnightData.bin");
	m_pcAnimators[eAnimator::KNIGHT]->LoadAnimationTexture(pd3dDevice, L"..//Assets//Animations//Enemies//Knight//KnightPositionTexture.DDS",
																	   L"..//Assets//Animations//Enemies//Knight//KnightRotationTexture.DDS");
}				

CAnimatorManager::~CAnimatorManager()
{
	for(int i = 0; i < eAnimator::COUNT; i++)
	{
		delete m_pcAnimators[i];
	}

	delete[] m_pcAnimators;
}


void CAnimator::LoadAnimationTexture(ID3D11Device* pcDevice, const wchar_t * pcFilepathA, const wchar_t * pcFilepathB)
{
	HRESULT hr = CreateDDSTextureFromFile(pcDevice, pcFilepathA, ( ID3D11Resource** )nullptr, &m_pd3dSRV[0]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingAnimationTexture", L"Error", MB_OK);
		exit(0);
	}

	hr = CreateDDSTextureFromFile(pcDevice, pcFilepathB, ( ID3D11Resource** )nullptr, &m_pd3dSRV[1]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingAnimationTexture", L"Error", MB_OK);
		exit(0);
	}
}


TAnimationBuffer* CAnimatorManager::GetAnimationData(CAnimatorComponent* pcAnimComponent)
{
	int nAnimator = pcAnimComponent->GetAnimator();
	int nAnimation = pcAnimComponent->GetCurrentAnimation();
	double dCurrentTime = pcAnimComponent->GetCurrentAnimationTime();

	if(dCurrentTime < m_pcAnimators[nAnimator]->m_ptAnimationClips[nAnimation].m_dDuration)
	{
		dCurrentTime += CTime::GetDelta() * pcAnimComponent->GetSpeed();
	}
	else if(pcAnimComponent->IsLooping())
	{
		dCurrentTime = 0.0f;
	}
	else
	{
		pcAnimComponent->SetRunning(false);
	}

	pcAnimComponent->SetAnimationTime(dCurrentTime);
	
	for(int i = 0; i < m_pcAnimators[nAnimator]->m_nJointCount;i++)
	{
		m_tMatrixBuffer.m_tBindpose[i] = m_pcAnimators[nAnimator]->m_ptBindPoseMatrices[i];
	}

	m_tMatrixBuffer.fCurrentTime = (float)dCurrentTime;
	m_tMatrixBuffer.fTotalTime = (float)m_pcAnimators[nAnimator]->m_ptAnimationClips[nAnimation].m_dDuration;	

	m_tMatrixBuffer.nJointCount = m_pcAnimators[nAnimator]->m_nJointCount;
	m_tMatrixBuffer.nCurrentAnimation = nAnimation;
	m_tMatrixBuffer.nFrameCount = m_pcAnimators[nAnimator]->m_ptAnimationClips[nAnimation].m_nFrameCount;	
	
	return &m_tMatrixBuffer;
}

ID3D11ShaderResourceView*const* CAnimatorManager::GetAnimationTexture(int nAnimator)
{
	return m_pcAnimators[nAnimator]->m_pd3dSRV;
}

