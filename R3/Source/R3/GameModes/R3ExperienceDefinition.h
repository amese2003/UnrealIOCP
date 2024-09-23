#pragma once

#include "Engine/DataAsset.h"
#include "R3ExperienceDefinition.generated.h"

class UR3PawnData;
class UGameFeatureAction;
class UR3ExperienceActionSet;

UCLASS(BlueprintType)
class UR3ExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UR3ExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TObjectPtr<UR3PawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TArray<FString> GameFeaturesToEnable;

	/** ExperienceActionSet은 UGameFeatureAction의 Set이며, Gameplay 용도에 맞게 분류의 목적으로 사용된다. */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TArray<TObjectPtr<UR3ExperienceActionSet>> ActionsSets;

	/** 일반적인 GameFeatureAction으로서 추가 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
		TArray<TObjectPtr<UGameFeatureAction>> Actions;
};