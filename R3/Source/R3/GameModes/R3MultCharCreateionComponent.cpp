#include "R3MultCharCreateionComponent.h"
#include "R3GameMode.h"
#include "R3/Player/R3PlayerController.h"
#include "R3/Player/R3SocketPlayerController.h"
#include "R3/Character/R3PawnExtensionComponent.h"
#include "R3/GameModes/R3ExperienceManagerComponent.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3Spawner.h"
#include "kismet/GameplayStatics.h"
#include "R3/Player/R3MonsterController.h"
#include "R3/R3LogChannel.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3MultCharCreateionComponent)

UR3MultCharCreateionComponent::UR3MultCharCreateionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR3MultCharCreateionComponent::BeginPlay()
{
	Super::BeginPlay();

	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	UR3ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnR3ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}


void UR3MultCharCreateionComponent::SpawnMultiplayerCharacter(const Protocol::ObjectInfo& Info)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = GetComponentLevel();
	SpawnInfo.ObjectFlags |= RF_Transient;
	AR3SocketPlayerController* NewController = GetWorld()->SpawnActor<AR3SocketPlayerController>(MultiControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
	

	if (NewController != nullptr)
	{
		AR3GameMode* GameMode = GetGameMode<AR3GameMode>();
		check(GameMode);

		GameMode->DispatchPostLogin(NewController);
		GameMode->RestartPlayer(NewController);

		if (NewController->GetPawn() != nullptr)
		{
			if (UR3PawnExtensionComponent* PawnExtComponent = NewController->GetPawn()->FindComponentByClass<UR3PawnExtensionComponent>())
			{
				PawnExtComponent->CheckDefaultInitialization();
			}

			NewController->ResetCharacterInfo();
			NewController->SetPlayerInfo(Info);
			NewController->InitializePosition();

			SpawnedBotList.Add(Info.object_id(), NewController);
		}
	}
}

void UR3MultCharCreateionComponent::SpawnMultiplayerMonster(const Protocol::ObjectInfo& Info)
{
	if (SpawnMonsterList.Contains(Info.object_id()) == false)
	{
		FVector SpawnLocation(Info.pos_info().x(), Info.pos_info().y(), Info.pos_info().z());
		FRotator SpawnRotator(Info.pos_info().pitch(), Info.pos_info().yaw(), Info.pos_info().rol());

		float speed = Info.speed();

		GetMonsterSpawner()->SpawnMonster(Info.object_id(), Info.creature_id(), SpawnLocation, SpawnRotator, speed);
	}
}

void UR3MultCharCreateionComponent::RegisterMonster(int object_id, AActor* MonsterActor)
{
	if (MonsterActor != nullptr)
	{
		if (AR3Character* Monster = Cast<AR3Character>(MonsterActor))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.OverrideLevel = GetComponentLevel();
			SpawnInfo.ObjectFlags |= RF_Transient;
			AR3MonsterController* NewController = GetWorld()->SpawnActor<AR3MonsterController>(MultiControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);

			NewController->Possess(Monster);

			if (NewController->GetPawn())
				SpawnMonsterList.Add(object_id, NewController);
		}
	}
}

void UR3MultCharCreateionComponent::RestartPlayer(const Protocol::ObjectInfo& Info)
{
	AR3GameMode* currentGameMode = GetGameMode<AR3GameMode>();

	AR3PlayerController* PC = Cast<AR3PlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC && PC->GetPawn() == nullptr)
	{
		if (currentGameMode->PlayerCanRestart(PC))
		{
			currentGameMode->RestartPlayer(PC);

			PC->ResetCharacterInfo();
			PC->SetPlayerInfo(Info);

			Protocol::PosInfo pos = Info.pos_info();
			PC->SetPosInfo(pos);
			PC->InitializePosition();

			SpawnedBotList.Add(Info.object_id(), PC);
		}
	}
}

void UR3MultCharCreateionComponent::UpdateCharacterMovement(const Protocol::S_MOVE& Info)
{
	Protocol::PosInfo pos = Info.info();

	if (Info.creature_type() == Protocol::CREATURE_TYPE_PLAYER)
	{
		if (!SpawnedBotList.Contains(Info.info().object_id()))
			return;

		AR3SocketPlayerController* SPC = Cast<AR3SocketPlayerController>(SpawnedBotList[Info.info().object_id()]);

		UE_LOG(LogR3, Log, TEXT("player_id : %d, player_state : %d"), pos.object_id(), pos.state());

		

		if (SPC)
		{
			FVector test = FVector(pos.x(), pos.y(), pos.z());

			DrawDebugSphere(GetWorld(), SPC->GetPawn()->GetActorLocation(), 200, 26, FColor(181, 0, 0), true, -1, 0, 2);
			DrawDebugSphere(GetWorld(), test, 200, 26, FColor(181, 0, 0), true, -1, 0, 2);

			SPC->SetPosInfo(pos);
			return;
		}

		AR3PlayerController* PC = Cast<AR3PlayerController>(SpawnedBotList[Info.info().object_id()]);

		if (PC)
		{
			PC->SetPosInfo(pos);
		}
	}

	if (Info.creature_type() == Protocol::CREATURE_TYPE_MONSTER)
	{
		if (!SpawnMonsterList.Contains(Info.info().object_id()))
			return;


	}

	

	/*AR3PlayerController* PC = Cast<AR3PlayerController>(SpawnedBotList[Info.object_id()]);

	if (PC)
	{
		if (AR3Character* character = PC->GetPawn<AR3Character>())
		{
			character->SetPosInfo(Info);
		}
	}


	AR3SocketPlayerController* SPC = Cast<AR3SocketPlayerController>(SpawnedBotList[Info.object_id()]);
	
	if (SPC)
	{
		if (AR3Character* character = SPC->GetPawn<AR3Character>())
		{
			character->SetPosInfo(Info);
		}
		
	}*/
}

void UR3MultCharCreateionComponent::OnExperienceLoaded(const UR3ExperienceDefinition* Experience)
{
}

TObjectPtr<AR3Spawner> UR3MultCharCreateionComponent::GetMonsterSpawner()
{
	if (MonsterSpawner == nullptr)
	{
		if (AActor* Spawner = UGameplayStatics::GetActorOfClass(GetWorld(), AR3Spawner::StaticClass()))
		{
			MonsterSpawner = Cast<AR3Spawner>(Spawner);
		}
	}

	return MonsterSpawner;
}
