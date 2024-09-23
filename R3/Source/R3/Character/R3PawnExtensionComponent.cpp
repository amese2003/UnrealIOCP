#include "R3PawnExtensionComponent.h"
#include "R3/R3LogChannel.h"
#include "R3/R3GameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3PawnExtensionComponent)

const FName UR3PawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UR3PawnExtensionComponent::UR3PawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// �⺻������ Tick�� ��������
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UR3PawnExtensionComponent::SetPawnData(const UR3PawnData* InPawnData)
{
	// Pawn�� ���� Authority�� ���� ���, SetPawnData�� �������� ����
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	// PawnData ������Ʈ
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

	// �ùٸ� ���Ϳ� ��ϵƴ��� Ȯ��
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogR3, Error, TEXT("this component has been added to a BP whose base class in not a Pawn!"));
			return;
		}
	}

	// GameFrameworkComponentManager�� InitState ����� ���� ��� ����;
	// - ����� ��ӹ޾Ҵ� IGameFrameworkInitStateInterface ������ RegisterInitStateFeature()�� Ȱ��
	// - �ش� �Լ��� ������ ����..
	RegisterInitStateFeature();

	// ����� �Լ���
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
	UE_LOG(LogR3, Error, TEXT("Debug Manager"));
}

void UR3PawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// FeatureName�� Name_None�� ������, Actor�� ��ϵ� Feature Component�� InitState ���¸� �����ϰڴ�.
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// InitState_Spawned�� ���� ��ȯ.
	//	- TryToChangeIInitState�� �Ʒ��� ���� �����Ѵ�.
	//		1. CanChangeInitState�� ���� ��ȯ ���ɼ� ���� �Ǵ�.
	//		2. HandleChangeInitState�� ���� ���� ���� (Feature Component)
	//		3. BindOnActorInitStateChanged�� Bind�� Delegate�� ���ǿ� �°� ȣ���� ��
	//			- R3PawnExtensionComponent�� ���, ��� Actor�� Feature ���� ��ȭ�� ���� OnActorInitStateChanged()�� ȣ���.
	ensure(TryToChangeInitState(FR3GameplayTags::Get().InitState_Spawned));

	// �ش� �Լ��� �츮�� �������̵��Ѵ�.
	//	- �� �Լ��� ForceupdateInitState�� ���� �������� �������ָ� ��.
	//	- ���� ���� ������Ʈ ���� (���� CanChangeInitState�� HandleChangeInitState�� �������ش�.)
	CheckDefaultInitialization();
}

void UR3PawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// �ռ�, OnRegister�� RegisterInitStateFeature()�� ¦�� ��������.
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UR3PawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		// LyraPawnExtensionComponent�� �ٸ� Feature Component���� ���°� DataAvailable�� �����Ͽ�, Sync�� ���ߴ� ������ �־���(CanChangeInitState)
		// - �̸� �����ϰ� �ϱ� ����, OnActorInitStateChanged������ DataAvailable�� ���� ���������� CheckDefaultInitialization�� ȣ���Ͽ� ���¸� �����Ѵ�
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

	// InitState_Spawned �ʱ�ȭ
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn�� �� ���õǾ������� �ٷ� Spawn���� �Ѿ
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// �̹� PawnData�� ������ ������ ����̴�.
		if (!PawnData)
		{
			return false;
		}

		// LocallyControlled�� Pawn�� Controller�� ������ ����!
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
		// Actor�� ���ε� �� ��� feature���� DataAvailable ������ ��, DataInitialized�� �Ѿ.
		// - HaveAllFeaturesReachedInitState Ȯ��
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	// ���� ����(Linear) transition
	return false;
}

void UR3PawnExtensionComponent::CheckDefaultInitialization()
{
	// PawnExtensionComponent�� Feature Component���� �ʱ�ȭ�� �����ϴ� Component.
	//	- ����, Actor�� ���ε��� feature Component�鿡 ���� CheckDefaultInitialization�� ȣ�����ֵ��� �Ѵ�. (ForceUpdate ����?)
	//	- �� �޼��带 IGaameFrameworkInitStateInterface�� �����ϴµ�, CheckDefaultInitializationForImplementers�̴�.
	//  - ������ CheckDefaultInitializationForImplementers�� ����.
	CheckDefaultInitializationForImplementers();

	const FR3GameplayTags& InitTags = FR3GameplayTags::Get();

	// ����� ���� InitState�� ���� �Ѱ�����Ѵ�. (�̰� �˾Ƽ� ������ ���� �ѵ� ����?)
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// CanChangeInitState()�� HandleChangeInitState(), �׸��� ChangeFeatureInitState ȣ���� ���� OnActgorInitStateChanged Delegate ȣ����� �����Ѵ�.
	// �Ʒ� �ڵ带 ������ ����.
	//	- �ش� �ڵ���� ����, �츮�� ������ ������ �����ϴ�.
	//		- ��� ���¸� ������ �� ���� �ѹ� ������Ʈ �Ѵ�.
	//		- InitState�� ���� ��ȭ�� Linear(������)����..
	//			- ������Ʈ�� ���߸� ������ ��������� ���� �ǹ�(chaining)
	ContinueInitStateChain(StateChain);
}
