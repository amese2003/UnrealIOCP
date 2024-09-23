#include "CommonGameInstance.h"
#include "CommonLocalPlayer.h"
#include "GameUIManagerSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonGameInstance)

UCommonGameInstance::UCommonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

int32 UCommonGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	// GameInstance에 관리하는 Player 컨테이너에 추가한다 (ReturnVal은 참고로 새로 추가한 index이다)
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		// PrimaryPlayer는 처음엔 캐싱하는 듯 하다 (무조건 처음 세팅되면 메인인가 Index>=0을 메인으로 잡긴하던데?)
		if (!PrimaryPlayer.IsValid())
		{
			PrimaryPlayer = NewPlayer;
		}

		// GameUIManagerSubsystem을 통해 NotifyPlayerAdded() 호출로 GameLayout을 추가한다
		//GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerAdded(Cast<UCommonLocalPlayer>(NewPlayer));
	}

	return ReturnVal;
}

bool UCommonGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	if (PrimaryPlayer == ExistingPlayer)
	{
		PrimaryPlayer.Reset();
	}

	//GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerDestroyed(Cast<UCommonLocalPlayer>(ExistingPlayer));
	return Super::RemoveLocalPlayer(ExistingPlayer);
}