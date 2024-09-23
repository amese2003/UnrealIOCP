//#pragma once
//
//#include "Containers/Array.h"
//#include "GameFeatureStateChangeObserver.h"
//#include "GameFeaturesProjectPolicies.h"
//#include "UObject/Object.h"
//#include "UObject/ObjectPtr.h"
//#include "UObject/UObjectGlobals.h"
//#include "R3GameFeaturePolicy.generated.h"
//
///**
//*  UR3GameFeaturePolicy는 GameFeature Plugin이 Memory에 로딩되거나, Active(활성화)를 관리하는 StateMachine을 모니터링 가능하다
//*/
//UCLASS()
//class UR3GameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
//{
//	GENERATED_BODY()
//
//public:
//	UR3GameFeaturePolicy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	/**
//	* R3GameFeaturePolicy interfaces
//	*/
//	virtual void InitGameFeatureManager() override;
//	virtual void ShutdownGameFeatureManager() override;
//
//
//	UPROPERTY(Transient)
//		TArray<TObjectPtr<UObject>> Observers;
//};
//
///**
//*  GameFeature Plugin이 Register/Unregister 단계에서 GameplayCuePath가 등록되어 GameplayCueManager가 관련 GameplayCue 에셋을 스캔할 수 있게 해야한다
//*  이를 위해 GameFeatureStateChangeObserver를 활용하여 옵저버 패턴으로 이를 가능하게 하도록 한다
//*	- 미래에 우리가 Override될 메서드에서 유추할 수 있다싶이, GameFeatureStateChangeObserver를 활용하면 가능하다
//*	- 이를 위해 추가적인 구현이 필요한데, 이는 ULyraCloneGameFeaturePolicy를 참고하자
//*
//*
//*  참고로 GameFeatureAction에도 Registering/Unregistering이 있긴하다.
//*	- 하지만, 우리가 앞서 구현했다싶이 직접 Registering/Unregistering을 호출해야한다
//*	- Registering/Unregistering 이걸 호출 안하면 아무 쓸모도 없다...
//*  - Lyra에선 GameplayFeaturePolicy를 오버라이딩 하는 방식으로 이를 진행했다.
//*/
//UCLASS()
//class UR3GameFeature_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
//{
//	GENERATED_BODY()
//
//public:
//	/**
//	*  IGameFeatureStateChangeObserver Interface
//	*/
//	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
//	virtual void OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
//
//};