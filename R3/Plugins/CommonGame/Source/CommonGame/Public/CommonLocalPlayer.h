#pragma once

#include "Engine/LocalPlayer.h"
#include "CommonLocalPlayer.generated.h"

UCLASS()
class COMMONGAME_API UCommonLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	/** 주목할 점이 하나 있다. 우리는 여기서는 FObjectInitializer::Get()을 넘기지 않았다 */
	UCommonLocalPlayer();

	/** Player controller가 Local Player에 할당(assign) 되었을 경우 실행할 Delegate */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController)
		FPlayerControllerSetDelegate OnPlayerControllerSet;

	/** PlayerState가 LocalPlayer에 할당(assign) 되면 경위 실행할 Delegate */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UCommonLocalPlayer* LocalPlayer, APlayerState* PlayerState)
		FPlayerStateSetDelegate OnPlayerStateSet;

	FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
};