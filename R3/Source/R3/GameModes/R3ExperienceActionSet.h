#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "R3ExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
*	DataAsset�μ�, UGameFeatureAction�� ī�װ�ȭ ��ų ��, ������ Ŭ����
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