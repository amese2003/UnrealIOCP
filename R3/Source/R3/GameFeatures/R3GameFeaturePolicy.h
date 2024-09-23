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
//*  UR3GameFeaturePolicy�� GameFeature Plugin�� Memory�� �ε��ǰų�, Active(Ȱ��ȭ)�� �����ϴ� StateMachine�� ����͸� �����ϴ�
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
//*  GameFeature Plugin�� Register/Unregister �ܰ迡�� GameplayCuePath�� ��ϵǾ� GameplayCueManager�� ���� GameplayCue ������ ��ĵ�� �� �ְ� �ؾ��Ѵ�
//*  �̸� ���� GameFeatureStateChangeObserver�� Ȱ���Ͽ� ������ �������� �̸� �����ϰ� �ϵ��� �Ѵ�
//*	- �̷��� �츮�� Override�� �޼��忡�� ������ �� �ִٽ���, GameFeatureStateChangeObserver�� Ȱ���ϸ� �����ϴ�
//*	- �̸� ���� �߰����� ������ �ʿ��ѵ�, �̴� ULyraCloneGameFeaturePolicy�� ��������
//*
//*
//*  ����� GameFeatureAction���� Registering/Unregistering�� �ֱ��ϴ�.
//*	- ������, �츮�� �ռ� �����ߴٽ��� ���� Registering/Unregistering�� ȣ���ؾ��Ѵ�
//*	- Registering/Unregistering �̰� ȣ�� ���ϸ� �ƹ� ���� ����...
//*  - Lyra���� GameplayFeaturePolicy�� �������̵� �ϴ� ������� �̸� �����ߴ�.
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