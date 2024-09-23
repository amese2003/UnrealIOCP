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

	/** ExperienceActionSet�� UGameFeatureAction�� Set�̸�, Gameplay �뵵�� �°� �з��� �������� ���ȴ�. */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TArray<TObjectPtr<UR3ExperienceActionSet>> ActionsSets;

	/** �Ϲ����� GameFeatureAction���μ� �߰� */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
		TArray<TObjectPtr<UGameFeatureAction>> Actions;
};