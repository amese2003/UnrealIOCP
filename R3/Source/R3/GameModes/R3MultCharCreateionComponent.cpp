#include "R3MultCharCreateionComponent.h"
#include "R3GameMode.h"
#include "R3/Player/R3PlayerController.h"
#include "R3/Player/R3SocketPlayerController.h"
#include "R3/Character/R3PawnExtensionComponent.h"
#include "R3/GameModes/R3ExperienceManagerComponent.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3Spawner.h"
#include "kismet/GameplayStatics.h"
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

			AR3Character* character = NewController->GetPawn<AR3Character>();
			character->ResetCharacterInfo();
			character->SetPlayerInfo(Info);
			character->InitializePosition();
		}

		SpawnedBotList.Add(Info.object_id(), NewController);
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
			SpawnMonsterList.Add(object_id, Monster);
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

			AR3Character* character = PC->GetPawn<AR3Character>();
			character->ResetCharacterInfo();
			character->SetPlayerInfo(Info);
			character->InitializePosition();

			/*PC->SetControlPlayer();*/

			SpawnedBotList.Add(Info.object_id(), PC);
		}
	}
}

void UR3MultCharCreateionComponent::UpdateCharacterMovement(const Protocol::PosInfo& Info)
{
	if (!SpawnedBotList.Contains(Info.object_id()))
		return;

	AR3PlayerController* PC = Cast<AR3PlayerController>(SpawnedBotList[Info.object_id()]);

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
		
	}
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
