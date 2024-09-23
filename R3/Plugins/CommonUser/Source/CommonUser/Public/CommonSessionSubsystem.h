#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubsystem.generated.h"

/** Foward Declation */
class APlayerController;

/**
*  UCommonSession_HostSessionRequest를 만들어 CommonSessionSubsystem 전달하면, CommonSessionSubsystem에서 MapId와 CmdArgs로 최종적인 CmdArgs를 만들어 맵 로딩 진행
*/
UCLASS(BlueprintType)
class COMMONUSER_API UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()

public:
	/** MapID -> Text 변환 */
	FString GetMapName() const;

	/** ServerTravel에 전달할 최종 URL 생성 */
	FString ConstructTravelURL() const;

	/** 준비할 MapID (맵 경로) */
	UPROPERTY(BlueprintReadWrite, Category = Session)
		FPrimaryAssetId MapID;

	/** 전달할 CmdArgs (Experience 이름을 전달함) -> 곧 UserFacingExperienceDefinition에서 보게 될 것 */
	UPROPERTY(BlueprintReadWrite, Category = Session)
		TMap<FString, FString> ExtraArgs;
};

/**
*	UCommonSessionSubsystem은 LyraClone에서 사용되어야 하기 때문에, Module Export를 해줘야 하고, 그래서 COMMONUSER_API를 추가해야한다..
*		- {ModuleName}_API의 추가는 *** 다른 모듈에서 사용할 경우 추가해주면 된다
*/

UCLASS()
class COMMONUSER_API UCommonSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCommonSessionSubsystem() {};

	UFUNCTION(BlueprintCallable, Category = Session)
		void HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request);

	/**
	*	Member variables
	*/
	/** PendingTravelURL은 혼칙 맵의 경로라고 생각해면 된다 */
	FString PendingTravelURL;

};