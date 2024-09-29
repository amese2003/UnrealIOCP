#include "R3BaseController.h"
#include "R3PlayerState.h"

#include "R3/R3LogChannel.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3HeroComponent.h"
#include "R3/Camera/R3PlayerCameraManager.h"
#include "R3/Network/NetworkManager.h"

#include "R3/ClientPacketHandler.h"
#include "R3/Network/SendBuffer.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3BaseController)

AR3BaseController::AR3BaseController(const FObjectInitializer& ObjectInitializer)
{
	PlayerCameraManagerClass = AR3PlayerCameraManager::StaticClass();
}

//void AR3BaseController::BeginPlay()
//{
//	Super::BeginPlay();
//
//	CurrentState = Protocol::MOVE_STATE_IDLE;
//}
//
//void AR3BaseController::PlayerTick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	UpdateSocketMovement(DeltaTime);
//}
//
//void AR3BaseController::ReceivedPlayer()
//{
//	Super::ReceivedPlayer();
//}

//void AR3BaseController::UpdateSocketMovement(float DeltaTime)
//{
//	switch (CurrentState)
//	{
//	case Protocol::MOVE_STATE_IDLE:
//		UpdateIdle();
//		break;
//
//	case Protocol::MOVE_STATE_RUN:
//		UpdateMoving(DeltaTime);
//		break;
//
//	case Protocol::MOVE_STATE_SKILL:
//		break;
//
//	case Protocol::MOVE_STATE_DEAD:
//		break;
//	}
//}
//
//void AR3BaseController::UpdateIdle()
//{
//}
//
//void AR3BaseController::UpdateMoving(float DeltaTime)
//{
//	AR3Character* character = Cast<AR3Character>(GetPawn());
//
//	FVector destPos(PosInfo->x(), PosInfo->y(), PosInfo->z());;
//
//	FVector currentLocation = character->GetActorLocation();
//	FVector moveDir = destPos - currentLocation;
//
//	float dist = moveDir.Size();
//
//	if (dist < Speed * DeltaTime)
//	{
//		character->SetActorLocation(destPos);
//		MoveToNextPos();
//	}
//	else
//	{
//		FVector nextLocation = currentLocation + moveDir.GetSafeNormal() * Speed * DeltaTime;
//		CurrentState = Protocol::MOVE_STATE_RUN;
//		character->SetActorLocation(nextLocation);
//	}
//}
//
//void AR3BaseController::UpdateSkill()
//{
//}
//
//void AR3BaseController::UpdateDead()
//{
//}

//void AR3BaseController::MoveToNextPos()
//{
//}
//
//void AR3BaseController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
//{
//	Super::PostProcessInput(DeltaTime, bGamePaused);
//}

void AR3BaseController::InitializePosition()
{
	FVector ExpectPosition(CharacterInfo->pos_info().x(), CharacterInfo->pos_info().y(), CharacterInfo->pos_info().z());
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
	Speed = Info.speed();
}

void AR3BaseController::SetPosInfo(Protocol::PosInfo& Info)
{
	if (PosInfo != nullptr)
	{
		PosInfo->CopyFrom(Info);
		CurrentState = Info.state();
	}
		
	bUpdated = true;
}
