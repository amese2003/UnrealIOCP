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
* - �� �״�� �ش�  Component�� game state�� owner�� �����鼭, experience�� ��� ������ ������ �ִ� Component
* - �Ӹ� �ƴ϶�, manager��� �ܾ ���ԵǾ� �ֵ���, experience�� ��� ���� ������Ʈ �� �̺�Ʈ�� ����
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
	*  �Ʒ��� OnExperienceLoaded�� ���ε��ϰų�, �̹� Experience �ε��� �Ϸ�Ǿ��ٸ� �ٷ� ȣ��
	*/
	void CallOrRegister_OnExperienceLoaded(FOnR3ExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoaded();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UR3ExperienceDefinition* GetCurrentExperienceChecked() const;

	/**
	*  ��� ����
	*
	*/

	/**
	*  �ش� ��� ������ Lyra������ ReplicatedUsing���� ���������.
	*  - �츮�� ���� Replication�� ���� �ʱ⿡ �ִ��� ��Ʈ��ũ �ڵ带 �����Ѵ�.
	*/
	UPROPERTY()
		TObjectPtr<const UR3ExperienceDefinition> CurrentExperience;

	// Experience�� �ε� ���¸� ����͸�
	ER3ExperienceLoadState LoadState = ER3ExperienceLoadState::Unloaded;

	// Experience �ε��� �Ϸ�� ����, Broadcasting Delegate
	FOnR3ExperienceLoaded OnExperienceLoaded;


	/** Ȱ��ȭ �� GameFeature Plugin�� */
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;
};