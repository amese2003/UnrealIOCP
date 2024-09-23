#include "R3SocketPlayerController.h"
#include "R3PlayerState.h"
#include "R3/R3.h"
#include "R3/R3LogChannel.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3HeroComponent.h"
#include "R3/Camera/R3PlayerCameraManager.h"
#include "R3/Network/NetworkManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3SocketPlayerController)

AR3SocketPlayerController::AR3SocketPlayerController(const FObjectInitializer& ObjectInitializer)
{
}

void AR3SocketPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdateSocketMovement(DeltaTime);
}

void AR3SocketPlayerController::UpdateSocketMovement(float DeltaTime)
{
	AR3Character* character = GetPawn<AR3Character>();

	if (!character)
		return;

	character->UpdatePosInfo();

	FVector InputDir(character->GetPosInfo()->x(), character->GetPosInfo()->y(), character->GetPosInfo()->z());
	InputDir = InputDir - character->GetActorLocation();
	InputDir = InputDir.GetSafeNormal2D();
	FVector2D desire(InputDir.X, InputDir.Y);


	UR3HeroComponent* heroComponent = character->GetComponentByClass<UR3HeroComponent>();
	heroComponent->Input_SocketMove(desire);

	UE_LOG(LogR3, Error, TEXT("Object id : %d"), character->GetPosInfo()->object_id());
	UE_LOG(LogR3, Error, TEXT("x: %f, y: %f"), character->GetPosInfo()->x(), character->GetPosInfo()->y());
	UE_LOG(LogR3, Error, TEXT("Input x: %f, Input y: %f"), desire.X, desire.Y);

}
