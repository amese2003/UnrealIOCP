#pragma once

#include "CommonGameInstance.h"
#include "R3GameInstance.generated.h"

/** foward declaration */
class NetworkManager;

UCLASS()
class UR3GameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:
	UR3GameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	*	UGameInstance's interface
	*/
	virtual void Init() override;
	virtual void Shutdown() override;

	/**
	*  SocketNetwork
	*/
	TSharedPtr<NetworkManager> NetworkManagerInstance;
	TSharedPtr<NetworkManager> GetNetworkManager() { return NetworkManagerInstance; }

	void HandleRecvPackets();
};
