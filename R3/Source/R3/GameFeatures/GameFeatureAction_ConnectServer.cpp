#include "GameFeatureAction_ConnectServer.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Components/GameFrameworkComponentManager.h"
#include "R3/System/R3GameInstance.h"
#include "R3/Network/NetworkManager.h"
#include "Kismet/GameplayStatics.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_ConnectServer)

void UGameFeatureAction_ConnectServer::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_ConnectServer::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_ConnectServer::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	if (auto* R3GameInstance = Cast<UR3GameInstance>(GameInstance))
	{
		TSharedPtr<NetworkManager> NetManager = R3GameInstance->GetNetworkManager();
		NetManager->ConnectToGameServer();
		//CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	}
}
