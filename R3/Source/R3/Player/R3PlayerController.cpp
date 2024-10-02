#include "R3PlayerController.h"
#include "R3PlayerState.h"

#include "R3/R3LogChannel.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3HeroComponent.h"
#include "R3/Camera/R3PlayerCameraManager.h"
#include "R3/Network/NetworkManager.h"

#include "R3/ClientPacketHandler.h"
#include "R3/Network/SendBuffer.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3PlayerController)

AR3PlayerController::AR3PlayerController(const FObjectInitializer& ObjectInitializer)
{
	PlayerCameraManagerClass = AR3PlayerCameraManager::StaticClass();
}

void AR3PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);


	UR3GameInstance* GameInstance = GetGameInstance<UR3GameInstance>();

	if (GameInstance && GameInstance->GetNetworkManager()->Connected())
	{
		UpdateSocketMovement(DeltaTime);
	}
}

void AR3PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
}



void AR3PlayerController::UpdateSocketMovement(float DeltaTime)
{
	Super::UpdateSocketMovement(DeltaTime);
}

AR3PlayerState* AR3PlayerController::GetR3PlayerState() const
{
	return CastChecked<AR3PlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

void AR3PlayerController::UpdateIdle()
{
	if (bMoveKeyPress)
	{
		SetMoveState(Protocol::MOVE_STATE_RUN);
		return;
	}
}

void AR3PlayerController::MoveToNextPos(float deltaTime)
{
	if (bMoveKeyPress == false)
	{
		CurrentState = Protocol::MOVE_STATE_IDLE;
		CheckUpdateFlag(deltaTime, true);

		UE_LOG(LogR3, Log, TEXT("Moving State End : %d"), CurrentState);
		return;
	}


	FVector destPos = GetPawn()->GetActorLocation() + InputDir * Speed;
	PosInfo->set_x(destPos.X);
	PosInfo->set_y(destPos.Y);
	PosInfo->set_z(destPos.Z);

	UE_LOG(LogR3, Log, TEXT("Moving State : %d"), CurrentState);
	CheckUpdateFlag(deltaTime);
}

void AR3PlayerController::CheckUpdateFlag(float deltaTime, bool ForceUpdate)
{
	MovePacketSendTimer -= deltaTime;

	if ((bUpdated && MovePacketSendTimer < 0.f) || ForceUpdate)
	{
		MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

		Protocol::C_MOVE MovePkt;

		Protocol::PosInfo* Info = MovePkt.mutable_info();
		MovePkt.set_creature_type(Protocol::CREATURE_TYPE_PLAYER);
		UE_LOG(LogR3, Log, TEXT("Send State : %d"), CurrentState);
		Info->CopyFrom(*PosInfo);
		Info->set_state(CurrentState);

		bUpdated = false;
		SEND_PACKET(MovePkt);
	}
}

void AR3PlayerController::SetDirInput(FVector Input)
{
	LastInputDir = InputDir;
	InputDir = Input;
	bUpdated = true;
}

FVector AR3PlayerController::GetDirInput()
{
	return InputDir;
}

void AR3PlayerController::SetMoveKeyPress(bool Input)
{
	bMoveKeyPress = Input;
}


//void AR3PlayerController::SendMovement(float DeltaTime)
//{
//	//if (!ControlPlayer)
//	//	return;
//
//	//// Send 판정
//	//bool ForceSendPacket = false;
//	//AR3Character* character = GetPawn<AR3Character>();
//
//	//if (!character)
//	//	return;
//
//	////UR3HeroComponent* heroComponent = character->GetComponentByClass<UR3HeroComponent>();
//
//	//FVector2d& DesiredInput = MovementInput;
//
//	//if (LastDesiredInput != DesiredInput)
//	//{
//	//	ForceSendPacket = true;
//	//	LastDesiredInput = DesiredInput;
//	//}
//
//	//// State 정보
//	//if (DesiredInput == FVector2D::Zero())
//	//	character->SetMoveState(Protocol::MOVE_STATE_IDLE);
//	//else
//	//	character->SetMoveState(Protocol::MOVE_STATE_RUN);
//
//	//MovePacketSendTimer -= DeltaTime;
//
//	//if (MovePacketSendTimer <= 0 || ForceSendPacket)
//	//{
//	//	MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;
//
//	//	Protocol::C_MOVE MovePkt;
//
//	//	// 현재 위치 정보
//	//	{
//	//		Protocol::PosInfo* Info = MovePkt.mutable_info();
//	//		Protocol::ObjectInfo* objInfo = character->GetPlayerInfo();
//
//	//		FVector CharacterLocation = character->GetActorLocation();
//	//		FRotator CharacterRotation = character->GetActorRotation();
//
//	//		Info->CopyFrom(objInfo->pos_info());
//
//
//	//		Info->set_x(CharacterLocation.X), Info->set_y(CharacterLocation.Y), Info->set_z(CharacterLocation.Z);
//	//		Info->set_rol(CharacterRotation.Roll), Info->set_pitch(CharacterRotation.Pitch), Info->set_yaw(CharacterRotation.Yaw);
//	//		Info->set_state(character->GetMoveState());
//	//	}
//
//
//	//	SEND_PACKET(MovePkt);
//	//}
//}

//void AR3PlayerController::UpdateSocketMovement(float DeltaTime)
//{
//
//}
