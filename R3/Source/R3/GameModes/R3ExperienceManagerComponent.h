#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "R3ExperienceManagerComponent.generated.h"


class UR3ExperienceDefinition;

enum class ER3ExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnR3ExperienceLoaded, const UR3ExperienceDefinition*);

/**
* R3ExperienceManagerComponent
* - 말 그대로 해당  Component는 game state를 owner로 가지면서, experience의 상대 정보를 가지고 있는 Component
* - 뿐만 아니라, manager라는 단어가 포함되어 있듯이, experience의 토글 상태 업데이트 및 이벤트를 관리
*/

UCLASS()
class UR3ExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UR3ExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	bool IsExperienceLoaded() { return (LoadState == ER3ExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }
	/**
	*
	*  아래의 OnExperienceLoaded에 바인딩하거나, 이미 Experience 로딩이 완료되었다면 바로 호출
	*/
	void CallOrRegister_OnExperienceLoaded(FOnR3ExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoaded();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UR3ExperienceDefinition* GetCurrentExperienceChecked() const;

	/**
	*  멤버 변수
	*
	*/

	/**
	*  해당 멤버 변수는 Lyra에서는 ReplicatedUsing으로 선언되있음.
	*  - 우리는 아직 Replication을 쓰지 않기에 최대한 네트워크 코드를 배제한다.
	*/
	UPROPERTY()
		TObjectPtr<const UR3ExperienceDefinition> CurrentExperience;

	// Experience의 로딩 상태를 모니터링
	ER3ExperienceLoadState LoadState = ER3ExperienceLoadState::Unloaded;

	// Experience 로딩이 완료된 이후, Broadcasting Delegate
	FOnR3ExperienceLoaded OnExperienceLoaded;


	/** 활성화 된 GameFeature Plugin들 */
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;
};