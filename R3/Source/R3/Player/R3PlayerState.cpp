#include "R3PlayerState.h"
#include "R3/GameModes/R3ExperienceManagerComponent.h"
#include "R3/GameModes/R3GameState.h"
#include "R3/GameModes/R3GameMode.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3PlayerState)

AR3PlayerState::AR3PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AR3PlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	UR3ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UR3ExperienceManagerComponent>();
	check(ExperienceManagerComponent);



	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnR3ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AR3PlayerState::SetPawnData(const UR3PawnData* InPawnData)
{
	check(InPawnData);

	// PawnData�� �ι� �����Ǵ� ���� ������ �ʴ´�.
	check(!PawnData);

	PawnData = InPawnData;
}

void AR3PlayerState::OnExperienceLoaded(const UR3ExperienceDefinition* CurrentExperience)
{
	if (AR3GameMode* GameMode = GetWorld()->GetAuthGameMode<AR3GameMode>())
	{
		// ALyraGameMode���� GetPawnDataForContorller�� �����ؾ���
		//	- GetPawnDataForContoller���� �츮�� ���� PawnData�� �������� �ʾ����Ƿ�, ExperienceManagerComponent�� DefaultPawnData�� �����Ѵ�.
		const UR3PawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}
