#include "ParticleEffect.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "RenderManager.h"

#include "MeshRenderer.h"
#include "EnumTypes.h"
#include "Transform.h"
#include "Entity.h"
#include "Level.h"
#include "Rigidbody.h"
#include "CameraController.h"
#include "Entity.h"
#include "Transform.h"


#pragma region Set



void CParticleEmitter::SetVertexBuffer(int nBuffer)
{
	m_nVertices = nBuffer;
}

void CParticleEmitter::SetIndexBuffer(int nBuffer)
{
	m_nIndices = nBuffer;
}

void CParticleEmitter::SetTexture(int nTexture, int nSlot)
{
	m_pnTexture[nSlot] = nTexture;
}

void CParticleEmitter::SetSampler(int nSampler)
{
	m_nSampler = nSampler;
}

void CParticleEmitter::SetInputLayout(int nInputLayout)
{
	m_nInputLayout = nInputLayout;
}

void CParticleEmitter::SetVertexShader(int nVS)
{
	m_nVS = nVS;
}

void CParticleEmitter::SetPixelShader(int nPS)
{
	m_nPS = nPS;
}

void CParticleEmitter::SetMaxParticles(int nMaxParticles)
{
	m_nMaxParticles = nMaxParticles;
}

void CParticleEmitter::SetEmitterShape(int nEmitterShape)
{
	m_nEmitterShape = nEmitterShape;
}

void CParticleEmitter::SetParticlesPerEmission(int nNumParticles)
{
	m_nParticlesPerEmission = nNumParticles;
}

void CParticleEmitter::SetLife(float nLifetime)
{
	m_fLife = nLifetime;
}

void CParticleEmitter::SetParticleStartScale(CMath::TVECTOR3 vScale)
{
	m_vInitialScale = vScale;
}

void CParticleEmitter::SetParticleEndScale(CMath::TVECTOR3 vScale)
{
	m_vFinalScale = vScale;
}

void CParticleEmitter::SetEmissionRate(float nEmissionRate)
{
	m_nEmissionRate = nEmissionRate;
}

void CParticleEmitter::SetMaxRotationSpeed(float nRotationSpeed)
{
	m_fMaxRotationSpeed = nRotationSpeed;
}

void CParticleEmitter::SetMinRotationSpeed(float nRotationSpeed)
{
	m_fMinRotationSpeed = nRotationSpeed;
}

void CParticleEmitter::SetLoop(bool bLoop)
{
	m_bIsLooping = bLoop;
}

void CParticleEmitter::SetPingPong(bool bPingPong)
{
	m_bIsPingPong = bPingPong;
}

void CParticleEmitter::SetOffset(CMath::TVECTOR3 Offset)
{
	m_vOffset = Offset;
}

void CParticleEmitter::SetMaxSpeed(float fSpeed)
{
	m_fMaxSpeed = fSpeed;
}

void CParticleEmitter::SetMinSpeed(float fSpeed)
{
	m_fMinSpeed = fSpeed;
}

void CParticleEmitter::SetEmissionAngle(float fAngle)
{
	m_fEmissionAngle = fAngle;
}

void CParticleEmitter::SetMaxExtent(CMath::TVECTOR3 MaxExtent)
{
	m_vMaxExtent = MaxExtent;
}

void CParticleEmitter::SetMinExtent(CMath::TVECTOR3 MinExtent)
{
	m_vMinExtent = MinExtent;
}

void CParticleEmitter::SetEmitterDirection(CMath::TVECTOR3 vDirection)
{
	m_vEmitterDirection = vDirection;
}

void CParticleEmitter::SetMaxInitialRotation(float fAngle)
{
	m_fMaxInitialZRot = fAngle;
}

void CParticleEmitter::SetMinInitialRotation(float fAngle)
{
	m_fMinInitialZRot = fAngle;
}

#pragma endregion

CParticleEmitter::CParticleEmitter(IEntity * pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::PARTICLE_EMITTER;

	m_pcParticleRef = nullptr;

	m_nMaxParticles = 1;

	m_nParticlesPerEmission = 1;

	m_nEmitterShape = eEmitter::CONE;

	m_fLife = 1.0f;

	m_vInitialScale = CMath::TVECTOR3(1.0f, 1.0f, 1.0f);
	m_vFinalScale = CMath::TVECTOR3(1.0f, 1.0f, 1.0f);

	m_nEmissionRate = 1.0f;
	m_fSpawnTimer = 0.0f;

	m_bIsMoving = true;
	m_bIsRotating = false;
	m_bIsScaling = false;

	m_bIsLooping = true;
	m_bIsPingPong = false;


	m_vOffset = CMath::TVECTOR3(0.0f, 0.0f, 0.0f);
	m_vEmitterDirection = CMath::TVECTOR3(0.0f, 1.0f, 0.0f);
	m_fEmissionAngle = TORAD(45.0f);

	m_fMaxSpeed = m_fMinSpeed = 1.0f;
	m_fMaxRotationSpeed = m_fMinRotationSpeed = 0.0f;

	m_vMaxExtent = m_vMinExtent = CMath::TVECTOR3(0.0f, 0.0f, 0.0f);

	m_fMaxInitialZRot = m_fMinInitialZRot = 0.0f;

	m_pcEmitterTransform = m_pcOwner->GetComponent<CTransform>();

	m_bIsRunning = false;
}

void CParticleEmitter::InitializeReferenceParticle()
{
	if(m_pcParticleRef)
	{
		CEntityManager::AddEntityToDeletionQueue(m_pcParticleRef);
		m_pcParticleRef = nullptr;
	}

	m_pcParticleRef = (CParticleEntity*)CEntityManager::CreateEntity(eEntity::PARTICLE);

	CTransform* pcTransformRef = (CTransform*)CEntityManager::AddComponentToEntity(m_pcParticleRef, eComponent::TRANSFORM);

	pcTransformRef->SetMatrix( CMath::MatrixXRotation(TORAD(-45.0f)) * CMath::MatrixYRotation(TORAD(315.0f))  * CMath::MatrixScaleFromVector(m_vInitialScale) *
							  pcTransformRef->GetMatrix());

	CMeshRenderer* pcMeshReference = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcParticleRef, eComponent::MESH_RENDERER);
	pcMeshReference->SetVertexBuffer(m_nVertices);
	pcMeshReference->SetIndexBuffer(m_nIndices);
	pcMeshReference->SetTextureCount(3);

	for(int i = 0; i < 3; i++)
	{
		pcMeshReference->SetTexture(m_pnTexture[i], i);
	}

	pcMeshReference->SetInputLayout(m_nInputLayout);
	pcMeshReference->SetVertexShader(m_nVS);
	pcMeshReference->SetPixelShader(m_nPS);
	pcMeshReference->SetSampler(m_nSampler);

	CEntityManager::AddComponentToEntity(m_pcParticleRef, eComponent::RIGIDBODY);


	m_pcParticleRef->m_fLifetime = m_fLife;

	m_pcParticleRef->SetActiveState(false);

	m_pcCameraTransform = CEntityManager::GetCameraEntity()->GetComponent<CTransform>();
}

void CParticleEmitter::ResetParticle(CParticleEntity * pcParticle)
{
	CTransform* particleTransform = pcParticle->GetComponent<CTransform>();

	float x = m_vMinExtent.x + ( (float)rand() / ( RAND_MAX ) * ( m_vMaxExtent.x - m_vMinExtent.x ) );
	float y = m_vMinExtent.y + ( (float)rand() / ( RAND_MAX ) * ( m_vMaxExtent.y - m_vMinExtent.y ) );
	float z = m_vMinExtent.z + ( (float)rand() / ( RAND_MAX ) * ( m_vMaxExtent.z - m_vMinExtent.z ) );

	CMath::TVECTOR3 vPositionOffset(x, y, z);

	if(!m_bIsPingPong)
	{
		particleTransform->SetPosition(m_pcEmitterTransform->GetPosition() + CMath::Vector3Transform(m_vOffset + vPositionOffset, m_pcEmitterTransform->GetMatrix()));
		particleTransform->SetScale(m_vInitialScale);

		//particleTransform->SetMatrix(CMath::MatrixZRotation(TORAD(CMath::RandomFloat(m_fMinInitialZRot, m_fMaxInitialZRot))) * particleTransform->GetMatrix());
	}

	CRigidbody* particleRigidbody = pcParticle->GetComponent<CRigidbody>();
	CMath::TVECTOR3 vDirection;

	if(m_bIsPingPong && pcParticle->m_fLifetime == -1.0f)
	{
		particleRigidbody->SetVelocity(-particleRigidbody->GetVelocity());
	}
	else
	{
		switch(m_nEmitterShape)
		{
		case eEmitter::CONE:
		{
			float fHalfAngle = m_fEmissionAngle * .5f;
			CMath::TMATRIX mRotation = CMath::MatrixXRotation(TORAD(fHalfAngle + ( float(rand()) / ( RAND_MAX / m_fEmissionAngle ) ))) * CMath::MatrixZRotation(TORAD(fHalfAngle - float(rand()) / ( RAND_MAX / m_fEmissionAngle )));

			vDirection = CMath::Vector3Normalize(CMath::Vector3Transform(m_vEmitterDirection, mRotation));

			break;
		}

		case eEmitter::RADIAL:
		{
			vDirection = CMath::TVECTOR3(-1 + float(rand()) / ( RAND_MAX ) * 2, -1 + float(rand()) / ( RAND_MAX ) * 2, -1 + float(rand()) / ( RAND_MAX ) * 2);
			break;
		}

		case eEmitter::PLANE:
		{
			vDirection = m_vEmitterDirection;

			break;
		}

		case eEmitter::STATIC:
		default:
		{
			break;
		}

		}

		particleRigidbody->SetVelocity(vDirection * ( m_fMinSpeed + float(rand()) / ( RAND_MAX / ( m_fMaxSpeed - m_fMinSpeed ) ) ));

	}

	pcParticle->m_fLifetime = m_fLife;
	pcParticle->m_fRotationSpeed = m_fMinRotationSpeed + ( float(rand()) / ( RAND_MAX ) / ( m_fMaxRotationSpeed - m_fMinRotationSpeed ) );
}

float CParticleEmitter::GetLife()
{
	return m_fLife;
}

void CParticleEmitter::RefreshEmitterSettings()
{
	m_bIsRunning = true; 

	m_bIsMoving = m_fMaxSpeed != 0 || m_fMinSpeed != 0;
	m_bIsRotating = m_fMaxRotationSpeed != 0 || m_fMinRotationSpeed != 0;
	m_bIsScaling = m_vInitialScale != m_vFinalScale;

	for(int i = 0; i < m_pcParticles.size(); i++)
	{
		CEntityManager::AddEntityToDeletionQueue(m_pcParticles[i]);
	}

	m_pcParticles.clear();

	InitializeReferenceParticle();
}

void CParticleEmitter::Update(float fDeltaTime)
{
	m_fSpawnTimer += fDeltaTime;

	//Create new particle if needed
	if(m_fSpawnTimer >= m_nEmissionRate)
	{
		for(int i = 0; i < m_nParticlesPerEmission; i++)
		{
			if(m_pcParticles.size() < m_nMaxParticles)
			{
				CParticleEntity* pcNewParticle = (CParticleEntity*)CEntityManager::CloneEntity(m_pcParticleRef);

				ResetParticle(pcNewParticle);
				m_pcParticles.push_back(pcNewParticle);
			}
			else
			{
				break;
			}
		}

		m_fSpawnTimer = 0;
	}

	for(int i = 0; i < m_pcParticles.size(); i++)
	{
		if(!m_pcParticles[i]->IsActive())
			continue;

		m_pcParticles[i]->m_fLifetime -= fDeltaTime;
		CTransform* pParticleTransform = m_pcParticles[i]->GetComponent<CTransform>();
		pParticleTransform->LookAt(m_pcCameraTransform->GetPosition());

		if(m_nEmitterShape == eEmitter::STATIC)
			return;

		if(m_pcParticles[i]->m_fLifetime > 0.0f)
		{
			if(m_bIsRotating)
				pParticleTransform->SetMatrix(CMath::MatrixZRotation(-fDeltaTime * m_pcParticles[i]->m_fRotationSpeed) * pParticleTransform->GetMatrix());

			if(m_bIsScaling)
				pParticleTransform->SetScale(CMath::Vector3Lerp(pParticleTransform->GetScale(), m_vFinalScale, fDeltaTime));
		}
		else
		{
			if(m_bIsPingPong || m_bIsLooping)
			{
				m_pcParticles[i]->m_fLifetime = -1.0;
				ResetParticle(m_pcParticles[i]);
			}
			else
			{
				m_pcParticles[i]->SetActiveState(false);
			}
		}
	}
}

void CParticleEmitter::Disable()
{
	for (int i = 0; i < m_pcParticles.size(); i++)
	{
		m_pcParticles[i]->SetActiveState(false);
	}

	m_bIsRunning = false;
}

void CParticleEmitter::DisableOnCollision(int nNewTexture)
{
	SetEmitterShape(eEmitter::RADIAL);
	
	SetMaxSpeed(8.0f);
	SetMinSpeed(4.0f);
	SetLife(1.0f);
	SetLoop(false);
	SetMaxRotationSpeed(2.0f);
	SetMinRotationSpeed(-2.0f);

	if(nNewTexture != eSRV::NONE)
		SetTexture(nNewTexture, 0);
	
	SetMaxExtent({ 0.0f, 0.0f, 0.0f });
	SetMinExtent({ 0.0f, 0.0f, 0.0f });

	SetMaxParticles(m_nMaxParticles * 2);
	
	SetParticleStartScale(m_vInitialScale * .5F);
	SetParticleEndScale(m_vFinalScale * .5F);

	
	SetParticlesPerEmission(20);
	SetEmissionRate(.1f);
	
	RefreshEmitterSettings();
}

bool CParticleEmitter::IsRunning()
{
	return m_bIsRunning;
}


CParticleEmitter & CParticleEmitter::operator=(CParticleEmitter & cCopy)
{
	//Particle Mesh
	m_nVertices = cCopy.m_nVertices;
	m_nIndices = cCopy.m_nIndices;

	for(size_t i = 0; i < 3; i++)
	{
		m_pnTexture[i] = cCopy.m_pnTexture[i];
	}

	m_nSampler = cCopy.m_nSampler;
	m_nInputLayout = cCopy.m_nInputLayout;
	m_nVS = cCopy.m_nVS;
	m_nPS = cCopy.m_nPS;

	m_nMaxParticles = cCopy.m_nMaxParticles;

	m_nEmitterShape = cCopy.m_nEmitterShape;

	m_fLife = cCopy.m_fLife;

	m_nEmissionRate = cCopy.m_nEmissionRate;
	m_fSpawnTimer = cCopy.m_fSpawnTimer;

	m_vInitialScale = cCopy.m_vInitialScale;
	m_vFinalScale = cCopy.m_vFinalScale;

	m_fMinSpeed = cCopy.m_fMinSpeed;
	m_fMaxSpeed = cCopy.m_fMaxSpeed;

	m_fEmissionAngle = cCopy.m_fEmissionAngle;
	m_vEmitterDirection = cCopy.m_vEmitterDirection;
	m_nParticlesPerEmission = cCopy.m_nParticlesPerEmission;

	m_fMaxRotationSpeed = cCopy.m_fMaxRotationSpeed;
	m_fMinRotationSpeed = cCopy.m_fMinRotationSpeed;

	m_bIsMoving = cCopy.m_bIsMoving;
	m_bIsRotating = cCopy.m_bIsMoving;
	m_bIsScaling = cCopy.m_bIsScaling;

	m_bIsLooping = cCopy.m_bIsLooping;
	m_bIsPingPong = cCopy.m_bIsPingPong;

	m_vOffset = cCopy.m_vOffset;

	m_vMaxExtent = cCopy.m_vMaxExtent;
	m_vMinExtent = cCopy.m_vMinExtent;

	m_pcParticleRef = cCopy.m_pcParticleRef;

	return cCopy;
}

CParticleEmitter::~CParticleEmitter()
{
	CEntityManager::AddEntityToDeletionQueue(m_pcParticleRef);

	for(int i = 0; i < m_pcParticles.size(); i++)
	{
		CEntityManager::AddEntityToDeletionQueue(m_pcParticles[i]);
	}
}
