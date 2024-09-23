#include "R3GameInstance.h"
#include "R3/R3GameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "R3/Network/NetworkManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3GameInstance)

UR3GameInstance::UR3GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR3GameInstance::Init()
{
	Super::Init();

	// 앞서 정의한	InitState의 GameplayTags등록
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		const FR3GameplayTags& GameplayTags = FR3GameplayTags::Get();

		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}

	// 네트워크 소켓을 위한 매니저 등록
	NetworkManagerInstance = MakeShared<NetworkManager>();
}

void UR3GameInstance::Shutdown()
{
	NetworkManagerInstance->DisconnectFromGameServer();

	Super::Shutdown();
}