#include "R3PlayerController.h"
#include "R3PlayerState.h"
#include "R3/R3.h"
#include "R3/R3LogChannel.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3HeroComponent.h"
#include "R3/Camera/R3PlayerCameraManager.h"
#include "R3/Network/NetworkManager.h"
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
		SendMovement(DeltaTime);
		UpdateSocketMovement(DeltaTime);
	}
}

void AR3PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

AR3PlayerState* AR3PlayerController::GetR3PlayerState() const
{
	return CastChecked<AR3PlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}


void AR3PlayerController::SendMovement(float DeltaTime)
{
	if (!ControlPlayer)
		return;

	// Send 판정
	bool ForceSendPacket = false;
	AR3Character* character = GetPawn<AR3Character>();

	if (!character)
		return;

	//UR3HeroComponent* heroComponent = character->GetComponentByClass<UR3HeroComponent>();

	FVector2d& DesiredInput = MovementInput;

	if (LastDesiredInput != DesiredInput)
	{
		ForceSendPacket = true;
		LastDesiredInput = DesiredInput;
	}

	// State 정보
	if (DesiredInput == FVector2D::Zero())
		character->SetMoveState(Protocol::MOVE_STATE_IDLE);
	else
		character->SetMoveState(Protocol::MOVE_STATE_RUN);

	MovePacketSendTimer -= DeltaTime;

	if (MovePacketSendTimer <= 0 || ForceSendPacket)
	{
		MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

		Protocol::C_MOVE MovePkt;

		// 현재 위치 정보
		{
			Protocol::PosInfo* Info = MovePkt.mutable_info();
			Protocol::ObjectInfo* objInfo = character->GetPlayerInfo();

			Info->CopyFrom(objInfo->pos_info());
			Info->set_yaw(YawValue);
			Info->set_state(character->GetMoveState());
		}

		SEND_PACKET(MovePkt);
	}
}

void AR3PlayerController::UpdateSocketMovement(float DeltaTime)
{
	AR3Character* character = GetPawn<AR3Character>();

	if (!character)
		return;

	character->UpdatePosInfo();

	if (ControlPlayer)
		return;

	UR3HeroComponent* heroComponent = character->GetComponentByClass<UR3HeroComponent>();
	heroComponent->Input_SocketMove(LastDesiredInput);

	UE_LOG(LogR3, Error, TEXT("Object id : %d. x : %f"), character->GetPosInfo()->object_id(), character->GetPosInfo()->x());
	
}
