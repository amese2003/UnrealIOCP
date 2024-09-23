#include "R3GameMode.h"
#include "R3ExperienceManagerComponent.h"
#include "R3ExperienceDefinition.h"
#include "R3GameState.h"
#include "Kismet/GameplayStatics.h"
#include "R3/Player/R3PlayerController.h"
#include "R3/Player/R3PlayerState.h"
#include "R3/Character/R3Character.h"
#include "R3/Character/R3PawnData.h"
#include "R3/Character/R3PawnExtensionComponent.h"
#include "R3/R3LogChannel.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3GameMode)

AR3GameMode::AR3GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	GameStateClass = AR3GameState::StaticClass();
	PlayerControllerClass = AR3PlayerController::StaticClass();
	PlayerStateClass = AR3PlayerState::StaticClass();
	DefaultPawnClass = AR3Character::StaticClass();
}

void AR3GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 아직 GameInstance를 통해, 초기화 작업이 진행되므로, 현 프레임에서는 Lyra의 Concpet인 Experience를 진행할 순 없다.
	// - 이를 처리할려면 한 프레임 뒤 이벤트를 받아 처리해야한다.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AR3GameMode::InitGameState()
{
	Super::InitGameState();

	UR3ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (!UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// OnExperienceLoaded 등록
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnR3ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

APawn* AR3GameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			// FindPawnExtensionComponent 구현
			if (UR3PawnExtensionComponent* PawnExtComp = UR3PawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UR3PawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}

	return nullptr;
}

void AR3GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

UClass* AR3GameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController를 활용하여, PawnData로부터 PawnClass를 유도하자.
	if (const UR3PawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AR3GameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	// 해당 함수에서는 우리가 로딩할 Experience에 대해 PrimaryAssetID를 생성하며, OnMatchAssignment를 넘겨준다.
	FPrimaryAssetId ExperienceId;


	// precedence order (highest wins)
	// - matchmaking assignment (if present) - x
	// - default experience

	UWorld* World = GetWorld();

	// 앞서, URL과 함께, ExtraArg로 남겼던 정보는 OptionsString에 있다
	if (ExperienceId.IsValid() == false && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// Experience의 Value를 가져와서, PrimaryAssetId를 생성해준다. 이 때, LyraCloneExperienceDefinition의 Class 이름을 사용한다
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UR3ExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}

	if (ExperienceId.IsValid() == false)
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("R3ExperienceDefinition"), FName("BP_R3DefaultExperience"));
	}

	// HandleMatchAssignmentIfNotExpectingOne과, OnMatchAssignmentGiven()은 아직 직관적이진 않은 것 같다..
	// - 나중에 어느정도 Lyra가 구현되면, 해당 함수를 더 이해할 수 있을것 같다
	OnMatchAssignmentGiven(ExperienceId);
}

void AR3GameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	check(ExperienceId.IsValid());

	UR3ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool AR3GameMode::IsExperienceLoaded() const
{
	check(GameState);
	UR3ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AR3GameMode::OnExperienceLoaded(const UR3ExperienceDefinition* CurrentExperience)
{
	RestartPlayerForController();
}

const UR3PawnData* AR3GameMode::GetPawnDataForController(const AController* InController) const
{
	// 게임 도중에 PawnData가 오버라이드가 되었을 경우, PawnData는 PlayerState에서 가져오게 됨
	if (InController)
	{
		if (const AR3PlayerState* clonePS = InController->GetPlayerState<AR3PlayerState>())
		{
			// GetPawnData 구현
			if (const UR3PawnData* PawnData = clonePS->GetPawnData<UR3PawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current Experience
	// 아직 PlayerState에 PawnData가 설정되어 있지 않은 경우, ExperienceManagerComponent의 CurrentExperience로부터 가져와서 설정
	check(GameState);
	UR3ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		// GetExperienceChecked 구현
		const UR3ExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	// 어떠한 케이스에서도 확인이 안되면 nullptr
	return nullptr;
}

void AR3GameMode::RestartPlayerForController()
{
	//GetWorld()->GetFirstPlayerController()

	// PlayerController를 순회하며...
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController가 Pawn을 Possess하지 않았다면, RestartPlayer를 통해 Pawn을 다시 Spawn한다...
		//	- 한번 OnPossess를 보도록 하자...
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}
