#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "R3UserFacingExperienceDefinition.generated.h"

/** Foward Declation */
class UCommonSession_HostSessionRequest;

UCLASS(BlueprintType)
class R3_API UR3UserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	*  Map�� �ε� �� Experience ��ȯ�� ����, MapID�� ExperienceID�� Ȱ���Ͽ�, HostSessionRequest ����
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		UCommonSession_HostSessionRequest* CreateHostingRequest() const;


	/**
	*  ��� ����
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
		FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "R3ExperienceDefinition"))
		FPrimaryAssetId ExperienceID;
};