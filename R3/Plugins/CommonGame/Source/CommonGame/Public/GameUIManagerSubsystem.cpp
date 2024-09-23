#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameUIManagerSubsystem)

UGameUIManagerSubsystem::UGameUIManagerSubsystem()
	: Super()
{
}

void UGameUIManagerSubsystem::SwitchToPolicy(UGameUIPolicy* InPolicy)
{
	if (CurrentPolicy != InPolicy)
	{
		CurrentPolicy = InPolicy;
	}
}

void UGameUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// CurrentPolicy가 설정되어 있지않고, DefaultUIPolicyClass가 제대로 설정되어 있을 경우 (우리는 LyraCloneUIPolicy)
	if (!CurrentPolicy && !DefaultUIPolicyClass.IsNull())
	{
		// UIPolicyClass는 BP예정이기에 로딩해야함
		TSubclassOf<UGameUIPolicy> PolicyClass = DefaultUIPolicyClass.LoadSynchronous();

		// UIPolicyClass를 통해 NewObject로 인스턴싱해서 CurrentPolicy에 설정
		SwitchToPolicy(NewObject<UGameUIPolicy>(this, PolicyClass));
	}

}

void UGameUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	SwitchToPolicy(nullptr);

}

bool UGameUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 우선 DedicateServer의 경우, GameUIManagerSubsystem을 활성화 하지 않음
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		// 만약 어따한 Subsystem도 UGameUIManagerSubsystem을 상속받지 않다면, 해당 GameInstanceSubsystem은 활성화 안됨
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		return ChildClasses.Num() == 0;
	}

	return false;
}


void UGameUIManagerSubsystem::NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer) && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerAdded(LocalPlayer);
	}
}

void UGameUIManagerSubsystem::NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerRemoved(LocalPlayer);
	}
}

void UGameUIManagerSubsystem::NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerDestroyed(LocalPlayer);
	}
}