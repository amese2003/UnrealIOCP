// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "R3/R3.h"

/** foward Declaration*/
class UR3GameInstance;

/**
 * 
 */
class R3_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession(class FSocket* Socket);
	~PacketSession();

	void Run();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	void Disconnect();

public:
	class FSocket* Socket;

	TSharedPtr<class RecvWorker> RecvWorkerThread;
	TSharedPtr<class SendWorker> SendWorkerThread;

	TQueue<TArray<uint8>> RecvPacketQueue;
	TQueue<SendBufferRef> SendPacketQueue;

public:
	TObjectPtr<UR3GameInstance> CurrentGameInstance;
};
