#include "ClientPacketHandler.h"
#include "Network/BufferReader.h"
#include "System/R3GameInstance.h"
#include "R3.h"
#include <Kismet/GameplayStatics.h>
#include <Components/Widget.h>
#include "Network/PacketSession.h"
#include "Network/NetworkManager.h"
#include "GameFramework/GameStateBase.h"
#include "R3/GameModes/R3MultCharCreateionComponent.h"
#include "R3/Character/R3Spawner.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	for (auto& Player : pkt.players())
	{
	}

	for (int32 i = 0; i < pkt.players_size(); i++)
	{
		const Protocol::ObjectInfo& Player = pkt.players(i);
	}

	// 로비에서 캐릭터 선택해서 인덱스 전송.
	Protocol::C_ENTER_GAME EnterGamePkt;
	EnterGamePkt.set_playerindex(0);
	SEND_PACKET(EnterGamePkt);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (AGameStateBase* GameState = Cast<AGameStateBase>(GWorld->GetGameState()))
	{
		const Protocol::ObjectInfo& Player = pkt.player();

		UR3MultCharCreateionComponent* Spanwer = GameState->FindComponentByClass<UR3MultCharCreateionComponent>();

		if (!Spanwer->ContainPlayer(Player.object_id()))
			Spanwer->RestartPlayer(Player);
	}

	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	if (auto* GameInstance = Cast<UR3GameInstance>(GWorld->GetGameInstance()))
	{
	}

	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	if (AGameStateBase* GameState = Cast<AGameStateBase>(GWorld->GetGameState()))
	{
		UR3MultCharCreateionComponent* Spanwer = GameState->FindComponentByClass<UR3MultCharCreateionComponent>();

		for (int32 i = 0; i < pkt.players_size(); i++)
		{
			const Protocol::ObjectInfo& Player = pkt.players(i);

			if (!Spanwer->ContainPlayer(Player.object_id()))
				Spanwer->SpawnMultiplayerCharacter(Player);
		}
	}

	return false;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	if (auto* GameInstance = Cast<UR3GameInstance>(GWorld->GetGameInstance()))
	{
		//GameInstance->HandleDespawn(pkt);
	}

	return true;
}


bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	if (AGameStateBase* GameState = Cast<AGameStateBase>(GWorld->GetGameState()))
	{
		UR3MultCharCreateionComponent* Spanwer = GameState->FindComponentByClass<UR3MultCharCreateionComponent>();
		Spanwer->UpdateCharacterMovement(pkt.info());
	}

	return true;
}


bool Handle_S_MONSTERSPAWN(PacketSessionRef& session, Protocol::S_MONSTERSPAWN& pkt)
{

	if (AGameStateBase* GameState = Cast<AGameStateBase>(GWorld->GetGameState()))
	{
		if (UR3MultCharCreateionComponent* Spanwer = GameState->FindComponentByClass<UR3MultCharCreateionComponent>())
		{
			for (int32 i = 0; i < pkt.monsters_size(); i++)
			{
				const Protocol::ObjectInfo& monster = pkt.monsters(i);
				Spanwer->SpawnMultiplayerMonster(monster);
			}
		}
	}

	return true;
}