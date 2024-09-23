// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketSession.h"
#include "NetworkWorker.h"
#include "R3/ClientPacketHandler.h"


PacketSession::PacketSession(class FSocket* Socket) : Socket(Socket)
{
	ClientPacketHandler::Init();
}

PacketSession::~PacketSession()
{
	Disconnect();
}

void PacketSession::HandleRecvPackets()
{
	while (true)
	{
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
			break;

		PacketSessionRef ThisRef = AsShared();
		ClientPacketHandler::HandlePacket(ThisRef, Packet.GetData(), Packet.Num());
		
	}
}

void PacketSession::SendPacket(SendBufferRef SendBuffer)
{
	SendPacketQueue.Enqueue(SendBuffer);
}

void PacketSession::Run()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}

void PacketSession::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread = nullptr;
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread = nullptr;
	}
}
