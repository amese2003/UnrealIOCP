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
	*  Map의 로딩 및 Experience 전환을 위해, MapID와 ExperienceID를 활용하여, HostSessionRequest 생성
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		UCommonSession_HostSessionRequest* CreateHostingRequest() const;


	/**
	*  멤버 변수
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
		FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "R3ExperienceDefinition"))
		FPrimaryAssetId ExperienceID;
};