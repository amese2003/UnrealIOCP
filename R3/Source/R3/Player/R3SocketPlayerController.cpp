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

PRAGMA_DISABLE_OPTIMIZATION
void AR3SocketPlayerController::UpdateSocketMovement(float DeltaTime)
{
	if (PosInfo == nullptr)
		return;

	SyncPosition(DeltaTime);

	switch (PosInfo->state())
	{
	case Protocol::MOVE_STATE_IDLE:
		UpdateIdle();
		break;

	case Protocol::MOVE_STATE_RUN:
		UpdateMoving(DeltaTime);
		break;

	case Protocol::MOVE_STATE_SKILL:
		UpdateSkill();
		break;

	case Protocol::MOVE_STATE_DEAD:
		UpdateDead();
		break;
	}
}

void AR3SocketPlayerController::UpdateMoving(float deltaTime)
{
	/*FVector destPos = FVector(PosInfo->x(), PosInfo->y(), PosInfo->z());

	FVector curPos = GetPawn()->GetActorLocation();

	UE_LOG(LogR3, Log, TEXT("Update Object_id : %d"), CharacterInfo->object_id());

	FVector2D rowColPos = FVector2D(destPos.X, destPos.Y);
	FVector2D rowColCur = FVector2D(curPos.X, curPos.Y);

	FVector2D moveDir = rowColPos - rowColCur;

	float dist = moveDir.SizeSquared();

	FVector Res = FVector(moveDir.X, moveDir.Y, 0.f);
	FVector nextPos = curPos + Res * deltaTime;


	if (dist < Speed * deltaTime)
	{
		GetPawn()->SetActorLocation(destPos);
	}
	else
	{
		GetPawn()->SetActorLocation(nextPos);
	}*/


	//Super::UpdateMoving(deltaTime);
	
	//if (dist < Speed * DeltaTime)
	//{
	//	GetPawn()->SetActorLocation(destPos);
	//	MoveToNextPos(DeltaTime);
	//}
	//else
	//{
	//	/*FVector Res = FVector(moveDir.X, moveDir.Y, 0.f);
	//	FVector nextPos = curPos + Res * DeltaTime;*/
	//	GetPawn()->SetActorLocation(destPos);
	//	CurrentState = Protocol::MOVE_STATE_RUN;
	//}
}



PRAGMA_ENABLE_OPTIMIZATION