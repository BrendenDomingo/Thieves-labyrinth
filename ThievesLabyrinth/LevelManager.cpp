#include "LevelManager.h"
#include "EventManager.h"
#include "AnimatorComponent.h"
#include "MeshRenderer.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "Entity.h"
#include "Stats.h"
#include "Transform.h"
#include "PathSurface.h"
#include "ParticleEffect.h"
#include "Component.h"
#include "LightComponent.h"
#include "PathAgent.h"

#include <vector>

CLevel* CLevelManager::m_pcCurrentLevel;
std::vector<CRoomEntity*> CLevelManager::m_pRoomsType;
std::vector<CEnemyEntity*> CLevelManager::m_pcEnemies;
std::vector<CTrapEntity*> CLevelManager::m_pcTraps;
std::vector<CSpawnEntity*> CLevelManager::m_pcSpawns;

CLevelManager::CLevelManager()
{
	
}

void CLevelManager::CreateLevel()
{
	m_pRoomsType.clear();
	m_pcEnemies.clear();

#pragma region Traps

	CTrapEntity* pcSpikes = (CTrapEntity*)CEntityManager::CreateEntity(eEntity::TRAP);
	CTransform* SpikeTransform =  (CTransform*)CEntityManager::AddComponentToEntity(pcSpikes, eComponent::TRANSFORM);

	SpikeTransform->SetScale({ 0.5f,0.5f,0.5f });

	CBoxCollider* pcSpikeCollider = (CBoxCollider*)CEntityManager::AddComponentToEntity(pcSpikes, eComponent::BOX_COLLIDER);
	pcSpikeCollider->SetCenter({ 0, 3.0f, 0 });
	pcSpikeCollider->SetExtent({ 4.5f, 4.5f, 5 });
	pcSpikeCollider->SetTrigger(true);

	CMeshRenderer* pcSpikeMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcSpikes, eComponent::MESH_RENDERER);
	pcSpikeMesh->SetVertexBuffer(eVertexBuffer::SPIKE_TRAP);
	pcSpikeMesh->SetIndexBuffer(eIndexBuffer::SPIKE_TRAP);
	pcSpikeMesh->SetTextureCount(3);
	pcSpikeMesh->SetTexture(eSRV::SPIKE_TRAP_DIFFUSE, 0);
	pcSpikeMesh->SetTexture(eSRV::NONE, 1);
	pcSpikeMesh->SetTexture(eSRV::NONE, 2);
	pcSpikeMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcSpikeMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcSpikeMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcSpikeMesh->SetSampler(eSampler::CLAMP);

	pcSpikeMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcSpikes, eComponent::MESH_RENDERER);
	pcSpikeMesh->SetVertexBuffer(eVertexBuffer::SPIKE_PLATFORM);
	pcSpikeMesh->SetIndexBuffer(eIndexBuffer::SPIKE_PLATFORM);
	pcSpikeMesh->SetTextureCount(3);
	pcSpikeMesh->SetTexture(eSRV::SPIKE_TRAP_DIFFUSE, 0);
	pcSpikeMesh->SetTexture(eSRV::NONE, 1);
	pcSpikeMesh->SetTexture(eSRV::NONE, 2);
	pcSpikeMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcSpikeMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcSpikeMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcSpikeMesh->SetSampler(eSampler::CLAMP);

	CEntityManager::AddComponentToEntity(pcSpikes, eComponent::SPIKE_TRAP_COMPONENT);

#pragma endregion

#pragma region Environment

	// Door
	CDoorEntity* Door = (CDoorEntity*)CEntityManager::CreateEntity(eEntity::DOOR);
	CEntityManager::AddComponentToEntity(Door, eComponent::TRANSFORM);

	CMeshRenderer* pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Door, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::DOOR);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::DOOR);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::DOOR_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::DOOR_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::DOOR_SPECULAR, 2);

	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::SEE_THROUGH);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	CEntityManager::AddComponentToEntity(Door, eComponent::BOX_COLLIDER);

	Door->GetComponent<CBoxCollider>()->SetCenter({ 0.7f, 9.0f, 0.0f });
	Door->GetComponent<CBoxCollider>()->SetExtent({ 0.2f, 9.0f, 10.0f });

	CEnvironmentEntity* wall = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(wall, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(wall, eComponent::BOX_COLLIDER);
	wall->GetComponent<CBoxCollider>()->SetCenter({ 0.0f, 6.0f, 0.0f });
	wall->GetComponent<CBoxCollider>()->SetExtent({ 1.4f, 6.0f, 10.0f });

	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(wall, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::WALL);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::WALL);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::WALL_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::WALL_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::WALL_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::SEE_THROUGH);
	pcMeshRender->SetSampler(eSampler::CLAMP);
	
	// Pillar
	CEnvironmentEntity* Pillar = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Pillar, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(Pillar, eComponent::BOX_COLLIDER);
	Pillar->GetComponent<CBoxCollider>()->SetCenter({ 0.0f, 6.25f, 0.0f });
	Pillar->GetComponent<CBoxCollider>()->SetExtent({ 5.0f, 6.0f, 5.0f });

	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Pillar, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::PILLAR);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::PILLAR);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::PILLAR_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::PILLAR_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::PILLAR_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::SEE_THROUGH);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	// Lights 
	CEnvironmentEntity* Light = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Light, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(Light, eComponent::LIGHT);

	CParticleEmitter* pFireEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(Light, eComponent::PARTICLE_EMITTER);

	pFireEmitter->SetMaxParticles(18);
	pFireEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
	pFireEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
	pFireEmitter->SetTexture(eSRV::TORCH_PARTICLE_EFFECT, 0);
	pFireEmitter->SetTexture(eSRV::NONE, 1);
	pFireEmitter->SetTexture(eSRV::NONE, 2);
	pFireEmitter->SetInputLayout(eInputLayout::DEFAULT);
	pFireEmitter->SetVertexShader(eVertexShader::DEFAULT);
	pFireEmitter->SetPixelShader(ePixelShader::PASS_2D);
	pFireEmitter->SetSampler(eSampler::WRAP);
	pFireEmitter->SetEmitterShape(eEmitter::PLANE);
	pFireEmitter->SetMaxSpeed(4.0f);
	pFireEmitter->SetMinSpeed(1.0f);
	pFireEmitter->SetLife(0.5f);
	pFireEmitter->SetOffset({ 1.2f, 1.8f, 0 });
	pFireEmitter->SetEmissionAngle(0);
	pFireEmitter->SetMaxRotationSpeed(1);
	pFireEmitter->SetMinRotationSpeed(-1);
	pFireEmitter->SetParticleStartScale(CMath::TVECTOR3(2, 2, .1f));
	pFireEmitter->SetParticleEndScale(CMath::TVECTOR3(.5f, .5f, .1f));
	pFireEmitter->SetMaxExtent(CMath::TVECTOR3(.2f, 0, .2f));
	pFireEmitter->SetMinExtent(CMath::TVECTOR3(-.2f, 0, -.2f));
	pFireEmitter->SetEmissionRate(0.1f);
	pFireEmitter->SetParticlesPerEmission(4);

	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Light, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::TORCH);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::TORCH);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::TORCH_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::TORCH_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	Light->GetComponent<CTransform>()->SetScale({ 3,3,3 });
	Light->GetComponent<CLightComponent>()->SetColor({ 0.886f, 0.345f, 0.133f, 1.0f });
	Light->GetComponent<CLightComponent>()->SetRadius(75.0f);
	Light->GetComponent<CLightComponent>()->SetPosition({ 0, 1, 0 });

	//Statue
	CEnvironmentEntity* pcStatue = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(pcStatue, eComponent::TRANSFORM);

	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcStatue, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::STATUE);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::STATUE);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::STATUE_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::STATUE_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::STATUE_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	CBoxCollider* pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(pcStatue, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 6, 0));
	pcColider->SetExtent(CMath::TVECTOR3(2, 7, 2));

	//Chair
	CEnvironmentEntity* pcChair = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(pcChair, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcChair, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::CHAIR);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::CHAIR);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::CHAIR_TABLE_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::CHAIR_TABLE_DIFFUSE, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(pcChair, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 0.5f, 0));
	pcColider->SetExtent(CMath::TVECTOR3(0.8f, 1, 3));

	//Flag
	CEnvironmentEntity* pcFlag = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(pcFlag, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcFlag, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::FLAG);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::FLAG);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::FLAG_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::FLAG_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	//Rock1
	CEnvironmentEntity* pcRock1 = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(pcRock1, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRock1, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::ROCK01);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::ROCK01);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::ROCKS_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::ROCKS_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::ROCKS_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(pcRock1, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 0));
	pcColider->SetExtent(CMath::TVECTOR3(15, 15, 20));

	//Rock2
	CEnvironmentEntity* pcRock2 = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(pcRock2, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRock2, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::ROCK02);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::ROCK02);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::ROCKS_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::ROCKS_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::ROCKS_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(pcRock2, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 0));
	pcColider->SetExtent(CMath::TVECTOR3(2, 2, 2));

	//Rock3
	CEnvironmentEntity* pcRock3 = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(pcRock3, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRock3, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::ROCK03);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::ROCK03);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::ROCKS_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::ROCKS_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::ROCKS_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(pcRock3, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 0));
	pcColider->SetExtent(CMath::TVECTOR3(2, 2, 2));

	//Barrel01
	CEnvironmentEntity* pcBarrel01 = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(pcBarrel01, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcBarrel01, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::BARREL01);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::BARREL01);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::BARREL_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::BARREL_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::BARREL_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(pcBarrel01, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 0));
	pcColider->SetExtent(CMath::TVECTOR3(3, 4, 2));

	//Sack
	CEnvironmentEntity* Sack = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Sack, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Sack, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::SACK);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::SACK);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::SACK_LOG_BARREL_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::SACK_LOG_BARREL_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::NONE, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(Sack, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 1));
	pcColider->SetExtent(CMath::TVECTOR3(2, 2, 2));

	//Logs
	CEnvironmentEntity* Logs = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Logs, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Logs, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::LOGS);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::LOGS);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::SACK_LOG_BARREL_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::SACK_LOG_BARREL_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::NONE, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(Logs, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 0));
	pcColider->SetExtent(CMath::TVECTOR3(3, 2, 3));

	//Cart
	CEnvironmentEntity* Cart = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Cart, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Cart, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::CART);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::CART);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::CART_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::CART_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(Cart, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 0));
	pcColider->SetExtent(CMath::TVECTOR3(3, 2, 3));

	//Chest
	CEnvironmentEntity* Chest = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Chest, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Chest, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::CHEST);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::CHEST);
	pcMeshRender->SetTextureCount(3);
	pcMeshRender->SetTexture(eSRV::CHEST_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::CHEST_NORMAL, 1);
	pcMeshRender->SetTexture(eSRV::CHEST_SPECULAR, 2);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(Chest, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 1, 0));
	pcColider->SetExtent(CMath::TVECTOR3(2, 2, 2));

	//PC
	CEnvironmentEntity* Platform_Circle = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Platform_Circle, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Platform_Circle, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::PLATFORM_CIRCLE);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::PLATFORM_CIRCLE);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::PLATFORM_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::PLATFORM_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	CCapsuleCollider* pcColiderC = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(Platform_Circle, eComponent::CAPSULE_COLLIDER);
	pcColiderC->SetRadius(10);
	pcColiderC->SetHeight(20.1f);

	//P
	CEnvironmentEntity* Platform = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Platform, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Platform, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::PLATFORM);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::PLATFORM);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::PLATFORM_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::PLATFORM_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	//A
	CEnvironmentEntity* Altar = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Altar, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Altar, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::ALTAR);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::ALTAR);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::ALTAR_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::ALTAR_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	//A
	CEnvironmentEntity* Arches = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Arches, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Arches, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::ARCHES);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::ARCHES);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::ARCHES_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::ARCHES_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	//BookCase
	CEnvironmentEntity* BookCase = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(BookCase, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(BookCase, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::BOOKCASE);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::BOOKCASE);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::BOOKCASE_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::BOOKCASE_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	pcColider = (CBoxCollider*)CEntityManager::AddComponentToEntity(BookCase, eComponent::BOX_COLLIDER);
	pcColider->SetCenter(CMath::TVECTOR3(0, 2, 0));
	pcColider->SetExtent(CMath::TVECTOR3(4, 5, 1));

	//Books
	CEnvironmentEntity* Book = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Book, eComponent::TRANSFORM);
	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Book, eComponent::MESH_RENDERER);

	pcMeshRender->SetVertexBuffer(eVertexBuffer::BOOKS);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::BOOKS);
	pcMeshRender->SetTextureCount(2);
	pcMeshRender->SetTexture(eSRV::BOOKS_DIFFUSE, 0);
	pcMeshRender->SetTexture(eSRV::BOOKS_NORMAL, 1);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);
	
	wall->SetActiveState(false);
	Door->SetActiveState(false);
	Pillar->SetActiveState(false);
	Light->SetActiveState(false);
	pcStatue->SetActiveState(false);
	pcChair->SetActiveState(false);
	pcFlag->SetActiveState(false);
	pcRock1->SetActiveState(false);
	pcRock2->SetActiveState(false);
	pcRock3->SetActiveState(false);
	pcBarrel01->SetActiveState(false);
	Sack->SetActiveState(false);
	Logs->SetActiveState(false);
	Cart->SetActiveState(false);
	Chest->SetActiveState(false);
	Platform_Circle->SetActiveState(false);
	Platform->SetActiveState(false);
	Altar->SetActiveState(false);
	Arches->SetActiveState(false);
	BookCase->SetActiveState(false);
	Book->SetActiveState(false);
	pcSpikes->SetActiveState(false);

#pragma endregion

#pragma region Spawner

	CSpawnEntity* Spawner = (CSpawnEntity*)CEntityManager::CreateEntity(eEntity::SPAWN);
	CTransform* SpawnPosition = (CTransform*)CEntityManager::AddComponentToEntity(Spawner, eComponent::TRANSFORM);
	/*CMeshRenderer* SpawnerMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Spawner, eComponent::MESH_RENDERER);
	SpawnerMesh->SetVertexBuffer(eVertexBuffer::SPAWN);
	SpawnerMesh->SetIndexBuffer(eIndexBuffer::SPAWN);
	SpawnerMesh->SetTextureCount(3);
	SpawnerMesh->SetTexture(eSRV::SPAWN, 0);
	SpawnerMesh->SetTexture(eSRV::NONE, 1);
	SpawnerMesh->SetTexture(eSRV::NONE, 2);
	SpawnerMesh->SetInputLayout(eInputLayout::DEFAULT);
	SpawnerMesh->SetVertexShader(eVertexShader::DEFAULT);
	SpawnerMesh->SetPixelShader(ePixelShader::DEFAULT);
	SpawnerMesh->SetSampler(eSampler::CLAMP);*/

	//SpawnPosition->SetScale(CMath::TVECTOR3(1, 2, 2));

	Spawner->SetActiveState(false);


#pragma endregion

#pragma region Enemys

	CEnemyEntity* pcEntity = (CEnemyEntity*)CEntityManager::CreateEntity(eEntity::ENEMY);
	// Mage
	{
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::TRANSFORM);
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::RIGIDBODY);

		CCapsuleCollider* pcMageCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::CAPSULE_COLLIDER);
		pcMageCollider->SetCenter(CMath::TVECTOR3(0, 3.5f, 0));
		pcMageCollider->SetHeight(7.0f);
		pcMageCollider->SetRadius(1.0f);

		CMeshRenderer* pcMageMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::MESH_RENDERER);
		pcMageMesh->SetVertexBuffer(eVertexBuffer::ENEMY_MAGE);
		pcMageMesh->SetIndexBuffer(eIndexBuffer::ENEMY_MAGE);
		pcMageMesh->SetTextureCount(3);
		pcMageMesh->SetTexture(eSRV::ENEMY_MAGE_DIFFUSE, 0);
		pcMageMesh->SetTexture(eSRV::ENEMY_MAGE_NORMAL, 1);
		pcMageMesh->SetTexture(eSRV::ENEMY_MAGE_SPECULAR, 2);
		pcMageMesh->SetInputLayout(eInputLayout::SKINNED);
		pcMageMesh->SetVertexShader(eVertexShader::SKINNED);
		pcMageMesh->SetPixelShader(ePixelShader::DEFAULT);
		pcMageMesh->SetSampler(eSampler::CLAMP);
		pcMageMesh->SetGlowColor({ 1,0,0,1 });

		CAnimatorComponent* pcMageAnimator = (CAnimatorComponent*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::ANIMATOR);
		pcMageAnimator->SetAnimator(eAnimator::MAGE);
		pcMageAnimator->SetAnimation(eEnemyAnimation::IDLE);
		pcMageAnimator->SetAnimationTime(0.0);

		CStats*	pcMageStats = (CStats*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::STATS);
		pcMageStats->SetMaxHP(2.0f);
		pcMageStats->SetHP(pcMageStats->GetMaxHP());
		pcMageStats->SetMaxMP(1000.0f);
		pcMageStats->SetMP(pcMageStats->GetMaxMP());
		pcMageStats->SetMovementSpeed(8.0f);
		pcMageStats->SetBaseDamage(1.0f);
		pcMageStats->SetAttackSpeed(1.0f);

		CEntityManager::AddComponentToEntity(pcEntity, eComponent::MAGE_CONTROLLER);

		pcEntity->SetActiveState(false);
	}
	m_pcEnemies.push_back(pcEntity);

	pcEntity = (CEnemyEntity*)CEntityManager::CreateEntity(eEntity::ENEMY);
	// Chicken
	{
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::TRANSFORM);
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::RIGIDBODY);


		CCapsuleCollider* pcChickenCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::CAPSULE_COLLIDER);
		pcChickenCollider->SetCenter(CMath::TVECTOR3(0, 1.5f, 0));
		pcChickenCollider->SetHeight(3.0f);
		pcChickenCollider->SetRadius(1.0f);

		CMeshRenderer* pcChickenMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::MESH_RENDERER);
		pcChickenMesh->SetVertexBuffer(eVertexBuffer::ENEMY_CHICKEN);
		pcChickenMesh->SetIndexBuffer(eIndexBuffer::ENEMY_CHICKEN);
		pcChickenMesh->SetTextureCount(3);
		pcChickenMesh->SetTexture(eSRV::ENEMY_CHICKEN_DIFFUSE, 0);
		pcChickenMesh->SetTexture(eSRV::NONE, 1);
		pcChickenMesh->SetTexture(eSRV::NONE, 2);
		pcChickenMesh->SetInputLayout(eInputLayout::DEFAULT);
		pcChickenMesh->SetVertexShader(eVertexShader::DEFAULT);
		pcChickenMesh->SetPixelShader(ePixelShader::DEFAULT);
		pcChickenMesh->SetSampler(eSampler::WRAP);
		pcChickenMesh->SetGlowColor({ 1,0,0,1 });

		CStats*	pcChickenStats = (CStats*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::STATS);
		pcChickenStats->SetMaxHP(0.1f);
		pcChickenStats->SetHP(pcChickenStats->GetMaxHP());
		pcChickenStats->SetMaxMP(0.0f);
		pcChickenStats->SetMP(pcChickenStats->GetMaxMP());
		pcChickenStats->SetMovementSpeed(15.0f);
		pcChickenStats->SetBaseDamage(3.0f);

		CParticleEmitter* pParticleEffect = (CParticleEmitter*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::PARTICLE_EMITTER);

		pParticleEffect->SetMaxParticles(15);
		pParticleEffect->SetVertexBuffer(eVertexBuffer::QUAD);
		pParticleEffect->SetIndexBuffer(eIndexBuffer::QUAD);
		pParticleEffect->SetTexture(eSRV::CHICKEN_PARTICLE, 0);
		pParticleEffect->SetTexture(eSRV::NONE, 1);
		pParticleEffect->SetTexture(eSRV::NONE, 2);
		pParticleEffect->SetInputLayout(eInputLayout::DEFAULT);
		pParticleEffect->SetVertexShader(eVertexShader::DEFAULT);
		pParticleEffect->SetPixelShader(ePixelShader::PASS_2D);
		pParticleEffect->SetSampler(eSampler::WRAP);

		pParticleEffect->SetEmitterShape(eEmitter::CONE);
		pParticleEffect->SetMaxSpeed(1.0f);
		pParticleEffect->SetMinSpeed(0.0f);
		pParticleEffect->SetLife(.4f);
		pParticleEffect->SetOffset({ 0, 1.2f, 0 });
		pParticleEffect->SetLoop(true);
		pParticleEffect->SetMaxRotationSpeed(5);
		pParticleEffect->SetMinRotationSpeed(-5);

		pParticleEffect->SetMaxRotationSpeed(5.0f);
		pParticleEffect->SetMinRotationSpeed(1.0f);

		pParticleEffect->SetParticleStartScale(CMath::TVECTOR3(1, 1, .5f));
		pParticleEffect->SetParticleEndScale(CMath::TVECTOR3(1.5f, 1.5f, .5f));

		pParticleEffect->SetEmissionRate(.1f);

		CEntityManager::AddComponentToEntity(pcEntity, eComponent::PATH_AGENT);
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::CHICKEN_CONTROLLER);

		pcEntity->SetActiveState(false);
	}
	m_pcEnemies.push_back(pcEntity);

	pcEntity = (CEnemyEntity*)CEntityManager::CreateEntity(eEntity::ENEMY);
	// Viking
	{
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::TRANSFORM);
		pcEntity->GetComponent<CTransform>()->SetScale({ 3.25f, 3.25f, 3.25f });
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::RIGIDBODY);


		CCapsuleCollider* pcVikingCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::CAPSULE_COLLIDER);
		pcVikingCollider->SetCenter(CMath::TVECTOR3(0, 3.5f, 0));
		pcVikingCollider->SetHeight(2.0f);
		pcVikingCollider->SetRadius(0.5f);

		CMeshRenderer* pcVikingMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::MESH_RENDERER);
		pcVikingMesh->SetVertexBuffer(eVertexBuffer::ENEMY_VIKING);
		pcVikingMesh->SetIndexBuffer(eIndexBuffer::ENEMY_VIKING);
		pcVikingMesh->SetTextureCount(3);
		pcVikingMesh->SetTexture(eSRV::ENEMY_VIKING_DIFFUSE, 0);
		pcVikingMesh->SetTexture(eSRV::ENEMY_VIKING_NORMAL, 1);
		pcVikingMesh->SetTexture(eSRV::ENEMY_VIKING_SPECULAR, 2);
		pcVikingMesh->SetInputLayout(eInputLayout::SKINNED);
		pcVikingMesh->SetVertexShader(eVertexShader::SKINNED);
		pcVikingMesh->SetPixelShader(ePixelShader::DEFAULT);
		pcVikingMesh->SetSampler(eSampler::CLAMP);
		pcVikingMesh->SetGlowColor({ 1,0,0,1 });

		CAnimatorComponent* pcVikingAnimator = (CAnimatorComponent*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::ANIMATOR);
		pcVikingAnimator->SetAnimator(eAnimator::VIKING);
		pcVikingAnimator->SetAnimation(eEnemyAnimation::IDLE);
		pcVikingAnimator->SetAnimationTime(0.0);

		CStats*	pcVikingStats = (CStats*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::STATS);
		pcVikingStats->SetMaxHP(4.0f);
		pcVikingStats->SetHP(4.0f);
		pcVikingStats->SetMaxMP(0.0f);
		pcVikingStats->SetMP(0.0f);
		pcVikingStats->SetMovementSpeed(10.0f);
		pcVikingStats->SetBaseDamage(1.0f);
		pcVikingStats->SetAttackSpeed(1.0f);

		CEntityManager::AddComponentToEntity(pcEntity, eComponent::PATH_AGENT);
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::VIKING_CONTROLLER);

		pcEntity->SetActiveState(false);
	}
	m_pcEnemies.push_back(pcEntity);

	pcEntity = (CEnemyEntity*)CEntityManager::CreateEntity(eEntity::ENEMY);
	// Archer
	{
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::TRANSFORM);
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::RIGIDBODY);


		CCapsuleCollider* pcArcherCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::CAPSULE_COLLIDER);
		pcArcherCollider->SetCenter(CMath::TVECTOR3(0, 3.5f, 0));
		pcArcherCollider->SetHeight(7.0f);
		pcArcherCollider->SetRadius(1.0f);

		CMeshRenderer* pcArcherMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::MESH_RENDERER);
		pcArcherMesh->SetVertexBuffer(eVertexBuffer::ENEMY_ARCHER);
		pcArcherMesh->SetIndexBuffer(eIndexBuffer::ENEMY_ARCHER);
		pcArcherMesh->SetTextureCount(3);
		pcArcherMesh->SetTexture(eSRV::ENEMY_ARCHER_DIFFUSE, 0);
		pcArcherMesh->SetTexture(eSRV::NONE, 1);
		pcArcherMesh->SetTexture(eSRV::ENEMY_ARCHER_SPECULAR, 2);
		pcArcherMesh->SetInputLayout(eInputLayout::SKINNED);
		pcArcherMesh->SetVertexShader(eVertexShader::SKINNED);
		pcArcherMesh->SetPixelShader(ePixelShader::DEFAULT);
		pcArcherMesh->SetSampler(eSampler::CLAMP);
		pcArcherMesh->SetGlowColor({ 1,0,0,1 });

		CAnimatorComponent* pcArcherAnimator = (CAnimatorComponent*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::ANIMATOR);
		pcArcherAnimator->SetAnimator(eAnimator::ARCHER);
		pcArcherAnimator->SetAnimation(eEnemyAnimation::IDLE);
		pcArcherAnimator->SetAnimationTime(0.0);

		CStats*	pcArcherStats = (CStats*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::STATS);
		pcArcherStats->SetMaxHP(2.0f);
		pcArcherStats->SetHP(pcArcherStats->GetMaxHP());
		pcArcherStats->SetMaxMP(0.0f);
		pcArcherStats->SetMP(pcArcherStats->GetMaxMP());
		pcArcherStats->SetMovementSpeed(8.0f);
		pcArcherStats->SetBaseDamage(1.0f);
		pcArcherStats->SetAttackSpeed(1.25f);

		CEntityManager::AddComponentToEntity(pcEntity, eComponent::PATH_AGENT);
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::ARCHER_CONTROLLER);

		pcEntity->SetActiveState(false);
	}
	m_pcEnemies.push_back(pcEntity);

	pcEntity = (CEnemyEntity*)CEntityManager::CreateEntity(eEntity::ENEMY);
	// Knight
	{
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::TRANSFORM);
		pcEntity->GetComponent<CTransform>()->SetScale({ 3.25f, 3.25f, 3.25f });
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::RIGIDBODY);

		CParticleEmitter* pcEmitter = (CParticleEmitter*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::PARTICLE_EMITTER);

		pcEmitter->SetMaxParticles(50);
		pcEmitter->SetVertexBuffer(eVertexBuffer::QUAD);
		pcEmitter->SetIndexBuffer(eIndexBuffer::QUAD);
		pcEmitter->SetTexture(eSRV::PARTICLE_SPARK, 0);
		pcEmitter->SetInputLayout(eInputLayout::DEFAULT);
		pcEmitter->SetVertexShader(eVertexShader::DEFAULT);
		pcEmitter->SetPixelShader(ePixelShader::PASS_2D);
		pcEmitter->SetSampler(eSampler::WRAP);
		pcEmitter->SetEmitterShape(eEmitter::PLANE);
		pcEmitter->SetMaxSpeed(12.0f);
		pcEmitter->SetMinSpeed(6.0f);
		pcEmitter->SetParticleStartScale({ 1.0f, 1.0f, 0.1f });
		pcEmitter->SetParticleEndScale({ 1.0f, 1.0f, 0.1f });

		pcEmitter->SetMaxRotationSpeed(0.0f);
		pcEmitter->SetMinRotationSpeed(0.0f);

		pcEmitter->SetMaxExtent({3.0f, 0, 3.0f});
		pcEmitter->SetMinExtent({-3.0f, 0, -3.0f});


		pcEmitter->SetEmissionRate(.05f);
		pcEmitter->SetParticlesPerEmission(5);
		pcEmitter->SetPingPong(true);
		pcEmitter->SetLoop(true);
		pcEmitter->SetLife(.4f);
		pcEmitter->SetOffset({0, 0, 0});
		pcEmitter->SetParticleStartScale(CMath::TVECTOR3(1, 1, .1f));
		pcEmitter->SetParticleEndScale(CMath::TVECTOR3(1, 1, .1f));

		CCapsuleCollider* pcKnightCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::CAPSULE_COLLIDER);
		pcKnightCollider->SetCenter(CMath::TVECTOR3(0, 3.5f, 0));
		pcKnightCollider->SetRadius(0.5f);
		pcKnightCollider->SetHeight(2.0f);

		CMeshRenderer* pcKnightMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::MESH_RENDERER);
		pcKnightMesh->SetVertexBuffer(eVertexBuffer::ENEMY_KNIGHT);
		pcKnightMesh->SetIndexBuffer(eIndexBuffer::ENEMY_KNIGHT);
		pcKnightMesh->SetTextureCount(3);
		pcKnightMesh->SetTexture(eSRV::ENEMY_KNIGHT_DIFFUSE, 0);
		pcKnightMesh->SetTexture(eSRV::NONE, 1);
		pcKnightMesh->SetTexture(eSRV::NONE, 2);
		pcKnightMesh->SetInputLayout(eInputLayout::SKINNED);
		pcKnightMesh->SetVertexShader(eVertexShader::SKINNED);
		pcKnightMesh->SetPixelShader(ePixelShader::DEFAULT);
		pcKnightMesh->SetSampler(eSampler::CLAMP);
		pcKnightMesh->SetGlowColor({ 1,0,0,1 });

		CAnimatorComponent* pcKnightAnimator = (CAnimatorComponent*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::ANIMATOR);
		pcKnightAnimator->SetAnimator(eAnimator::KNIGHT);
		pcKnightAnimator->SetAnimation(eEnemyAnimation::IDLE);
		pcKnightAnimator->SetAnimationTime(0.0);

		CStats*	pcKnightStats = (CStats*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::STATS);
		pcKnightStats->SetMaxHP(5.0f);
		pcKnightStats->SetHP(pcKnightStats->GetMaxHP());
		pcKnightStats->SetMaxMP(0.0f);
		pcKnightStats->SetMP(pcKnightStats->GetMaxMP());
		pcKnightStats->SetMovementSpeed(5.0f);
		pcKnightStats->SetBaseDamage(1.0f);
		pcKnightStats->SetAttackSpeed(1.0f);

		CEntityManager::AddComponentToEntity(pcEntity, eComponent::PATH_AGENT);
		CEntityManager::AddComponentToEntity(pcEntity, eComponent::KNIGHT_CONTROLLER);

		pcEntity->SetActiveState(false);
	}
	m_pcEnemies.push_back(pcEntity);

#pragma endregion

#pragma region Room 1 

	CMath::TVECTOR4 Rotation90 = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90);
	CMath::TVECTOR4 Rotation90N = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, -90);
	CMath::TVECTOR4 Rotation0 = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0);
	CMath::TVECTOR4 Rotation180 = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 180);
	CMath::TVECTOR4 Rotation45 = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 45);
	CMath::TVECTOR4 Rotation45N = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, -45);
	CMath::TVECTOR4 Rotation140 = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 140);
	CMath::TVECTOR4 Rotation140N = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, -140);
	CMath::TVECTOR4 Rotation220 = CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 220);

	CRoomEntity* pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);
	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	CPathSurface* pcPathSurface = (CPathSurface*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::PATH_SURFACE);

	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM1);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM1);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::FLOOR_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::FLOOR_NORMAL, 1);
	pcMesh->SetTexture(eSRV::FLOOR_SPECULAR, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);

	//Spikes
	for (int i = 0; i < 3; i++)
	{
		pcRoom->m_pcTraps.push_back((CTrapEntity*)CEntityManager::CloneEntity(pcSpikes));
	}

	//Pillars
	{
		for (int i = 0; i < 4; i++)
		{
			pcRoom->m_pcPillars.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Pillar));
			pcRoom->m_pcPillars[i]->m_pcRoom = pcRoom;
			pcRoom->m_pcPillars[i]->GetComponent<CTransform>()->SetScale({ 1.0f, 1.1f, 1.0f });
		}

		pcRoom->m_pcPillars[0]->GetComponent<CTransform>()->SetPosition({ 45.0f, 0.0f, 45.0f });
		pcRoom->m_pcPillars[1]->GetComponent<CTransform>()->SetPosition({ 45.0f, 0.0f,-45.0f });
		pcRoom->m_pcPillars[2]->GetComponent<CTransform>()->SetPosition({ -45.0f, 0.0f, 45.0f });
		pcRoom->m_pcPillars[3]->GetComponent<CTransform>()->SetPosition({ -45.0f, 0.0f,-45.0f });
	}

	//Statues
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcStatue));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetPosition({ 35,0,35 });
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetPosition({ -35,0,35 });
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetPosition({ 35,-2,-37 });
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetPosition({ -35,0,-35 });
	
	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetRotation(Rotation140N);
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetRotation(Rotation140);
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetRotation(Rotation45);

	//Chairs
	for (int i = 0; i < 8; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcChair));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetPosition({ 42,0,30 });
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetPosition({ 30,0,42 });
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetPosition({ 42,1.5f,-30 });
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetPosition({ 30,0,-42 });
	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetPosition({ -42,0,-30 });
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetPosition({ -30,0,-42 });
	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetPosition({ -42,0,30 });
	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetPosition({ -30,0,42 });

	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 1, 0 }, -90));
	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetRotation(Rotation90N);

	//Flags
	for (int i = 0; i < 8; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcFlag));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetPosition({ 27,0,47 });
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetPosition({ -27,0,47 });
	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetPosition({ 27,0,-47 });
	pcRoom->m_pcClutter[15]->GetComponent<CTransform>()->SetPosition({ -27,0,-47 });
	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetPosition({ -47,0,27 });
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetPosition({ 47,0,-27 });
	pcRoom->m_pcClutter[18]->GetComponent<CTransform>()->SetPosition({ 47,0,27 });
	pcRoom->m_pcClutter[19]->GetComponent<CTransform>()->SetPosition({ -47,0,-27 });

	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[18]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[19]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Rock01
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcRock1));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.1f,0.1f,0.1f });
	}

	pcRoom->m_pcClutter[20]->GetComponent<CTransform>()->SetPosition({ 34,1,-45 });
	pcRoom->m_pcClutter[21]->GetComponent<CTransform>()->SetPosition({ 34,1,-37 });
	
	pcRoom->m_pcClutter[20]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[21]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 1, 0 }, -90));

	//Rock 02
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcRock2));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[22]->GetComponent<CTransform>()->SetPosition({ 43,0.5f,-38 });
	pcRoom->m_pcClutter[23]->GetComponent<CTransform>()->SetPosition({ 35.5f,0.5f,-37 });

	pcRoom->m_pcClutter[22]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[23]->GetComponent<CTransform>()->SetRotation(Rotation45N);

	pcRoom->m_pcClutter[22]->GetComponent<CTransform>()->SetScale({2, 2, 2});
	pcRoom->m_pcClutter[23]->GetComponent<CTransform>()->SetScale({ 1.5f, 1.5f, 1.5f });

	//Rock 03
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcRock3));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[24]->GetComponent<CTransform>()->SetPosition({ 45,0.5f,-32});
	pcRoom->m_pcClutter[25]->GetComponent<CTransform>()->SetPosition({ 25,0.5f,-45});

	pcRoom->m_pcClutter[24]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[25]->GetComponent<CTransform>()->SetRotation(Rotation45N);

	//Barrel01
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcBarrel01));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[26]->GetComponent<CTransform>()->SetPosition({ -35.5f,0,45 });
	pcRoom->m_pcClutter[27]->GetComponent<CTransform>()->SetPosition({ -45,0,35.5f });

	pcRoom->m_pcClutter[27]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Sack
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Sack));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[28]->GetComponent<CTransform>()->SetPosition({ -35.5,0,40});
	pcRoom->m_pcClutter[29]->GetComponent<CTransform>()->SetPosition({ -41,0,35.5 });

	pcRoom->m_pcClutter[29]->GetComponent<CTransform>()->SetRotation(Rotation90);
	
	//Logs
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Logs));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.8f,0.8f,0.8f });
	}

	pcRoom->m_pcClutter[30]->GetComponent<CTransform>()->SetPosition({ 35.5f,0,45 });
	pcRoom->m_pcClutter[31]->GetComponent<CTransform>()->SetPosition({ 45,0,35.5f });

	pcRoom->m_pcClutter[31]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Cart
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Cart));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.5f,0.5f,0.5f });
	}

	pcRoom->m_pcClutter[32]->GetComponent<CTransform>()->SetPosition({ 36,0,40 });
	pcRoom->m_pcClutter[33]->GetComponent<CTransform>()->SetPosition({ 40,0,36 });

	pcRoom->m_pcClutter[33]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Circel Platform
	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Platform_Circle));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
	}

	pcRoom->m_pcClutter[34]->GetComponent<CTransform>()->SetPosition({ 0,0,0 });

	pcRoom->m_pcClutter[34]->GetComponent<CTransform>()->SetRotation(Rotation180);

	//Platform
	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Platform));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
	}

	pcRoom->m_pcClutter[35]->GetComponent<CTransform>()->SetPosition({ 0,0,0 });
	pcRoom->m_pcClutter[35]->GetComponent<CTransform>()->SetRotation(Rotation180);

	//Altar
	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Altar));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
	}

	pcRoom->m_pcClutter[36]->GetComponent<CTransform>()->SetPosition({ 0,0,0 });
	pcRoom->m_pcClutter[36]->GetComponent<CTransform>()->SetRotation(Rotation180);

	//Arches
	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Arches));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
	}

	pcRoom->m_pcClutter[37]->GetComponent<CTransform>()->SetPosition({ 0,0,0 });
	pcRoom->m_pcClutter[37]->GetComponent<CTransform>()->SetRotation(Rotation180);

	//Walls, SetExtent, Rotate Walls
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));
			pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;

			pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetScale({ 1.43f, 1, 1.7f });

			if (i < 4)
				pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(Rotation90);
			else
				pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(Rotation0);
		}

		pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition({ 26.0f, 0.0f, 48.0f });
		pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition({-26.0f, 0.0f, 48.0f });
		pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition({ 26.0f, 0.0f,-48.0f });
		pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition({-26.0f, 0.0f,-48.0f });
		pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetPosition({-48.0f, 0.0f, 26.0f });
		pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetPosition({ 48.0f, 0.0f,-26.0f });
		pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetPosition({ 48.0f, 0.0f, 26.0f });
		pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetPosition({-48.0f, 0.0f,-26.0f });
	}

	//Doors
	{
		for (int i = 0; i < 4; i++)
		{
			pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));
			pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
		}
		float fDoorOffset = 49.8f;
		pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3( fDoorOffset, 0, 0));
		pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-fDoorOffset, 0, 0));
		pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, fDoorOffset));
		pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0,-fDoorOffset));

		pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(Rotation90N);
		pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(Rotation180);
	}

	//SpawnPoints
	{
		for (int i = 0; i < 104; i++)
		{
			pcRoom->m_pcSpawns.push_back((CSpawnEntity*)CEntityManager::CloneEntity(Spawner));
		}

		pcRoom->m_pcSpawns[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 20));
		pcRoom->m_pcSpawns[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 25));
		pcRoom->m_pcSpawns[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 30));
		pcRoom->m_pcSpawns[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 35));
		pcRoom->m_pcSpawns[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 40));
		pcRoom->m_pcSpawns[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 20));
		pcRoom->m_pcSpawns[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 25));
		pcRoom->m_pcSpawns[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 30));
		pcRoom->m_pcSpawns[8]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 35));
		pcRoom->m_pcSpawns[9]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 40));
		pcRoom->m_pcSpawns[10]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 20));
		pcRoom->m_pcSpawns[11]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 25));
		pcRoom->m_pcSpawns[12]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 30));
		pcRoom->m_pcSpawns[13]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 35));
		pcRoom->m_pcSpawns[14]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 40));
		pcRoom->m_pcSpawns[15]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 25));
		pcRoom->m_pcSpawns[16]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 25));
		pcRoom->m_pcSpawns[17]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 25));
		pcRoom->m_pcSpawns[18]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 20));
		pcRoom->m_pcSpawns[19]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 20));
		pcRoom->m_pcSpawns[20]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 20));
		pcRoom->m_pcSpawns[21]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 15));
		pcRoom->m_pcSpawns[22]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 15));
		pcRoom->m_pcSpawns[23]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 15));
		pcRoom->m_pcSpawns[24]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 15));
		pcRoom->m_pcSpawns[25]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 15));
		pcRoom->m_pcSpawns[26]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -20));
		pcRoom->m_pcSpawns[27]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -25));
		pcRoom->m_pcSpawns[28]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -30));
		pcRoom->m_pcSpawns[29]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -35));
		pcRoom->m_pcSpawns[30]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -40));
		pcRoom->m_pcSpawns[31]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -20));
		pcRoom->m_pcSpawns[32]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -25));
		pcRoom->m_pcSpawns[33]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -30));
		pcRoom->m_pcSpawns[34]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -35));
		pcRoom->m_pcSpawns[35]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -40));
		pcRoom->m_pcSpawns[36]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -20));
		pcRoom->m_pcSpawns[37]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -25));
		pcRoom->m_pcSpawns[38]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -30));
		pcRoom->m_pcSpawns[39]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -35));
		pcRoom->m_pcSpawns[40]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -40));
		pcRoom->m_pcSpawns[41]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -25));
		pcRoom->m_pcSpawns[42]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -25));
		pcRoom->m_pcSpawns[43]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -25));
		pcRoom->m_pcSpawns[44]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -20));
		pcRoom->m_pcSpawns[45]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -20));
		pcRoom->m_pcSpawns[46]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -20));
		pcRoom->m_pcSpawns[47]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -15));
		pcRoom->m_pcSpawns[48]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -15));
		pcRoom->m_pcSpawns[49]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -15));
		pcRoom->m_pcSpawns[50]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -15));
		pcRoom->m_pcSpawns[51]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -15));
		pcRoom->m_pcSpawns[52]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 20));
		pcRoom->m_pcSpawns[53]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 25));
		pcRoom->m_pcSpawns[54]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 30));
		pcRoom->m_pcSpawns[55]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 35));
		pcRoom->m_pcSpawns[56]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 40));
		pcRoom->m_pcSpawns[57]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 20));
		pcRoom->m_pcSpawns[58]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 25));
		pcRoom->m_pcSpawns[59]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 30));
		pcRoom->m_pcSpawns[60]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 35));
		pcRoom->m_pcSpawns[61]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 40));
		pcRoom->m_pcSpawns[62]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 20));
		pcRoom->m_pcSpawns[63]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 25));
		pcRoom->m_pcSpawns[64]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 30));
		pcRoom->m_pcSpawns[65]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 35));
		pcRoom->m_pcSpawns[66]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 40));
		pcRoom->m_pcSpawns[67]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 25));
		pcRoom->m_pcSpawns[68]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 25));
		pcRoom->m_pcSpawns[69]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 25));
		pcRoom->m_pcSpawns[70]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 20));
		pcRoom->m_pcSpawns[71]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 20));
		pcRoom->m_pcSpawns[72]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 20));
		pcRoom->m_pcSpawns[73]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 15));
		pcRoom->m_pcSpawns[74]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 15));
		pcRoom->m_pcSpawns[75]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 15));
		pcRoom->m_pcSpawns[76]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 15));
		pcRoom->m_pcSpawns[77]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 15));
		pcRoom->m_pcSpawns[78]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -20));
		pcRoom->m_pcSpawns[79]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -25));
		pcRoom->m_pcSpawns[80]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -30));
		pcRoom->m_pcSpawns[81]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -35));
		pcRoom->m_pcSpawns[82]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -40));
		pcRoom->m_pcSpawns[83]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -20));
		pcRoom->m_pcSpawns[84]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -25));
		pcRoom->m_pcSpawns[85]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -30));
		pcRoom->m_pcSpawns[86]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -35));
		pcRoom->m_pcSpawns[87]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -40));
		pcRoom->m_pcSpawns[88]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -20));
		pcRoom->m_pcSpawns[89]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -25));
		pcRoom->m_pcSpawns[90]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -30));
		pcRoom->m_pcSpawns[91]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -35));
		pcRoom->m_pcSpawns[92]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -40));
		pcRoom->m_pcSpawns[93]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -25));
		pcRoom->m_pcSpawns[94]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -25));
		pcRoom->m_pcSpawns[95]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -25));
		pcRoom->m_pcSpawns[96]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -20));
		pcRoom->m_pcSpawns[97]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -20));
		pcRoom->m_pcSpawns[98]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -20));
		pcRoom->m_pcSpawns[99]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -15));
		pcRoom->m_pcSpawns[100]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -15));
		pcRoom->m_pcSpawns[101]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -15));
		pcRoom->m_pcSpawns[102]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -15));
		pcRoom->m_pcSpawns[103]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -15));
		
	}

	// Lights
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcLights.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Light));
			pcRoom->m_pcLights[i]->m_pcRoom = pcRoom;
		}

		//	Top
		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetPosition({ 15.0f, 4.0f, 47.5f });
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetPosition({-15.0f, 4.0f, 47.5f });

		//pcRoom->m_pcLights[0]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[1]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));

		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetRotation(Rotation90);

		pcRoom->m_pcLights[0]->top = true;
		pcRoom->m_pcLights[1]->top = true;

		//	Right
		pcRoom->m_pcLights[2]->GetComponent<CTransform>()->SetPosition({-47.5f, 4.0f,-15.0f });
		pcRoom->m_pcLights[3]->GetComponent<CTransform>()->SetPosition({-47.5f, 4.0f, 15.0f });

		//pcRoom->m_pcLights[2]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[3]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));

		pcRoom->m_pcLights[2]->right = true;
		pcRoom->m_pcLights[3]->right = true;

		//	Bottom
		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetPosition({-15.0f, 4.0f,-47.5f });
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetPosition({ 15.0f, 4.0f,-47.5f });

		//pcRoom->m_pcLights[4]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[5]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));

		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetRotation(Rotation90N);
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetRotation(Rotation90N);

		pcRoom->m_pcLights[4]->bottom = true;
		pcRoom->m_pcLights[5]->bottom = true;

		//	Left
		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetPosition({ 47.5f, 4.0f, 15.0f });
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetPosition({ 47.5f, 4.0f,-15.0f });

		//pcRoom->m_pcLights[6]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
		//pcRoom->m_pcLights[7]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));

		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetRotation(Rotation180);
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetRotation(Rotation180);

		pcRoom->m_pcLights[6]->left = true;
		pcRoom->m_pcLights[7]->left = true;
	}

	pcRoom->SetActiveState(false);
	m_pRoomsType.push_back(pcRoom);
#pragma endregion

#pragma region Room 2

	pcRoom = nullptr;
	pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::PATH_SURFACE);
	pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM2);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM2);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::FLOOR_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::FLOOR_NORMAL, 1);
	pcMesh->SetTexture(eSRV::FLOOR_SPECULAR, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);

	//Spikes
	for (int i = 0; i < 3; i++)
	{
		pcRoom->m_pcTraps.push_back((CTrapEntity*)CEntityManager::CloneEntity(pcSpikes));
	}

	//Pillars
	{
		for (int i = 0; i < 12; i++)
		{
			pcRoom->m_pcPillars.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Pillar));
			pcRoom->m_pcPillars[i]->GetComponent<CTransform>()->SetScale({ 0.5f, 1.1f, 0.5f });

			pcRoom->m_pcPillars[i]->m_pcRoom = pcRoom;
		}

		pcRoom->m_pcPillars[0]->GetComponent<CTransform>()->SetPosition({ 30.0f, 0, 30.0f });
		pcRoom->m_pcPillars[1]->GetComponent<CTransform>()->SetPosition({ 30.0f, 0,-30.0f });
		pcRoom->m_pcPillars[2]->GetComponent<CTransform>()->SetPosition({ -30.0f, 0, 30.0f });
		pcRoom->m_pcPillars[3]->GetComponent<CTransform>()->SetPosition({ -30.0f, 0,-30.0f });
		pcRoom->m_pcPillars[4]->GetComponent<CTransform>()->SetPosition({ 30.0f, 0, 9.0f });
		pcRoom->m_pcPillars[5]->GetComponent<CTransform>()->SetPosition({ 30.0f, 0,-9.0f });
		pcRoom->m_pcPillars[6]->GetComponent<CTransform>()->SetPosition({ -30.0f, 0, 9.0f });
		pcRoom->m_pcPillars[7]->GetComponent<CTransform>()->SetPosition({ -30.0f, 0,-9.0f });
		pcRoom->m_pcPillars[8]->GetComponent<CTransform>()->SetPosition({ 9.0f, 0, 30.0f });
		pcRoom->m_pcPillars[9]->GetComponent<CTransform>()->SetPosition({ 9.0f, 0,-30.0f });
		pcRoom->m_pcPillars[10]->GetComponent<CTransform>()->SetPosition({ -9.0f, 0, 30.0f });
		pcRoom->m_pcPillars[11]->GetComponent<CTransform>()->SetPosition({ -9.0f, 0,-30.0f });
	}

	//Walls
	{
		for (int i = 0; i < 16; i++)
		{
			pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));

			if (i < 4 || i > 11)
			{
				pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(Rotation90);
			}
				
			pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;
		}

		pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition({ 19.5f, 0, 30.0f });
		pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition({ -19.5f, 0, 30.0f });
		pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition({ 19.5f, 0,-30.0f });
		pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition({ -19.5f, 0,-30.0f });
		pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetPosition({ 30.0f, 0, 19.5f });
		pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetPosition({ -30.0f, 0, 19.5f });
		pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetPosition({ 30.0f, 0,-19.5f });
		pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetPosition({ -30.0f, 0,-19.5f });
		pcRoom->m_pcWalls[8]->GetComponent<CTransform>()->SetPosition({ 9.0f, 0, 40.0f });
		pcRoom->m_pcWalls[9]->GetComponent<CTransform>()->SetPosition({ -9.0f, 0, 40.0f });
		pcRoom->m_pcWalls[10]->GetComponent<CTransform>()->SetPosition({ 9.0f, 0,-40.0f });
		pcRoom->m_pcWalls[11]->GetComponent<CTransform>()->SetPosition({ -9.0f, 0,-40.0f });
		pcRoom->m_pcWalls[12]->GetComponent<CTransform>()->SetPosition({ 40.0f, 0, 9.0f });
		pcRoom->m_pcWalls[13]->GetComponent<CTransform>()->SetPosition({ -40.0f, 0, 9.0f });
		pcRoom->m_pcWalls[14]->GetComponent<CTransform>()->SetPosition({ 40.0f, 0,-9.0f });
		pcRoom->m_pcWalls[15]->GetComponent<CTransform>()->SetPosition({ -40.0f, 0,-9.0f });
	}

	//Flags
	for (int i = 0; i < 8; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcFlag));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetPosition({ 20,0,29 });
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetPosition({ 29,0,20 });
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetPosition({ 20,0,-29 });
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetPosition({ 29,0,-20 });
	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetPosition({ -20,0,29 });
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetPosition({ -29,0,20 });
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetPosition({ -20,0,-29 });
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetPosition({ -29,0,-20 });
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Barrels
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcBarrel01));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetPosition({ 20,0,27.5f });
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetPosition({ 27.5f,0,20 });

	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Sack
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Sack));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetPosition({ -20,0,27.5f });
	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetPosition({ -27.5f,0,20 });

	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Logs
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Logs));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.8f,0.8f,0.8f });
	}

	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetPosition({ 20,0,-27.5f });
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetPosition({ 27.5f,0,-20 });

	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//chests
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Chest));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1.2f,1.2f,1.2f });
	}

	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetPosition({ 20,0,20});
	pcRoom->m_pcClutter[15]->GetComponent<CTransform>()->SetPosition({ -20,0,20 });
	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetPosition({ 20,0,-20 });
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetPosition({ -20,0,-20 });

	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[15]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetRotation(Rotation45);

	//Doors
	{
		for (int i = 0; i < 4; i++)
		{
			pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));
			pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
		}

		pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(49, 0, 0));
		pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-49, 0, 0));
		pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 49));
		pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -49));

		//Rotate
		pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(Rotation180);
		pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetRotation(Rotation0);
		pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	}

	//SpawnPoints
	{
		for (int i = 0; i < 41; i++)
		{
			pcRoom->m_pcSpawns.push_back((CSpawnEntity*)CEntityManager::CloneEntity(Spawner));
		}

		pcRoom->m_pcSpawns[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 0));
		pcRoom->m_pcSpawns[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 0));
		pcRoom->m_pcSpawns[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, 0));
		pcRoom->m_pcSpawns[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 5));
		pcRoom->m_pcSpawns[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 10));
		pcRoom->m_pcSpawns[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 5));
		pcRoom->m_pcSpawns[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, 5));
		pcRoom->m_pcSpawns[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 10));
		pcRoom->m_pcSpawns[8]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, 10));
		pcRoom->m_pcSpawns[9]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 0));
		pcRoom->m_pcSpawns[10]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, 0));
		pcRoom->m_pcSpawns[11]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 5));
		pcRoom->m_pcSpawns[12]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, 5));
		pcRoom->m_pcSpawns[13]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 10));
		pcRoom->m_pcSpawns[14]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, 10));
		pcRoom->m_pcSpawns[15]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -5));
		pcRoom->m_pcSpawns[16]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -10));
		pcRoom->m_pcSpawns[17]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, -5));
		pcRoom->m_pcSpawns[18]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, -10));
		pcRoom->m_pcSpawns[19]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, -5));
		pcRoom->m_pcSpawns[20]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, -10));
		pcRoom->m_pcSpawns[21]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, -5));
		pcRoom->m_pcSpawns[22]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, -10));
		pcRoom->m_pcSpawns[23]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, -5));
		pcRoom->m_pcSpawns[24]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, -10));
		pcRoom->m_pcSpawns[25]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 0));
		pcRoom->m_pcSpawns[26]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 15));
		pcRoom->m_pcSpawns[27]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 5));
		pcRoom->m_pcSpawns[28]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 15));
		pcRoom->m_pcSpawns[29]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 15));
		pcRoom->m_pcSpawns[30]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 0));
		pcRoom->m_pcSpawns[31]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 5));
		pcRoom->m_pcSpawns[32]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 15));
		pcRoom->m_pcSpawns[33]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 15));
		pcRoom->m_pcSpawns[34]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -15));
		pcRoom->m_pcSpawns[35]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, -15));
		pcRoom->m_pcSpawns[36]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -5));
		pcRoom->m_pcSpawns[37]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -15));
		pcRoom->m_pcSpawns[38]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, -15));
		pcRoom->m_pcSpawns[39]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -5));
		pcRoom->m_pcSpawns[40]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -15));
	}

	// Lights
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcLights.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Light));
			pcRoom->m_pcLights[i]->m_pcRoom = pcRoom;
			pcRoom->m_pcLights[i]->GetComponent<CLightComponent>()->SetRadius(50.0f);
		}

		//	Top
		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetPosition({ 9.0f, 5.0f, 28.5f });
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetPosition({ -9.0f, 5.0f, 28.5f });

		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetRotation(Rotation90);

		pcRoom->m_pcLights[0]->top = true;
		pcRoom->m_pcLights[1]->top = true;

		//	Right
		pcRoom->m_pcLights[2]->GetComponent<CTransform>()->SetPosition({ -28.5f, 5.0f,-9.0f });
		pcRoom->m_pcLights[3]->GetComponent<CTransform>()->SetPosition({ -28.5f, 5.0f, 9.0f });

		pcRoom->m_pcLights[2]->right = true;
		pcRoom->m_pcLights[3]->right = true;

		//	Bottom
		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetPosition({ -9.0f, 5.0f, -28.5f });
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetPosition({ 9.0f, 5.0f,-28.5f });

		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetRotation(Rotation90N);
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetRotation(Rotation90N);

		pcRoom->m_pcLights[4]->bottom = true;
		pcRoom->m_pcLights[5]->bottom = true;

		//	Left
		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetPosition({ 28.5f, 5.0f, 9.0f });
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetPosition({ 28.5f, 5.0f,-9.0f });

		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetRotation(Rotation180);
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetRotation(Rotation180);

		pcRoom->m_pcLights[6]->left = true;
		pcRoom->m_pcLights[7]->left = true;

		//pcRoom->m_pcLights[0]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[1]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[2]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[3]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[4]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[5]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[6]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
		//pcRoom->m_pcLights[7]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
	}

	m_pRoomsType.push_back(pcRoom);

	pcRoom->SetActiveState(false);

#pragma endregion

#pragma region Room 3

	pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::PATH_SURFACE);

	pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM1);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM1);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::FLOOR_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::FLOOR_NORMAL, 1);
	pcMesh->SetTexture(eSRV::FLOOR_SPECULAR, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);
	
	//Spikes
	for (int i = 0; i < 3; i++)
	{
		pcRoom->m_pcTraps.push_back((CTrapEntity*)CEntityManager::CloneEntity(pcSpikes));
	}

	//Pillars
	{
		for (int i = 0; i < 5; i++)
		{
			pcRoom->m_pcPillars.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Pillar));
			pcRoom->m_pcPillars[i]->m_pcRoom = pcRoom;
			pcRoom->m_pcPillars[i]->GetComponent<CTransform>()->SetScale({ 1.0f, 1.1f, 1.0f });
		}

		pcRoom->m_pcPillars[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(45, 0, 45));
		pcRoom->m_pcPillars[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(45, 0, -45));
		pcRoom->m_pcPillars[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-45, 0, 45));
		pcRoom->m_pcPillars[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-45, 0, -45));
		pcRoom->m_pcPillars[4]->GetComponent<CTransform>()->SetPosition({ 0.0f, 0.0f, 0.0f });
		pcRoom->m_pcPillars[4]->GetComponent<CTransform>()->SetScale(CMath::TVECTOR3(3.25f, 3.25f, 3.25f));
		pcRoom->m_pcPillars[4]->GetComponent<CBoxCollider>()->SetExtent({ 5.0f, 10, 5.0f });
		pcRoom->m_pcPillars[4]->GetComponent<CBoxCollider>()->SetCenter({ 0.0f, 3.0f, 0.0f });
	}

	//Bookcase
	for (int i = 0; i < 16; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(BookCase));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetPosition({ 45,0,35 });
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetPosition({ 45,0,23 });
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetPosition({ 35,0,45 });
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetPosition({ 23,0,45 });
	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetPosition({ -45,0,35 });
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetPosition({ -45,0,23 });
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetPosition({ -35,0,45 });
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetPosition({ -23,0,45 });
	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetPosition({ 45,0,-35 });
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetPosition({ 45,0,-23 });
	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetPosition({ 35,0,-45 });
	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetPosition({ 23,0,-45 });
	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetPosition({ -45,0,-35 });
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetPosition({ -45,0,-23 });
	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetPosition({ -35,0,-45 });
	pcRoom->m_pcClutter[15]->GetComponent<CTransform>()->SetPosition({ -23,0,-45 });

	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Books
	for (int i = 0; i < 16; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Book));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetPosition({ 44.5f,0,35 });
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetPosition({ 44.5f,0,23 });
	pcRoom->m_pcClutter[18]->GetComponent<CTransform>()->SetPosition({ 35,0,44.5f });
	pcRoom->m_pcClutter[19]->GetComponent<CTransform>()->SetPosition({ 23,0,44.5f });
	pcRoom->m_pcClutter[20]->GetComponent<CTransform>()->SetPosition({ -44.5f,0,35 });
	pcRoom->m_pcClutter[21]->GetComponent<CTransform>()->SetPosition({ -44.5f,0,23 });
	pcRoom->m_pcClutter[22]->GetComponent<CTransform>()->SetPosition({ -35,0,44.5f });
	pcRoom->m_pcClutter[23]->GetComponent<CTransform>()->SetPosition({ -23,0,44.5f });
	pcRoom->m_pcClutter[24]->GetComponent<CTransform>()->SetPosition({ 44.5f,0,-35 });
	pcRoom->m_pcClutter[25]->GetComponent<CTransform>()->SetPosition({ 44.5f,0,-23 });
	pcRoom->m_pcClutter[26]->GetComponent<CTransform>()->SetPosition({ 35,0,-44.5f });
	pcRoom->m_pcClutter[27]->GetComponent<CTransform>()->SetPosition({ 23,0,-44.5f });
	pcRoom->m_pcClutter[28]->GetComponent<CTransform>()->SetPosition({ -44.5f,0,-35 });
	pcRoom->m_pcClutter[29]->GetComponent<CTransform>()->SetPosition({ -44.5f,0,-23 });
	pcRoom->m_pcClutter[30]->GetComponent<CTransform>()->SetPosition({ -35,0,-44.5f });
	pcRoom->m_pcClutter[31]->GetComponent<CTransform>()->SetPosition({ -23,0,-44.5f });

	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[18]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[19]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[20]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[21]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[22]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[23]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[24]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[25]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[28]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[29]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//MiddleBooks
	for (int i = 0; i < 13; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(BookCase));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[32]->GetComponent<CTransform>()->SetPosition({ 17,0,0});
	pcRoom->m_pcClutter[33]->GetComponent<CTransform>()->SetPosition({ 17,0,10});
	pcRoom->m_pcClutter[34]->GetComponent<CTransform>()->SetPosition({ 17,0,-10});															
	pcRoom->m_pcClutter[35]->GetComponent<CTransform>()->SetPosition({ -17,0,0});
	pcRoom->m_pcClutter[36]->GetComponent<CTransform>()->SetPosition({ -17,0,10});
	pcRoom->m_pcClutter[37]->GetComponent<CTransform>()->SetPosition({ -17,0,-10});																	
	pcRoom->m_pcClutter[38]->GetComponent<CTransform>()->SetPosition({ 0,0,17});
	pcRoom->m_pcClutter[39]->GetComponent<CTransform>()->SetPosition({ 10,0,17});
	pcRoom->m_pcClutter[40]->GetComponent<CTransform>()->SetPosition({ -10,0,17});																
	pcRoom->m_pcClutter[41]->GetComponent<CTransform>()->SetPosition({ 0,0,-17});
	pcRoom->m_pcClutter[42]->GetComponent<CTransform>()->SetPosition({ 10,0,-17});
	pcRoom->m_pcClutter[43]->GetComponent<CTransform>()->SetPosition({ -10,0,-17});

	pcRoom->m_pcClutter[32]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[33]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[34]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[35]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[36]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[37]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[41]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[42]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[43]->GetComponent<CTransform>()->SetRotation(Rotation180);

	//Books
	for (int i = 0; i < 13; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Book));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[44]->GetComponent<CTransform>()->SetPosition({ 17.5,0,0 });
	pcRoom->m_pcClutter[45]->GetComponent<CTransform>()->SetPosition({ 17.5,0,10 });
	pcRoom->m_pcClutter[46]->GetComponent<CTransform>()->SetPosition({ 17.5,0,-10 });
	pcRoom->m_pcClutter[47]->GetComponent<CTransform>()->SetPosition({ -17.5,0,0 });
	pcRoom->m_pcClutter[48]->GetComponent<CTransform>()->SetPosition({ -17.5,0,10 });
	pcRoom->m_pcClutter[49]->GetComponent<CTransform>()->SetPosition({ -17.5,0,-10 });
	pcRoom->m_pcClutter[50]->GetComponent<CTransform>()->SetPosition({ 0,0,17.5 });
	pcRoom->m_pcClutter[51]->GetComponent<CTransform>()->SetPosition({ 10,0,17.5 });
	pcRoom->m_pcClutter[52]->GetComponent<CTransform>()->SetPosition({ -10,0,17.5 });
	pcRoom->m_pcClutter[53]->GetComponent<CTransform>()->SetPosition({ 0,0,-17.5 });
	pcRoom->m_pcClutter[54]->GetComponent<CTransform>()->SetPosition({ 10,0,-17.5 });
	pcRoom->m_pcClutter[55]->GetComponent<CTransform>()->SetPosition({ -10,0,-17.5 });

	pcRoom->m_pcClutter[44]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[45]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[46]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[47]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[48]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[49]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[53]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[54]->GetComponent<CTransform>()->SetRotation(Rotation180);
	pcRoom->m_pcClutter[55]->GetComponent<CTransform>()->SetRotation(Rotation180);
	
	//Walls, Rotate Walls, SetExtent
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));
			pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;

			pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetScale({ 1.43f, 1, 1.7f });

			if (i < 4)
				pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(Rotation90);
			else
				pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(Rotation0);
		}

		pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition({ 26.0f, 0.0f, 48.0f });
		pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition({ -26.0f, 0.0f, 48.0f });
		pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition({ 26.0f, 0.0f,-48.0f });
		pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition({ -26.0f, 0.0f,-48.0f });
		pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetPosition({ -48.0f, 0.0f, 26.0f });
		pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetPosition({ 48.0f, 0.0f,-26.0f });
		pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetPosition({ 48.0f, 0.0f, 26.0f });
		pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetPosition({ -48.0f, 0.0f,-26.0f });
	}

	//Doors
	{
		for (int i = 0; i < 4; i++)
		{
			pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));
			pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
		}
		pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(48.5f, 0, 0));
		pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-48.5f, 0, 0));
		pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 48.5f));
		pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -48.5f));

		//Rotate
		pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetRotation(Rotation0);
		pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(Rotation90N);
		pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(Rotation180);
	}

	//SpawnPoints
	{
		for (int i = 0; i < 104; i++)
		{
			pcRoom->m_pcSpawns.push_back((CSpawnEntity*)CEntityManager::CloneEntity(Spawner));
		}

		pcRoom->m_pcSpawns[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 20));
		pcRoom->m_pcSpawns[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 25));
		pcRoom->m_pcSpawns[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 30));
		pcRoom->m_pcSpawns[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 35));
		pcRoom->m_pcSpawns[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 40));
		pcRoom->m_pcSpawns[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 20));
		pcRoom->m_pcSpawns[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 25));
		pcRoom->m_pcSpawns[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 30));
		pcRoom->m_pcSpawns[8]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 35));
		pcRoom->m_pcSpawns[9]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 40));
		pcRoom->m_pcSpawns[10]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 20));
		pcRoom->m_pcSpawns[11]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 25));
		pcRoom->m_pcSpawns[12]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 30));
		pcRoom->m_pcSpawns[13]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 35));
		pcRoom->m_pcSpawns[14]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 40));
		pcRoom->m_pcSpawns[15]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 25));
		pcRoom->m_pcSpawns[16]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 25));
		pcRoom->m_pcSpawns[17]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 25));
		pcRoom->m_pcSpawns[18]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 20));
		pcRoom->m_pcSpawns[19]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 20));
		pcRoom->m_pcSpawns[20]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 20));
		pcRoom->m_pcSpawns[21]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 15));
		pcRoom->m_pcSpawns[22]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 15));
		pcRoom->m_pcSpawns[23]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 15));
		pcRoom->m_pcSpawns[24]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 15));
		pcRoom->m_pcSpawns[25]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 15));
		pcRoom->m_pcSpawns[26]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -20));
		pcRoom->m_pcSpawns[27]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -25));
		pcRoom->m_pcSpawns[28]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -30));
		pcRoom->m_pcSpawns[29]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -35));
		pcRoom->m_pcSpawns[30]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -40));
		pcRoom->m_pcSpawns[31]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -20));
		pcRoom->m_pcSpawns[32]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -25));
		pcRoom->m_pcSpawns[33]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -30));
		pcRoom->m_pcSpawns[34]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -35));
		pcRoom->m_pcSpawns[35]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -40));
		pcRoom->m_pcSpawns[36]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -20));
		pcRoom->m_pcSpawns[37]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -25));
		pcRoom->m_pcSpawns[38]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -30));
		pcRoom->m_pcSpawns[39]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -35));
		pcRoom->m_pcSpawns[40]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -40));
		pcRoom->m_pcSpawns[41]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -25));
		pcRoom->m_pcSpawns[42]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -25));
		pcRoom->m_pcSpawns[43]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -25));
		pcRoom->m_pcSpawns[44]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -20));
		pcRoom->m_pcSpawns[45]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -20));
		pcRoom->m_pcSpawns[46]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -20));
		pcRoom->m_pcSpawns[47]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -15));
		pcRoom->m_pcSpawns[48]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -15));
		pcRoom->m_pcSpawns[49]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -15));
		pcRoom->m_pcSpawns[50]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -15));
		pcRoom->m_pcSpawns[51]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -15));
		pcRoom->m_pcSpawns[52]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 20));
		pcRoom->m_pcSpawns[53]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 25));
		pcRoom->m_pcSpawns[54]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 30));
		pcRoom->m_pcSpawns[55]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 35));
		pcRoom->m_pcSpawns[56]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 40));
		pcRoom->m_pcSpawns[57]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 20));
		pcRoom->m_pcSpawns[58]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 25));
		pcRoom->m_pcSpawns[59]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 30));
		pcRoom->m_pcSpawns[60]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 35));
		pcRoom->m_pcSpawns[61]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 40));
		pcRoom->m_pcSpawns[62]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 20));
		pcRoom->m_pcSpawns[63]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 25));
		pcRoom->m_pcSpawns[64]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 30));
		pcRoom->m_pcSpawns[65]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 35));
		pcRoom->m_pcSpawns[66]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 40));
		pcRoom->m_pcSpawns[67]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 25));
		pcRoom->m_pcSpawns[68]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 25));
		pcRoom->m_pcSpawns[69]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 25));
		pcRoom->m_pcSpawns[70]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 20));
		pcRoom->m_pcSpawns[71]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 20));
		pcRoom->m_pcSpawns[72]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 20));
		pcRoom->m_pcSpawns[73]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 15));
		pcRoom->m_pcSpawns[74]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 15));
		pcRoom->m_pcSpawns[75]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 15));
		pcRoom->m_pcSpawns[76]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 15));
		pcRoom->m_pcSpawns[77]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 15));
		pcRoom->m_pcSpawns[78]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -20));
		pcRoom->m_pcSpawns[79]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -25));
		pcRoom->m_pcSpawns[80]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -30));
		pcRoom->m_pcSpawns[81]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -35));
		pcRoom->m_pcSpawns[82]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -40));
		pcRoom->m_pcSpawns[83]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -20));
		pcRoom->m_pcSpawns[84]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -25));
		pcRoom->m_pcSpawns[85]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -30));
		pcRoom->m_pcSpawns[86]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -35));
		pcRoom->m_pcSpawns[87]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -40));
		pcRoom->m_pcSpawns[88]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -20));
		pcRoom->m_pcSpawns[89]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -25));
		pcRoom->m_pcSpawns[90]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -30));
		pcRoom->m_pcSpawns[91]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -35));
		pcRoom->m_pcSpawns[92]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -40));
		pcRoom->m_pcSpawns[93]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -25));
		pcRoom->m_pcSpawns[94]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -25));
		pcRoom->m_pcSpawns[95]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -25));
		pcRoom->m_pcSpawns[96]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -20));
		pcRoom->m_pcSpawns[97]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -20));
		pcRoom->m_pcSpawns[98]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -20));
		pcRoom->m_pcSpawns[99]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -15));
		pcRoom->m_pcSpawns[100]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -15));
		pcRoom->m_pcSpawns[101]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -15));
		pcRoom->m_pcSpawns[102]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -15));
		pcRoom->m_pcSpawns[103]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -15));
	}

	// Lights
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcLights.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Light));
			pcRoom->m_pcLights[i]->m_pcRoom = pcRoom;
		}

		//	Top
		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetPosition({ 15.0f, 4.0f, 47.5f });
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetPosition({ -15.0f, 4.0f, 47.5f });

		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetRotation(Rotation90);

		pcRoom->m_pcLights[0]->top = true;
		pcRoom->m_pcLights[1]->top = true;

		//	Right
		pcRoom->m_pcLights[2]->GetComponent<CTransform>()->SetPosition({ -47.5f, 4.0f,-15.0f });
		pcRoom->m_pcLights[3]->GetComponent<CTransform>()->SetPosition({ -47.5f, 4.0f, 15.0f });

		pcRoom->m_pcLights[2]->right = true;
		pcRoom->m_pcLights[3]->right = true;

		//	Bottom
		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetPosition({ -15.0f, 4.0f,-47.5f });
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetPosition({ 15.0f, 4.0f,-47.5f });

		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetRotation(Rotation90N);
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetRotation(Rotation90N);

		pcRoom->m_pcLights[4]->bottom = true;
		pcRoom->m_pcLights[5]->bottom = true;

		//	Left
		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetPosition({ 47.5f, 4.0f, 15.0f });
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetPosition({ 47.5f, 4.0f,-15.0f });

		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetRotation(Rotation180);
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetRotation(Rotation180);

		pcRoom->m_pcLights[6]->left = true;
		pcRoom->m_pcLights[7]->left = true;

		//pcRoom->m_pcLights[0]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[1]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[2]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[3]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[4]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[5]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[6]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
		//pcRoom->m_pcLights[7]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
	}

	pcRoom->SetActiveState(false);

	m_pRoomsType.push_back(pcRoom);

#pragma endregion

#pragma region Room 4

	pcRoom = nullptr;
	pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::PATH_SURFACE);
	pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM1);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM1);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::FLOOR_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::FLOOR_NORMAL, 1);
	pcMesh->SetTexture(eSRV::FLOOR_SPECULAR, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);

	//Spikes
	for (int i = 0; i < 3; i++)
	{
		pcRoom->m_pcTraps.push_back((CTrapEntity*)CEntityManager::CloneEntity(pcSpikes));
	}
	
	//Pillars
	for (int i = 0; i < 8; i++)
	{
		pcRoom->m_pcPillars.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Pillar));
		pcRoom->m_pcPillars[i]->m_pcRoom = pcRoom;

		pcRoom->m_pcPillars[i]->GetComponent<CTransform>()->SetScale({ 1.0f, 1.1f, 1.0f });
		if (i >= 4)
			pcRoom->m_pcPillars[i]->GetComponent<CTransform>()->SetScale({ 0.75f, 2.0f, 0.75f });
	}

	pcRoom->m_pcPillars[0]->GetComponent<CTransform>()->SetPosition({ 45.0f, 0.0f, 45.0f });
	pcRoom->m_pcPillars[1]->GetComponent<CTransform>()->SetPosition({ 45.0f, 0.0f,-45.0f });
	pcRoom->m_pcPillars[2]->GetComponent<CTransform>()->SetPosition({ -45.0f, 0.0f, 45.0f });
	pcRoom->m_pcPillars[3]->GetComponent<CTransform>()->SetPosition({ -45.0f, 0.0f,-45.0f });
	pcRoom->m_pcPillars[4]->GetComponent<CTransform>()->SetPosition({ 13, 2, 13 });

	pcRoom->m_pcPillars[4]->GetComponent<CBoxCollider>()->SetCenter({ 0,6,0 });
	
	pcRoom->m_pcPillars[5]->GetComponent<CTransform>()->SetPosition({ 22.5f, 0, -22.5f });
	pcRoom->m_pcPillars[6]->GetComponent<CTransform>()->SetPosition({ -22.5f, 0, 22.5f });
	pcRoom->m_pcPillars[7]->GetComponent<CTransform>()->SetPosition({ -22.5f, 0, -22.5f });

	pcRoom->m_pcPillars[4]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 0, 0 }, 90));

	#pragma region Clutter

	//rock1
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcRock1));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.2f,0.2f,0.2f });
	}

	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetPosition({ 13,0,13 });
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetPosition({ -20,0,-20 });
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetPosition({ 20,0,-20 });
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetPosition({ -20,0,20 });

	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 0, 0 }, -90));
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 0, 0 }, -90));
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 0, 0 }, -90));
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 0, 0 }, -90));

	//Rock 02
	for (int i = 0; i < 7; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcRock2));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetPosition({ 13,5,13.5 });
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetPosition({13,2,13.5 });
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetPosition({13,3,13.5 });
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetPosition({24,0,-18 });
	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetPosition({ -24,0,18 });
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetPosition({ -24,0,-18 });
	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetPosition({ 13,0,38 });

	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetRotation(Rotation45); 
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetRotation(Rotation45N);

	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetScale({ 2, 2, 2 });
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetScale({ 2, 2, 2 });
	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetScale({ 2, 2, 2 });
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetScale({ 1.5f, 1.5f, 1.5f });
	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetScale({ 2, 2, 2 });
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetScale({ 2, 2, 2 });
	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetScale({ 1.5f, 1.5f, 1.5f });

	//Rock 03
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcRock3));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetPosition({ 8,0,13});
	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetPosition({ 28,0,-28});
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetPosition({-28,0,28 });
	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetPosition({-28,0,-28 });

	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetRotation(Rotation45N);

	//Flag
	for (int i = 0; i < 8; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcFlag));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[15]->GetComponent<CTransform>()->SetPosition({ 27,0,47 });
	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetPosition({ -27,0,47 });
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetPosition({ 27,0,-47 });
	pcRoom->m_pcClutter[18]->GetComponent<CTransform>()->SetPosition({ -27,0,-47 });
	pcRoom->m_pcClutter[19]->GetComponent<CTransform>()->SetPosition({ -47,0,27 });
	pcRoom->m_pcClutter[20]->GetComponent<CTransform>()->SetPosition({ 47,0,-27 });
	pcRoom->m_pcClutter[21]->GetComponent<CTransform>()->SetPosition({ 47,0,27 });
	pcRoom->m_pcClutter[22]->GetComponent<CTransform>()->SetPosition({ -47,0,-27 });

	pcRoom->m_pcClutter[19]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[20]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[21]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[22]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Barrel01
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcBarrel01));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[23]->GetComponent<CTransform>()->SetPosition({ -35.5,0,45 });
	pcRoom->m_pcClutter[24]->GetComponent<CTransform>()->SetPosition({ -45,0,35.5 });
	pcRoom->m_pcClutter[25]->GetComponent<CTransform>()->SetPosition({ -18.5,0,45 });
	pcRoom->m_pcClutter[26]->GetComponent<CTransform>()->SetPosition({ -45,0,18.5 });

	pcRoom->m_pcClutter[24]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[26]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Sack
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Sack));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[27]->GetComponent<CTransform>()->SetPosition({ 35.5,0,-45});
	pcRoom->m_pcClutter[28]->GetComponent<CTransform>()->SetPosition({ 44,0,-35.5});
	pcRoom->m_pcClutter[29]->GetComponent<CTransform>()->SetPosition({ 18.5,0,-45});
	pcRoom->m_pcClutter[30]->GetComponent<CTransform>()->SetPosition({ 44,0,-18.5});

	pcRoom->m_pcClutter[28]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[30]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Logs
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Logs));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.8f,0.8f,0.8f });
	}

	pcRoom->m_pcClutter[31]->GetComponent<CTransform>()->SetPosition({ 35.5f,0,45 });
	pcRoom->m_pcClutter[32]->GetComponent<CTransform>()->SetPosition({ 45,0,35.5f });
	pcRoom->m_pcClutter[33]->GetComponent<CTransform>()->SetPosition({ 18.5f,0,45 });
	pcRoom->m_pcClutter[34]->GetComponent<CTransform>()->SetPosition({ 44,0,18.5f });

	pcRoom->m_pcClutter[32]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcClutter[34]->GetComponent<CTransform>()->SetRotation(Rotation90);

	//Cart
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Cart));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.5,0.5,0.5 });
	}

	pcRoom->m_pcClutter[35]->GetComponent<CTransform>()->SetPosition({ -35.5f,2,-45 });
	pcRoom->m_pcClutter[36]->GetComponent<CTransform>()->SetPosition({ -45,0,-35.5f });
	pcRoom->m_pcClutter[37]->GetComponent<CTransform>()->SetPosition({ -18.5f,0,-45 });
	pcRoom->m_pcClutter[38]->GetComponent<CTransform>()->SetPosition({ -44,0,-18.5f });

	pcRoom->m_pcClutter[35]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 1, 0, 0 }, 180));
	pcRoom->m_pcClutter[36]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[38]->GetComponent<CTransform>()->SetRotation(Rotation90);

	for (int i = 0; i < 12; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcChair));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[39]->GetComponent<CTransform>()->SetPosition({ 40,0,27 });
	pcRoom->m_pcClutter[40]->GetComponent<CTransform>()->SetPosition({ 27,0,40 });
	pcRoom->m_pcClutter[41]->GetComponent<CTransform>()->SetPosition({ 37, 0 ,37 });
	pcRoom->m_pcClutter[42]->GetComponent<CTransform>()->SetPosition({ -40,0,27 });
	pcRoom->m_pcClutter[43]->GetComponent<CTransform>()->SetPosition({ -27,0,40 });
	pcRoom->m_pcClutter[44]->GetComponent<CTransform>()->SetPosition({ -37, 0 ,37 });
	pcRoom->m_pcClutter[45]->GetComponent<CTransform>()->SetPosition({ 40,0,-27 });
	pcRoom->m_pcClutter[46]->GetComponent<CTransform>()->SetPosition({ 27,0,-40 });
	pcRoom->m_pcClutter[47]->GetComponent<CTransform>()->SetPosition({ 37, 0 ,-37 });
	pcRoom->m_pcClutter[48]->GetComponent<CTransform>()->SetPosition({ -40,0,-27 });
	pcRoom->m_pcClutter[49]->GetComponent<CTransform>()->SetPosition({ -27,0,-40 });
	pcRoom->m_pcClutter[50]->GetComponent<CTransform>()->SetPosition({ -37, 0 ,-37 });
	
	pcRoom->m_pcClutter[40]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[41]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[43]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[44]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[46]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[47]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[49]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcClutter[50]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	
#pragma endregion

	//Walls, SetExtent, Rotate Walls
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));
			pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;

			pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetScale({ 1.43f, 1, 1.7f });

			if (i < 4)
				pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(Rotation90);
			else
				pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(Rotation0);
		}

		pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition({ 26.0f, 0.0f, 48.0f });
		pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition({ -26.0f, 0.0f, 48.0f });
		pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition({ 26.0f, 0.0f,-48.0f });
		pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition({ -26.0f, 0.0f,-48.0f });
		pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetPosition({ -48.0f, 0.0f, 26.0f });
		pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetPosition({ 48.0f, 0.0f,-26.0f });
		pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetPosition({ 48.0f, 0.0f, 26.0f });
		pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetPosition({ -48.0f, 0.0f,-26.0f });
	}

	//Doors
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));
		pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
	}

	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(48.5f, 0, 0));
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-48.5f, 0, 0));
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 48.5f));
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -48.5f));

	//Rotate
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetRotation(Rotation0);
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(Rotation180);

	//SpawnPoints
	{
		for (int i = 0; i < 87; i++)
		{
			pcRoom->m_pcSpawns.push_back((CSpawnEntity*)CEntityManager::CloneEntity(Spawner));
		}

		pcRoom->m_pcSpawns[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 20));
		pcRoom->m_pcSpawns[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 25));
		pcRoom->m_pcSpawns[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 30));
		pcRoom->m_pcSpawns[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 35));
		pcRoom->m_pcSpawns[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 20));
		pcRoom->m_pcSpawns[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 25));
		pcRoom->m_pcSpawns[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 30));
		pcRoom->m_pcSpawns[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 35));
		pcRoom->m_pcSpawns[8]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 40));
		pcRoom->m_pcSpawns[9]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 25));
		pcRoom->m_pcSpawns[10]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 25));
		pcRoom->m_pcSpawns[11]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 20));
		pcRoom->m_pcSpawns[12]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 20));
		pcRoom->m_pcSpawns[13]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 20));
		pcRoom->m_pcSpawns[14]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, 15));
		pcRoom->m_pcSpawns[15]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, 15));
		pcRoom->m_pcSpawns[16]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, 15));
		pcRoom->m_pcSpawns[17]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, 15));
		pcRoom->m_pcSpawns[18]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -25));
		pcRoom->m_pcSpawns[19]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -30));
		pcRoom->m_pcSpawns[20]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, -35));
		pcRoom->m_pcSpawns[21]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -30));
		pcRoom->m_pcSpawns[22]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -35));
		pcRoom->m_pcSpawns[23]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, -40));
		pcRoom->m_pcSpawns[24]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -20));
		pcRoom->m_pcSpawns[25]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -25));
		pcRoom->m_pcSpawns[26]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -30));
		pcRoom->m_pcSpawns[27]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -35));
		pcRoom->m_pcSpawns[28]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -40));
		pcRoom->m_pcSpawns[29]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -25));
		pcRoom->m_pcSpawns[30]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -25));
		pcRoom->m_pcSpawns[31]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -20));
		pcRoom->m_pcSpawns[32]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -20));
		pcRoom->m_pcSpawns[33]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -20));
		pcRoom->m_pcSpawns[34]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, -15));
		pcRoom->m_pcSpawns[35]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, -15));
		pcRoom->m_pcSpawns[36]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -15));
		pcRoom->m_pcSpawns[37]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-25, 0, 35));
		pcRoom->m_pcSpawns[38]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 25));
		pcRoom->m_pcSpawns[39]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 30));
		pcRoom->m_pcSpawns[40]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 35));
		pcRoom->m_pcSpawns[41]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-20, 0, 40));
		pcRoom->m_pcSpawns[42]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 20));
		pcRoom->m_pcSpawns[43]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 25));
		pcRoom->m_pcSpawns[44]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 30));
		pcRoom->m_pcSpawns[45]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 35));
		pcRoom->m_pcSpawns[46]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 40));
		pcRoom->m_pcSpawns[47]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 25));
		pcRoom->m_pcSpawns[48]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 25));
		pcRoom->m_pcSpawns[49]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 20));
		pcRoom->m_pcSpawns[50]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 20));
		pcRoom->m_pcSpawns[51]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 20));
		pcRoom->m_pcSpawns[52]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-30, 0, 15));
		pcRoom->m_pcSpawns[53]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35, 0, 15));
		pcRoom->m_pcSpawns[54]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 15));
		pcRoom->m_pcSpawns[55]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -30));
		pcRoom->m_pcSpawns[56]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -35));
		pcRoom->m_pcSpawns[57]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -30));
		pcRoom->m_pcSpawns[58]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -35));
		pcRoom->m_pcSpawns[59]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(20, 0, -40));
		pcRoom->m_pcSpawns[60]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -20));
		pcRoom->m_pcSpawns[61]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -25));
		pcRoom->m_pcSpawns[62]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -30));
		pcRoom->m_pcSpawns[63]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -35));
		pcRoom->m_pcSpawns[64]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -40));
		pcRoom->m_pcSpawns[65]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -25));
		pcRoom->m_pcSpawns[66]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -25));
		pcRoom->m_pcSpawns[67]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -20));
		pcRoom->m_pcSpawns[68]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -20));
		pcRoom->m_pcSpawns[69]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -20));
		pcRoom->m_pcSpawns[70]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(25, 0, -15));
		pcRoom->m_pcSpawns[71]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(30, 0, -15));
		pcRoom->m_pcSpawns[72]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(35, 0, -15));
		pcRoom->m_pcSpawns[73]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -15));
		pcRoom->m_pcSpawns[74]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0,0,0));
		pcRoom->m_pcSpawns[75]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5,0,0));
		pcRoom->m_pcSpawns[76]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10,0,0));
		pcRoom->m_pcSpawns[77]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15,0,0));
		pcRoom->m_pcSpawns[78]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5,0,0));
		pcRoom->m_pcSpawns[79]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10,0,0));
		pcRoom->m_pcSpawns[80]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15,0,0));
		pcRoom->m_pcSpawns[81]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0,0,5));
		pcRoom->m_pcSpawns[82]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0,0,10));
		pcRoom->m_pcSpawns[83]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0,0,15));
		pcRoom->m_pcSpawns[84]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0,0,-5));
		pcRoom->m_pcSpawns[85]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0,0,-10));
		pcRoom->m_pcSpawns[86]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0,0,-15));
	}

	// Lights
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcLights.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Light));
			pcRoom->m_pcLights[i]->m_pcRoom = pcRoom;
		}

		//	Top
		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetPosition({ 15.0f, 4.0f, 47.5f });
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetPosition({ -15.0f, 4.0f, 47.5f });

		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetRotation(Rotation90);

		pcRoom->m_pcLights[0]->top = true;
		pcRoom->m_pcLights[1]->top = true;

		//	Right
		pcRoom->m_pcLights[2]->GetComponent<CTransform>()->SetPosition({ -47.5f, 4.0f,-15.0f });
		pcRoom->m_pcLights[3]->GetComponent<CTransform>()->SetPosition({ -47.5f, 4.0f, 15.0f });

		pcRoom->m_pcLights[2]->right = true;
		pcRoom->m_pcLights[3]->right = true;

		//	Bottom
		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetPosition({ -15.0f, 4.0f,-47.5f });
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetPosition({ 15.0f, 4.0f,-47.5f });

		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetRotation(Rotation90N);
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetRotation(Rotation90N);

		pcRoom->m_pcLights[4]->bottom = true;
		pcRoom->m_pcLights[5]->bottom = true;

		////	Left
		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetPosition({ 47.5f, 4.0f, 15.0f });
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetPosition({ 47.5f, 4.0f,-15.0f });

		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetRotation(Rotation180);
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetRotation(Rotation180);

		pcRoom->m_pcLights[6]->left = true;
		pcRoom->m_pcLights[7]->left = true;

		//pcRoom->m_pcLights[0]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[1]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[2]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[3]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[4]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[5]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[6]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
		//pcRoom->m_pcLights[7]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
	}
	
	//Enemies
	pcRoom->SetActiveState(false);

	m_pRoomsType.push_back(pcRoom);
	
#pragma endregion

#pragma region Room 5

	pcRoom = nullptr;
	pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::PATH_SURFACE);
	pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM5);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM5);
	pcMesh->SetTextureCount(3);
	pcMesh->SetTexture(eSRV::FLOOR_DIFFUSE, 0);
	pcMesh->SetTexture(eSRV::FLOOR_NORMAL, 1);
	pcMesh->SetTexture(eSRV::FLOOR_SPECULAR, 2);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);

	//Spikes
	for (int i = 0; i < 3; i++)
	{
		pcRoom->m_pcTraps.push_back((CTrapEntity*)CEntityManager::CloneEntity(pcSpikes));
	}

	//Pillars
	for (int i = 0; i < 8; i++)
	{
		pcRoom->m_pcPillars.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Pillar));
		pcRoom->m_pcPillars[i]->m_pcRoom = pcRoom;
		pcRoom->m_pcPillars[i]->GetComponent<CTransform>()->SetScale({ 1.0f, 1.1f, 1.0f });
	}

	pcRoom->m_pcPillars[0]->GetComponent<CTransform>()->SetPosition({ 14.0f, 0.0f, 45.0f });
	pcRoom->m_pcPillars[1]->GetComponent<CTransform>()->SetPosition({ 45.0f, 0.0f, 14.0f });
	pcRoom->m_pcPillars[2]->GetComponent<CTransform>()->SetPosition({-14.0f, 0.0f, 45.0f });
	pcRoom->m_pcPillars[3]->GetComponent<CTransform>()->SetPosition({ 45.0f, 0.0f,-14.0f });
	pcRoom->m_pcPillars[4]->GetComponent<CTransform>()->SetPosition({ 14.0f, 0.0f,-45.0f });
	pcRoom->m_pcPillars[5]->GetComponent<CTransform>()->SetPosition({-45.0f, 0.0f, 14.0f });
	pcRoom->m_pcPillars[6]->GetComponent<CTransform>()->SetPosition({-14.0f, 0.0f,-45.0f });
	pcRoom->m_pcPillars[7]->GetComponent<CTransform>()->SetPosition({-45.0f, 0.0f,-14.0f });

	//Flags
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcFlag));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}
	
	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetPosition({ 33,0,33 });
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetPosition({ -33,0,33 });
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetPosition({ 33,0,-33 });
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetPosition({ -33,0,-33 });

	pcRoom->m_pcClutter[0]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[1]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[2]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[3]->GetComponent<CTransform>()->SetRotation(Rotation45);

	//Barrel01
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcBarrel01));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetPosition({ 35,0,25 });
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetPosition({ 25,0,35 });

	pcRoom->m_pcClutter[4]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[5]->GetComponent<CTransform>()->SetRotation(Rotation45);
	
	//Sack
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Sack));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetPosition({ 35,0,-25 });
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetPosition({ 25,0,-35 });

	pcRoom->m_pcClutter[6]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[7]->GetComponent<CTransform>()->SetRotation(Rotation45);

	//Logs
	for (int i = 0; i < 2; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Logs));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 0.8f,0.8f,0.8f });
	}

	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetPosition({ -35,0,25 });
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetPosition({ -25,0,35 });

	pcRoom->m_pcClutter[8]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[9]->GetComponent<CTransform>()->SetRotation(Rotation45);

	//Chairs
	for (int i = 0; i < 8; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcChair));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetPosition({ -35,0,15 });
	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetPosition({ -15,0,35 });
	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetPosition({ 35,0,-15 });
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetPosition({ 15,0,-35 });
	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetPosition({ -35,0,-15 });
	pcRoom->m_pcClutter[15]->GetComponent<CTransform>()->SetPosition({ -15,0,-35 });
	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetPosition({ 35,0,15 });
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetPosition({ 15,0,35 });

	pcRoom->m_pcClutter[10]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[11]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[12]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[13]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcClutter[14]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[15]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[16]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcClutter[17]->GetComponent<CTransform>()->SetRotation(Rotation45N);

	//Statues
	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcClutter.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(pcStatue));
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->m_pcRoom = pcRoom;
		pcRoom->m_pcClutter[pcRoom->m_pcClutter.size() - 1]->GetComponent<CTransform>()->SetScale({ 1,1,1 });
	}

	pcRoom->m_pcClutter[18]->GetComponent<CTransform>()->SetPosition({ 0,0,0 });

	pcRoom->m_pcClutter[18]->GetComponent<CTransform>()->SetRotation(Rotation90N);

	//Walls, SetExtent
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));
		pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;

		pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetScale({ 1.43f, 1.0f, 2.2f });
	}

	pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition({ 33.8f, 0, 33.8f });
	pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition({-33.8f, 0, 33.8f });
	pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition({ 33.8f, 0,-33.8f });
	pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition({-33.8f, 0,-33.8f });
	
	pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetRotation(Rotation45N);
	pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetRotation(Rotation45);
	pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetRotation(Rotation45N);


	//Doors
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));
		pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
	}

	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition({ 48.5f, 0, 0.0f });
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition({-48.5f, 0, 0.0f });
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition({ 0.0f, 0, 48.5f });
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition({ 0.0f, 0,-48.5f });
	
	//Rotate
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetRotation(Rotation0);
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(Rotation90);
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(Rotation90N);
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(Rotation180);
	
	//SpawnPoints
	for (int i = 0; i < 40; i++)
	{
		pcRoom->m_pcSpawns.push_back((CSpawnEntity*)CEntityManager::CloneEntity(Spawner));
	}

	pcRoom->m_pcSpawns[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -15));
	pcRoom->m_pcSpawns[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 0));
	pcRoom->m_pcSpawns[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, 0));
	pcRoom->m_pcSpawns[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 5));
	pcRoom->m_pcSpawns[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 10));
	pcRoom->m_pcSpawns[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 5));
	pcRoom->m_pcSpawns[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, 5));
	pcRoom->m_pcSpawns[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 10));
	pcRoom->m_pcSpawns[8]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, 10));
	pcRoom->m_pcSpawns[9]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 0));
	pcRoom->m_pcSpawns[10]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, 0));
	pcRoom->m_pcSpawns[11]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 5));
	pcRoom->m_pcSpawns[12]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, 5));
	pcRoom->m_pcSpawns[13]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 10));
	pcRoom->m_pcSpawns[14]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, 10));
	pcRoom->m_pcSpawns[15]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -5));
	pcRoom->m_pcSpawns[16]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -10));
	pcRoom->m_pcSpawns[17]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, -5));
	pcRoom->m_pcSpawns[18]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, -10));
	pcRoom->m_pcSpawns[19]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, -5));
	pcRoom->m_pcSpawns[20]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, -10));
	pcRoom->m_pcSpawns[21]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, -5));
	pcRoom->m_pcSpawns[22]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, -10));
	pcRoom->m_pcSpawns[23]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, -5));
	pcRoom->m_pcSpawns[24]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, -10));
	pcRoom->m_pcSpawns[25]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 0));
	pcRoom->m_pcSpawns[26]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 15));
	pcRoom->m_pcSpawns[27]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 5));
	pcRoom->m_pcSpawns[28]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, 15));
	pcRoom->m_pcSpawns[29]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, 15));
	pcRoom->m_pcSpawns[30]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 0));
	pcRoom->m_pcSpawns[31]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 5));
	pcRoom->m_pcSpawns[32]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, 15));
	pcRoom->m_pcSpawns[33]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, 15));
	pcRoom->m_pcSpawns[34]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -15));
	pcRoom->m_pcSpawns[35]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-5, 0, -15));
	pcRoom->m_pcSpawns[36]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -5));
	pcRoom->m_pcSpawns[37]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-15, 0, -15));
	pcRoom->m_pcSpawns[38]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(5, 0, -15));
	pcRoom->m_pcSpawns[39]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(15, 0, -5));

	// Lights
	{
		for (int i = 0; i < 8; i++)
		{
			pcRoom->m_pcLights.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Light));
			pcRoom->m_pcLights[i]->m_pcRoom = pcRoom;
		}

		//	Top
		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetPosition({ 14.0f, 5.0f, 42.0f });
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetPosition({ -14.0f, 5.0f, 42.0f });

		pcRoom->m_pcLights[0]->GetComponent<CTransform>()->SetRotation(Rotation90);
		pcRoom->m_pcLights[1]->GetComponent<CTransform>()->SetRotation(Rotation90);

		pcRoom->m_pcLights[0]->top = true;
		pcRoom->m_pcLights[1]->top = true;

		//	Right
		pcRoom->m_pcLights[2]->GetComponent<CTransform>()->SetPosition({ -42.0f, 5.0f,-14.0f });
		pcRoom->m_pcLights[3]->GetComponent<CTransform>()->SetPosition({ -42.0f, 5.0f, 14.0f });

		pcRoom->m_pcLights[2]->right = true;
		pcRoom->m_pcLights[3]->right = true;

		//	Bottom
		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetPosition({ -14.0f, 5.0f,-42.0f });
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetPosition({ 14.0f, 5.0f,-42.0f });

		pcRoom->m_pcLights[4]->GetComponent<CTransform>()->SetRotation(Rotation90N);
		pcRoom->m_pcLights[5]->GetComponent<CTransform>()->SetRotation(Rotation90N);

		pcRoom->m_pcLights[4]->bottom = true;
		pcRoom->m_pcLights[5]->bottom = true;

		//	Left
		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetPosition({ 42.0f, 5.0f, 14.0f });
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetPosition({ 42.0f, 5.0f,-14.0f });

		pcRoom->m_pcLights[6]->GetComponent<CTransform>()->SetRotation(Rotation180);
		pcRoom->m_pcLights[7]->GetComponent<CTransform>()->SetRotation(Rotation180);

		pcRoom->m_pcLights[6]->left = true;
		pcRoom->m_pcLights[7]->left = true;

		//pcRoom->m_pcLights[0]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[1]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, -2.7f));
		//pcRoom->m_pcLights[2]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[3]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(2.7f, 5, 0));
		//pcRoom->m_pcLights[4]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[5]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(0, 5, 2.7f));
		//pcRoom->m_pcLights[6]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
		//pcRoom->m_pcLights[7]->GetComponent<CParticleEmitter>()->SetOffset(CMath::TVECTOR3(-2.5f, 5, 0));
	}
	
	pcRoom->SetActiveState(false);

	m_pRoomsType.push_back(pcRoom);
	
#pragma endregion

	m_pcCurrentLevel = new CLevel();

	CLevel::RandomizeRoomEnemies(m_pcEnemies, floor);
}

CLevelManager::~CLevelManager()
{
	if (m_pcCurrentLevel)	delete m_pcCurrentLevel;
}

void CLevelManager::DeleteLevel()
{
	if (m_pcCurrentLevel)	delete m_pcCurrentLevel;
}