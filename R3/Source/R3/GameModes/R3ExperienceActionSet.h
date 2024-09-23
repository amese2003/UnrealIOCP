#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "R3ExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
*	DataAsset로서, UGameFeatureAction을 카테고리화 시킬 때, 유용한 클래스
*/
UCLASS(BlueprintType)
class UR3ExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UR3ExperienceActionSet();

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif

	/**
	*	Member Variable
	*/
	UPROPERTY(EditAnywhere, Category = "Actions to Prefere")
		TArray<TObjectPtr<UGameFeatureAction>> Actions;
};