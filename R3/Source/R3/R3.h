// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define USING_SHARED_PTR(name)	using name##Ref = TSharedPtr<class name>;

USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(SendBuffer);

#include "ClientPacketHandler.h"
#include "System/R3GameInstance.h"
#include "Network/SendBuffer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#define SEND_PACKET(pkt)																			\
SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);								\
Cast<UR3GameInstance>(GWorld->GetGameInstance())->NetworkManagerInstance->SendPacket(sendBuffer);	