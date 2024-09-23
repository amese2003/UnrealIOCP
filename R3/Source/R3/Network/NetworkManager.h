#pragma once

#include "CoreMinimal.h"
#include "R3/R3.h"

class R3_API NetworkManager
{
public:
	NetworkManager();

	void ConnectToGameServer();
	void DisconnectFromGameServer();

	void HandleRecvPackets();
	void SendPacket(TSharedPtr<SendBuffer> SendBuffer);

	void SetSocket(FSocket* InSocket) { Socket = InSocket; }

	bool Connected() { return GameServerSession != nullptr; }


private:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<class PacketSession> GameServerSession;
};