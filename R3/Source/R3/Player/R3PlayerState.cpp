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

	// PawnData가 두번 설정되는 것은 원하지 않는다.
	check(!PawnData);

	PawnData = InPawnData;
}

void AR3PlayerState::OnExperienceLoaded(const UR3ExperienceDefinition* CurrentExperience)
{
	if (AR3GameMode* GameMode = GetWorld()->GetAuthGameMode<AR3GameMode>())
	{
		// ALyraGameMode에서 GetPawnDataForContorller를 구현해야함
		//	- GetPawnDataForContoller에서 우리는 아직 PawnData를 설정하지 않았으므로, ExperienceManagerComponent의 DefaultPawnData로 설정한다.
		const UR3PawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}
