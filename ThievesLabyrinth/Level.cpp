#include "LevelManager.h"
#include "GUIManager.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "ParticleEffect.h"
#include "PathSurface.h"
#include "Transform.h"
#include "Entity.h"
#include "MeshRenderer.h"
#include "EnumTypes.h"
#include "Stats.h"

#include <time.h>
#include <iostream>
#include <vector>
#include <list>

int CLevel::Floor;

#define	ROOMS rand() % 5

std::vector<CRoomEntity*> CLevel::m_pcRooms;
CRoomEntity* CLevel::m_pcCurrentRoom;
CRoomEntity* CLevel::m_pcStartRoom;
CRoomEntity* CLevel::m_pcEndRoom;
Spot CLevel::CurrentSpot;

int CLevel::nTotalEnemyCount = 0;

std::vector < std::vector < Spot > > cbMaze;
std::list < std::pair < int, int> > cnTunnels;

CLevel::CLevel()
{
	cbMaze.clear();
	cnTunnels.clear();
	cbMaze.resize(nColumns);
	for (int y = 0; y < nColumns; y++)
	{
		cbMaze[y].resize(nRows);
	}

	for (int x = 0; x < nRows; x++)
	{
		for (int y = 0; y < nColumns; y++)
		{
			cbMaze[y][x].bRoom = false;
		}
	}

	cnTunnels.push_back(std::make_pair(nRows / 2, nColumns / 2));
	
	while (cnTunnels.size() > 0)
	{
		std::list < std::pair < int, int> >::iterator  cnBegin, cnEnd, cnTemp;
		cnBegin = cnTunnels.begin();
		cnEnd = cnTunnels.end();

		while (cnBegin != cnEnd)
		{
			bool bRemoveTunnel = false;
			switch (rand() % 4)
			{
			case 0:
			{
				(*cnBegin).second -= 2;

				if ((*cnBegin).second < 0 || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;
					break;
				}

				cbMaze[(*cnBegin).second + 1][(*cnBegin).first].bRoom = true;

				break;
			}

			case 1:
			{
				(*cnBegin).second += 2;

				if ((*cnBegin).second >= nColumns || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;

					break;
				}

				cbMaze[(*cnBegin).second - 1][(*cnBegin).first].bRoom = true;

				break;
			}

			case 2:
			{
				(*cnBegin).first -= 2;

				if ((*cnBegin).first < 0 || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;
					break;
				}

				cbMaze[(*cnBegin).second][(*cnBegin).first + 1].bRoom = true;

				break;
			}

			case 3:
			{
				(*cnBegin).first += 2;

				if ((*cnBegin).first >= nRows || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;

					break;
				}

				cbMaze[(*cnBegin).second][(*cnBegin).first - 1].bRoom = true;

				break;
			}

			}
			if (bRemoveTunnel)
			{
				cnBegin = cnTunnels.erase(cnBegin);
			}
			else
			{
				cnTunnels.push_back(std::make_pair((*cnBegin).first, (*cnBegin).second));
				
				cnTunnels.push_back(std::make_pair((*cnBegin).first, (*cnBegin).second));

				cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom = true;

				++cnBegin;
			}
		}
	}

	for (int x = 0; x < nRows; x++)
	{
		for (int y = 0; y < nColumns; y++)
		{
			if (cbMaze[y][x].bRoom == true)
			{
				CRoomEntity* room = (CRoomEntity*)CEntityManager::CloneEntity(CLevelManager::m_pRoomsType[ROOMS]);
	
				room->SetActiveState(false);
				room->Doors = false;
				room->Entities = false;
				
				switch (rand() % 4)
				{
				case 1:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
					break;
				}

				case 2:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90.0f));
					break;
				}

				case 3:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 180.0f));
					break;
				}

				case 4:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 270.0f));
					break;
				}
				default:

					break;
				}

				cbMaze[y][x].Room = room;

				room->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3((float)y * nWidth, 0, (float)x * nWidth));

				room->Visited = false;

				m_pcRooms.push_back(room);
			}
		}
	}
	nTotalEnemyCount = 0;
	int RoomSize = (int)m_pcRooms.size();
	for (int i = 0; i < RoomSize; i++)
	{
		nTotalEnemyCount += m_pcRooms[i]->m_nEnemyCount;
	}

	for (int x = 0; x < nRows; x++)
	{
		for (int y = 0; y < nColumns; y++)
		{
			if (cbMaze[y][x].bRoom == true)
			{
				if (y - 1 >= 0)
				{
					if (cbMaze[y - 1][x].bRoom == true)
					{
						cbMaze[y - 1][x].Room->Bottom = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y - 1][x].Room);
						cbMaze[y][x].Room->Top = true;
					}
				}

				if (y + 1 < nColumns)
				{
					if (cbMaze[y + 1][x].bRoom == true)
					{
						cbMaze[y + 1][x].Room->Top = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y + 1][x].Room);
						cbMaze[y][x].Room->Bottom = true;
					}
				}

				if (x - 1 >= 0)
				{
					if (cbMaze[y][x - 1].bRoom == true)
					{
						cbMaze[y][x - 1].Room->Right = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y][x - 1].Room);
						cbMaze[y][x].Room->Left = true;
					}
				}

				if (x + 1 < nRows)
				{
					if (cbMaze[y][x + 1].bRoom == true)
					{
						cbMaze[y][x + 1].Room->Left = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y][x + 1].Room);
						cbMaze[y][x].Room->Right = true;
					}
				}

				if (nstart == 0)
				{
					CurrentSpot = cbMaze[y][x];
					m_pcStartRoom = cbMaze[y][x].Room;
					m_pcCurrentRoom = m_pcStartRoom;
					nstart++;
				}

				m_pcEndRoom = cbMaze[y][x].Room;

			}
		}
	}

	CGUIManager::SetMiniMap(cbMaze);
	m_pcCurrentRoom->m_nEnemyCount = 0;
	UpdateDoors(m_pcCurrentRoom);
}

CLevel::~CLevel()
{
	int RoomSize = (int)m_pcRooms.size();
	for (size_t i = 0; i <RoomSize; i++)
	{
		m_pcRooms[i]->m_nEnemyCount = 0;
	}

	m_pcRooms.clear();
	nTotalEnemyCount = 0;
}

void CLevel::Update(CRoomEntity* Room)
{
	m_pcCurrentRoom = Room;
	m_pcCurrentRoom->Visited = true;

	/*for (size_t i = 0; i < Room->m_pcEnemies.size(); i++)
	{
		if (Room->m_pcEnemies[i]->GetComponent<IEnemyController>()->GetEnemyType() == eEnemyType::CHICKEN)
		{
			CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::FUSE));
		}
	}*/

	for (int i = 0; i < Room->m_pcTraps.size(); i++)
	{
		Room->m_pcTraps[i]->SetActiveState(true);
	}

	for(CEnemyEntity* pcEnemy : Room->m_pcEnemies) 
	{
		CParticleEmitter* pcEmitter = pcEnemy->GetComponent<CParticleEmitter>();

		if(pcEmitter)
		{
			pcEmitter->RefreshEmitterSettings();
		}
	}	

	int NeighborsSize = (int)Room->m_pcNeighbors.size();

	for (int i = 0; i < NeighborsSize; i++)
	{
		Room->m_pcNeighbors[i]->SpawnDoors();
		Room->m_pcNeighbors[i]->SpawnEntities();

		for (int j = 0; j < Room->m_pcNeighbors[i]->m_pcTraps.size(); j++)
		{
			Room->m_pcNeighbors[i]->m_pcTraps[j]->SetActiveState(false);
		}
		
		UpdateLights(Room->m_pcNeighbors[i]);
	}

	UpdateLights(Room);
	
	if (Room->m_nEnemyCount != 0)
	{
		if (Room->Right == true)
		{
			Room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Left == true)
		{
			Room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Top == true)
		{
			Room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Bottom == true)
		{
			Room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}

		Room->TurnEnemiesOn();
	}

	if (Room->m_nEnemyCount == 0)
	{
		if (Room->Right == true)
		{
			Room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Left == true)
		{
			Room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Top == true)
		{
			Room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Bottom == true)
		{
			Room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
	}

	nTotalEnemyCount = 0;
	int RoomSize = (int)m_pcRooms.size();
	for (int i = 0; i < RoomSize; i++)
	{
		nTotalEnemyCount += m_pcRooms[i]->m_nEnemyCount;
	}
}

void CLevel::UpdateDoors(CRoomEntity* Room)
{
	if (Room->m_nEnemyCount <= 0)
	{
		CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::DOOR_OPEN));
		int NeighborsSize = (int)Room->m_pcNeighbors.size();
		for (size_t i = 0; i < NeighborsSize; i++)
		{
			int DoorSize = (int)Room->m_pcNeighbors[i]->m_pcDoors.size();
			for (size_t j = 0; j < DoorSize; j++)
			{
				if (Room->m_pcNeighbors[i]->Right == true)
				{
					Room->m_pcNeighbors[i]->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(true);
					Room->m_pcNeighbors[i]->m_pcDoors[2]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
				}
				if (Room->m_pcNeighbors[i]->Left == true)
				{
					Room->m_pcNeighbors[i]->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(true);
					Room->m_pcNeighbors[i]->m_pcDoors[3]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
				}
				if (Room->m_pcNeighbors[i]->Top == true)
				{
					Room->m_pcNeighbors[i]->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(true);
					Room->m_pcNeighbors[i]->m_pcDoors[1]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
				}
				if (Room->m_pcNeighbors[i]->Bottom == true)
				{
					Room->m_pcNeighbors[i]->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(true);
					Room->m_pcNeighbors[i]->m_pcDoors[0]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
				}
			}
		}

		if (Room->Right == true)
		{
			Room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(true);
			Room->m_pcDoors[2]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
		}
		if (Room->Left == true)
		{
			Room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(true);
			Room->m_pcDoors[3]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
		}
		if (Room->Top == true)
		{
			Room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(true);
			Room->m_pcDoors[1]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
		}
		if (Room->Bottom == true)
		{
			Room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(true);
			Room->m_pcDoors[0]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::OPENDOOR);
		}
	}
}

void CLevel::SetDoorsTrue(CRoomEntity * room)
{
	if (room->m_nEnemyCount > 0)
	{
		CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::DOOR_CLOSE));
		int NeighborsSize = (int)room->m_pcNeighbors.size();
		for (size_t i = 0; i < NeighborsSize ; i++)
		{
			int DoorSize = (int)room->m_pcNeighbors[i]->m_pcDoors.size();
			for (size_t j = 0; j < DoorSize; j++)
			{
				if (room->m_pcNeighbors[i]->Right == true)
				{
					room->m_pcNeighbors[i]->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(false);
					room->m_pcNeighbors[i]->m_pcDoors[2]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
				}
				if (room->m_pcNeighbors[i]->Left == true)
				{
					room->m_pcNeighbors[i]->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(false);
					room->m_pcNeighbors[i]->m_pcDoors[3]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
				}
				if (room->m_pcNeighbors[i]->Top == true)
				{
					room->m_pcNeighbors[i]->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(false);
					room->m_pcNeighbors[i]->m_pcDoors[1]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
				}
				if (room->m_pcNeighbors[i]->Bottom == true)
				{
					room->m_pcNeighbors[i]->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(false);
					room->m_pcNeighbors[i]->m_pcDoors[0]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
				}
			}
		}

		if (room->Right == true)
		{
			room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(false);
			room->m_pcDoors[2]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
		}
		if (room->Left == true)
		{
			room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(false);
			room->m_pcDoors[3]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
		}
		if (room->Top == true)
		{
			room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(false);
			room->m_pcDoors[1]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
		}
		if (room->Bottom == true)
		{
			room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(false);
			room->m_pcDoors[0]->GetComponent<CMeshRenderer>()->SetVertexBuffer(eVertexBuffer::DOOR);
		}
	}

	
}

void CLevel::RandomizeRoomEnemies(std::vector<CEnemyEntity*> pcEnemies, int floor)
{
	nTotalEnemyCount = 0;
	size_t columns = cbMaze.size();
	for (size_t i = 0; i < columns; i++)
	{
		size_t rows = cbMaze[i].size();
		for (size_t j = 0; j < rows; j++)
		{
			if (cbMaze[i][j].bRoom)
			{
				int random = rand() % cbMaze[i][j].Room->m_pcSpawns.size();

				float distance = CMath::Vector3Magnitude(m_pcStartRoom->GetComponent<CTransform>()->GetPosition() - cbMaze[i][j].Room->GetComponent<CTransform>()->GetPosition());

				float roomsaway = distance / 100;

				if (roomsaway > 9)
				{
					roomsaway = 9;
				}

				CStats* stats;
				
					for (int k = 0; k < roomsaway; k++)
					{
						cbMaze[i][j].Room->m_pcEnemies.push_back((CEnemyEntity*)CEntityManager::CloneEntity(pcEnemies[rand() % pcEnemies.size()]));

						switch (floor)
						{

						case 2:
						{
							stats = (CStats*)cbMaze[i][j].Room->m_pcEnemies[k]->GetComponent<CStats>();
							stats->SetBaseDamage(stats->GetBaseDamage() + 0.5f);
							stats->SetMaxHP(stats->GetMaxHP() + 0.5f);
							break;
						}

						case 3:
						{
							stats = (CStats*)cbMaze[i][j].Room->m_pcEnemies[k]->GetComponent<CStats>();
							stats->SetBaseDamage(stats->GetBaseDamage() + 1);
							stats->SetMaxHP(stats->GetMaxHP() + 1);
							break;
						}
						case 4:
						{
							stats = (CStats*)cbMaze[i][j].Room->m_pcEnemies[k]->GetComponent<CStats>();
							stats->SetBaseDamage(stats->GetBaseDamage() + 1.5f);
							stats->SetMaxHP(stats->GetMaxHP() + 1.5f);
							break;
						}

						case 5:
						{
							stats = (CStats*)cbMaze[i][j].Room->m_pcEnemies[k]->GetComponent<CStats>();
							stats->SetBaseDamage(stats->GetBaseDamage() + 2);
							stats->SetMaxHP(stats->GetMaxHP() + 2);
							break;
						}


						default:
							break;
						}
							
						cbMaze[i][j].Room->m_pcEnemies[k]->m_pcRoom = cbMaze[i][j].Room;
					}
				

				int size = (int)cbMaze[i][j].Room->m_pcEnemies.size();

				if (size > (int)cbMaze[i][j].Room->m_pcSpawns.size())
				{
					size = (int)cbMaze[i][j].Room->m_pcSpawns.size();
				}
				
				for (int k = 0; k < size; k++)
				{
					while (cbMaze[i][j].Room->m_pcSpawns[random]->Taken == true)
					{
						random = rand() % cbMaze[i][j].Room->m_pcSpawns.size();
					}

					cbMaze[i][j].Room->m_pcEnemies[k]->GetComponent<CTransform>()->SetPosition(cbMaze[i][j].Room->m_pcSpawns[rand() % cbMaze[i][j].Room->m_pcSpawns.size()]->GetComponent<CTransform>()->GetPosition());
					cbMaze[i][j].Room->m_pcSpawns[random]->Taken = true;
				}

				for (int k = 0; k < 3; k++)
				{
					cbMaze[i][j].Room->m_pcTraps[k]->GetComponent<CTransform>()->SetPosition(cbMaze[i][j].Room->m_pcSpawns[rand() % cbMaze[i][j].Room->m_pcSpawns.size()]->GetComponent<CTransform>()->GetPosition());
				}

				cbMaze[i][j].Room->m_nEnemyCount = (int)cbMaze[i][j].Room->m_pcEnemies.size();
				nTotalEnemyCount += cbMaze[i][j].Room->m_nEnemyCount;
			}
		}
	}

	m_pcCurrentRoom = CurrentSpot.Room;
	m_pcCurrentRoom->Visited = true;
	CurrentSpot.Room->SpawnEntities();
	CurrentSpot.Room->SpawnDoors();

	for (int i = 0; i < (int)CurrentSpot.Room->m_pcNeighbors.size(); i++)
	{
		CurrentSpot.Room->m_pcNeighbors[i]->SpawnEntities();
		CurrentSpot.Room->m_pcNeighbors[i]->SpawnDoors();
	}

	CLevel::Update(CurrentSpot.Room);
}

int CLevel::GetTotalEnemyCount()
{
	return nTotalEnemyCount;
}

void CLevel::UpdateLights(CRoomEntity * pcRoom)
{
	int LightsSize = (int)pcRoom->m_pcLights.size();
	for (size_t i = 0; i < LightsSize; i++)
	{
		pcRoom->RemoveLight(pcRoom->m_pcLights[i]->GetComponent<CLightComponent>());
	}

	if (pcRoom->Right)
	{
		for (size_t i = 0; i < 2; i++)
		{
			pcRoom->m_pcLights[i]->SetActiveState(true);
			pcRoom->m_pcLights[i]->GetComponent<CParticleEmitter>()->RefreshEmitterSettings();

			pcRoom->AddLight(pcRoom->m_pcLights[i]->GetComponent<CLightComponent>());
		}
	}

	if (pcRoom->Top)
	{
		for (size_t i = 2; i < 4; i++)
		{
			pcRoom->m_pcLights[i]->SetActiveState(true);
			pcRoom->m_pcLights[i]->GetComponent<CParticleEmitter>()->RefreshEmitterSettings();

			pcRoom->AddLight(pcRoom->m_pcLights[i]->GetComponent<CLightComponent>());
		}
	}

	if (pcRoom->Left)
	{
		for (size_t i = 4; i < 6; i++)
		{
			pcRoom->m_pcLights[i]->SetActiveState(true);
			pcRoom->m_pcLights[i]->GetComponent<CParticleEmitter>()->RefreshEmitterSettings();

			pcRoom->AddLight(pcRoom->m_pcLights[i]->GetComponent<CLightComponent>());
		}
	}

	if (pcRoom->Bottom)
	{
		for (size_t i = 6; i < 8; i++)
		{
			pcRoom->m_pcLights[i]->SetActiveState(true);
			pcRoom->m_pcLights[i]->GetComponent<CParticleEmitter>()->RefreshEmitterSettings();

			pcRoom->AddLight(pcRoom->m_pcLights[i]->GetComponent<CLightComponent>());
		}
	}
}