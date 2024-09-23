#include "CommonPlayerController.h"
#include "CommonLocalPlayer.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonPlayerController)

ACommonPlayerController::ACommonPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ACommonPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	// PlayerController가 LocalPlayer에 넣으면 활성화되는 이벤트가 ReceivedPlayer()이다
	if (UCommonLocalPlayer* LocalPlayer = Cast<UCommonLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);

		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}

	}
}