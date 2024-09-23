#include "CommonLocalPlayer.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonLocalPlayer)

UCommonLocalPlayer::UCommonLocalPlayer() 
// 여기 Super()로 넘기는 과정에서 FObjectInitializer::Get()를 통해 넘겼다
// - 이는 우리가 UCommonLocalPlayer()를 상속받는 LocalPlayer는 굳이 FObjectInitializer를 신경 쓸 필요가 없다
// - 만약 상속받는 LocalPlayer에서 FObjectInitializer를 통해 추가 AddObject를 정의한다면 이와 같은 구현은 수정할 필요가 있다.
	: Super(FObjectInitializer::Get())
{
}


FDelegateHandle UCommonLocalPlayer::CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate)
{
	APlayerController* PC = GetPlayerController(GetWorld());
	APlayerState* PlayerState = PC ? PC->PlayerState : nullptr;

	if (PlayerState)
	{
		// PlayerState가 실행되어 있으면 바로 호출
		Delegate.Execute(this, PlayerState);
	}

	// OnPlayerStateSet에 등록
	return OnPlayerStateSet.Add(Delegate);
}