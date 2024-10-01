#include "R3BaseController.h"
#include "R3PlayerState.h"

#include "R3/R3LogChannel.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3HeroComponent.h"
#include "R3/Camera/R3PlayerCameraManager.h"
#include "R3/Network/NetworkManager.h"
#include "DrawDebugHelpers.h"

#include "R3/ClientPacketHandler.h"
#include "R3/Network/SendBuffer.h"
#include "R3/R3LogChannel.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3BaseController)

PRAGMA_DISABLE_OPTIMIZATION
AR3BaseController::AR3BaseController(const FObjectInitializer& ObjectInitializer)
{
	PlayerCameraManagerClass = AR3PlayerCameraManager::StaticClass();
}

void AR3BaseController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);


	UR3GameInstance* GameInstance = GetGameInstance<UR3GameInstance>();

	if (GameInstance && GameInstance->GetNetworkManager()->Connected())
	{
		UpdateSocketMovement(DeltaTime);
	}
}

bool AR3BaseController::IsNetworking()
{
	UR3GameInstance* GameInstance = GetGameInstance<UR3GameInstance>();
	return GameInstance->GetNetworkManager()->Connected();
}

void AR3BaseController::UpdateSocketMovement(float DeltaTime)
{
	switch (CurrentState)
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

void AR3BaseController::UpdateIdle()
{
}

void AR3BaseController::UpdateMoving(float DeltaTime)
{
	FVector destPos = FVector(PosInfo->x(), PosInfo->y(), PosInfo->z());

	FVector curPos = GetPawn()->GetActorLocation();

	FVector2D rowColPos = FVector2D(destPos.X, destPos.Y);
	FVector2D rowColCur = FVector2D(curPos.X, curPos.Y);

	FVector2D moveDir = rowColPos - rowColCur;

	float dist = moveDir.SizeSquared();

	if (dist < Speed * DeltaTime)
	{
		GetPawn()->SetActorLocation(destPos); 
		MoveToNextPos(DeltaTime);
	}
	else
	{
		FVector nextPos = curPos + moveDir.Normalize() * Speed * DeltaTime;
		GetPawn()->SetActorLocation(destPos);
		CurrentState = Protocol::MOVE_STATE_RUN;
	}

	
}

void AR3BaseController::UpdateSkill()
{
}

void AR3BaseController::UpdateDead()
{
}

void AR3BaseController::InitializePosition()
{
	FVector ExpectPosition(PosInfo->x(), PosInfo->y(), PosInfo->z());
	APawn* myPawn = GetPawn();

	myPawn->SetActorLocation(ExpectPosition);
}

void AR3BaseController::ResetCharacterInfo()
{
	if (CharacterInfo != nullptr)
	{
		CharacterInfo->Clear();
		delete CharacterInfo;
	}

	CharacterInfo = new Protocol::ObjectInfo();
	PosInfo = new Protocol::PosInfo();
}

void AR3BaseController::SetPlayerInfo(const Protocol::ObjectInfo& Info)
{
	CharacterInfo->CopyFrom(Info);
	CurrentState = Info.pos_info().state();
	Speed = Info.speed();

	bUpdated = true;
}

void AR3BaseController::SetPosInfo(Protocol::PosInfo& Info)
{
	if (PosInfo != nullptr)
	{
		PosInfo->CopyFrom(Info);
		CurrentState = Info.state();
		UE_LOG(LogR3, Log, TEXT("set State : %d"), CurrentState);
	}
		
	bUpdated = true;
}

PRAGMA_ENABLE_OPTIMIZATION