#include "R3NetworkExecutionComponent.h"
#include "R3GameMode.h"
#include "R3/System/R3GameInstance.h"
#include "R3/Network/NetworkManager.h"
#include "R3/Player/R3SocketPlayerController.h"
#include "R3/Character/R3PawnExtensionComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3NetworkExecutionComponent)

UR3NetworkExecutionComponent::UR3NetworkExecutionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UR3NetworkExecutionComponent::BeginPlay()
{
	Super::BeginPlay();

	UR3GameInstance* GameInstance = GetGameInstance<UR3GameInstance>();

	if (GameInstance)
	{
		TSharedPtr<NetworkManager> CurrentNetwork = GameInstance->GetNetworkManager();

		if (CurrentNetwork->Connected())
		{
			NetworkManagerInstance = CurrentNetwork;
		}
	}
}

void UR3NetworkExecutionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (NetworkManagerInstance)
	{
		NetworkManagerInstance->HandleRecvPackets();
	}
}
