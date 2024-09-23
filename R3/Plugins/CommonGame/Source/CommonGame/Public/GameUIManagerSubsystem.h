#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameUIManagerSubsystem.generated.h"

/** foward declaration */
class UGameUIPolicy;
class UCommonLocalPlayer;

/**
*  GameUIManagerSubsystem은 GameInstanceSubsystem에 기반한다
* - 여기서 주목할 점은 UGameUIManagerSubsystem은 UCLASS 속성으로 Abstract라고 선언되어있다
* - 해당 클래스는 단독으로 사용 불가능하며, 누군가 상속받는 Concrete Class로서 활용되야 한다
*/
UCLASS(Abstract, config=Game)
class COMMONGAME_API UGameUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGameUIManagerSubsystem();

	const UGameUIPolicy* GetCurrentUIPolicy() const { return CurrentPolicy; }
	UGameUIPolicy* GetCurrentUIPolicy() { return CurrentPolicy; }

	UPROPERTY(Transient)
		TObjectPtr<UGameUIPolicy> CurrentPolicy = nullptr;

	void SwitchToPolicy(UGameUIPolicy* InPolicy);

	/**
	* UGameInstanceSubsystem's interface
	*/
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/**
	* UGameUIManagerSubsystem's interface
	*/
	void NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer);

	/**
	*  default 및 UI policy를 생성할 class
	* - 우리는 해당 클래스는 BP_LyraCloneGameUIPolicy로 지정할 것이다
	*/
	UPROPERTY(Config, EditAnywhere)
		TSoftClassPtr<UGameUIPolicy> DefaultUIPolicyClass;
};