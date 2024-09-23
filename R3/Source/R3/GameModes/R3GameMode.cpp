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

	// ���� GameInstance�� ����, �ʱ�ȭ �۾��� ����ǹǷ�, �� �����ӿ����� Lyra�� Concpet�� Experience�� ������ �� ����.
	// - �̸� ó���ҷ��� �� ������ �� �̺�Ʈ�� �޾� ó���ؾ��Ѵ�.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AR3GameMode::InitGameState()
{
	Super::InitGameState();

	UR3ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (!UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// OnExperienceLoaded ���
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
			// FindPawnExtensionComponent ����
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
	// GetPawnDataForController�� Ȱ���Ͽ�, PawnData�κ��� PawnClass�� ��������.
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
	// �ش� �Լ������� �츮�� �ε��� Experience�� ���� PrimaryAssetID�� �����ϸ�, OnMatchAssignment�� �Ѱ��ش�.
	FPrimaryAssetId ExperienceId;


	// precedence order (highest wins)
	// - matchmaking assignment (if present) - x
	// - default experience

	UWorld* World = GetWorld();

	// �ռ�, URL�� �Բ�, ExtraArg�� ����� ������ OptionsString�� �ִ�
	if (ExperienceId.IsValid() == false && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// Experience�� Value�� �����ͼ�, PrimaryAssetId�� �������ش�. �� ��, LyraCloneExperienceDefinition�� Class �̸��� ����Ѵ�
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UR3ExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}

	if (ExperienceId.IsValid() == false)
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("R3ExperienceDefinition"), FName("BP_R3DefaultExperience"));
	}

	// HandleMatchAssignmentIfNotExpectingOne��, OnMatchAssignmentGiven()�� ���� ���������� ���� �� ����..
	// - ���߿� ������� Lyra�� �����Ǹ�, �ش� �Լ��� �� ������ �� ������ ����
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
	// ���� ���߿� PawnData�� �������̵尡 �Ǿ��� ���, PawnData�� PlayerState���� �������� ��
	if (InController)
	{
		if (const AR3PlayerState* clonePS = InController->GetPlayerState<AR3PlayerState>())
		{
			// GetPawnData ����
			if (const UR3PawnData* PawnData = clonePS->GetPawnData<UR3PawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current Experience
	// ���� PlayerState�� PawnData�� �����Ǿ� ���� ���� ���, ExperienceManagerComponent�� CurrentExperience�κ��� �����ͼ� ����
	check(GameState);
	UR3ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		// GetExperienceChecked ����
		const UR3ExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	// ��� ���̽������� Ȯ���� �ȵǸ� nullptr
	return nullptr;
}

void AR3GameMode::RestartPlayerForController()
{
	//GetWorld()->GetFirstPlayerController()

	// PlayerController�� ��ȸ�ϸ�...
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController�� Pawn�� Possess���� �ʾҴٸ�, RestartPlayer�� ���� Pawn�� �ٽ� Spawn�Ѵ�...
		//	- �ѹ� OnPossess�� ������ ����...
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}
