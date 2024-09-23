#include "R3PawnExtensionComponent.h"
#include "R3/R3LogChannel.h"
#include "R3/R3GameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3PawnExtensionComponent)

const FName UR3PawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UR3PawnExtensionComponent::UR3PawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 기본적으로 Tick을 꺼버리자
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UR3PawnExtensionComponent::SetPawnData(const UR3PawnData* InPawnData)
{
	// Pawn에 대해 Authority가 없을 경우, SetPawnData는 진행하지 않음
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	// PawnData 업데이트
	PawnData = InPawnData;
}

void UR3PawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

#include "Components/GameFrameworkComponentManager.h"

void UR3PawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	// 올바른 액터에 등록됐는지 확인
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogR3, Error, TEXT("this component has been added to a BP whose base class in not a Pawn!"));
			return;
		}
	}

	// GameFrameworkComponentManager에 InitState 사용을 위해 등록 진행;
	// - 등록은 상속받았던 IGameFrameworkInitStateInterface 에서는 RegisterInitStateFeature()를 활용
	// - 해당 함수를 간단히 보자..
	RegisterInitStateFeature();

	// 디버깅 함수ㅜ
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
	UE_LOG(LogR3, Error, TEXT("Debug Manager"));
}

void UR3PawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// FeatureName에 Name_None을 넣으면, Actor에 등록된 Feature Component의 InitState 상태를 관찰하겠다.
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// InitState_Spawned로 상태 변환.
	//	- TryToChangeIInitState는 아래와 같이 진행한다.
	//		1. CanChangeInitState로 상태 변환 가능성 유무 판단.
	//		2. HandleChangeInitState로 내부 상태 변경 (Feature Component)
	//		3. BindOnActorInitStateChanged로 Bind된 Delegate를 조건에 맞게 호출해 줌
	//			- R3PawnExtensionComponent의 경우, 모든 Actor의 Feature 상태 변화에 대해 OnActorInitStateChanged()가 호출됨.
	ensure(TryToChangeInitState(FR3GameplayTags::Get().InitState_Spawned));

	// 해당 함수는 우리가 오버라이딩한다.
	//	- 이 함수를 ForceupdateInitState와 같은 느낌으로 이해해주면 됨.
	//	- 현재 강제 업데이트 진행 (물론 CanChangeInitState와 HandleChangeInitState를 진행해준다.)
	CheckDefaultInitialization();
}

void UR3PawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 앞서, OnRegister의 RegisterInitStateFeature()의 짝을 맞춰주자.
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UR3PawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		// LyraPawnExtensionComponent는 다른 Feature Component등의 상태가 DataAvailable를 관찰하여, Sync를 맞추는 구간이 있었다(CanChangeInitState)
		// - 이를 가능하게 하기 위해, OnActorInitStateChanged에서는 DataAvailable에 대해 지속적으로 CheckDefaultInitialization를 호출하여 상태를 복원한다
		const FR3GameplayTags& InitTags = FR3GameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UR3PawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FR3GameplayTags& InitTags = FR3GameplayTags::Get();

	// InitState_Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn이 잘 세팅되어있으면 바로 Spawn으로 넘어감
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// 이미 PawnData를 누군가 설정한 모양이다.
		if (!PawnData)
		{
			return false;
		}

		// LocallyControlled와 Pawn의 Controller가 없으면 에러!
		const bool HasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (HasAuthority || bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// Actor에 바인드 된 모든 feature들의 DataAvailable 상태일 때, DataInitialized로 넘어감.
		// - HaveAllFeaturesReachedInitState 확인
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	// 위의 선형(Linear) transition
	return false;
}

void UR3PawnExtensionComponent::CheckDefaultInitialization()
{
	// PawnExtensionComponent는 Feature Component들의 초기화를 관장하는 Component.
	//	- 따라서, Actor에 바인딩된 feature Component들에 대해 CheckDefaultInitialization를 호출해주도록 한다. (ForceUpdate 느낌?)
	//	- 이 메서드를 IGaameFrameworkInitStateInterface가 제공하는데, CheckDefaultInitializationForImplementers이다.
	//  - 간단히 CheckDefaultInitializationForImplementers를 보자.
	CheckDefaultInitializationForImplementers();

	const FR3GameplayTags& InitTags = FR3GameplayTags::Get();

	// 사용자 정의 InitState를 직접 넘겨줘야한다. (이걸 알아서 시켜줄 법도 한데 굳이?)
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// CanChangeInitState()의 HandleChangeInitState(), 그리고 ChangeFeatureInitState 호출을 통한 OnActgorInitStateChanged Delegate 호출까지 진행한다.
	// 아래 코드를 간단히 보자.
	//	- 해당 코드까지 보면, 우리는 간단히 짐작이 가능하다.
	//		- 계속 상태를 가능할 때 까지 한번 업데이트 한다.
	//		- InitState에 대한 변화는 Linear(선형적)있음..
	//			- 업데이트가 멈추면 누군가 시작해줘야 함을 의미(chaining)
	ContinueInitStateChain(StateChain);
}
