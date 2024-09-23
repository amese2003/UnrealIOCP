//#include "R3GameFeaturePolicy.h"
//#include "GameFeaturesSubsystem.h"
//#include "GameFeatureAction.h"
//#include "GameFeatureData.h"
//#include "GameplayCueSet.h"
//#include "AbilitySystemGlobals.h"
//#include UE_INLINE_GENERATED_CPP_BY_NAME(R3GameFeaturePolicy)
//
//UR3GameFeaturePolicy::UR3GameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//}
//
//void UR3GameFeaturePolicy::InitGameFeatureManager()
//{
//	// GameFeature_AddGameplayCuePath를 등록
//	Observers.Add(NewObject<UR3GameFeature_AddGameplayCuePaths>());
//
//	// Observers를 순회하며, GameFeatureSubsystem에 Observer를 순회하며 등록
//	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
//	for (UObject* Observer : Observers)
//	{
//		Subsystem.AddObserver(Observer);
//	}
//
//	// 앞서 Subsystem에 이미 Observer를 등록했으므로, GameFeatureManager가 초기화되는 과정에 반영할 것임
//	Super::InitGameFeatureManager();
//}
//
//void UR3GameFeaturePolicy::ShutdownGameFeatureManager()
//{
//	Super::ShutdownGameFeatureManager();
//
//	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
//	for (UObject* Observer : Observers)
//	{
//		Subsystem.RemoveObserver(Observer);
//	}
//
//	Observers.Empty();
//}
//
//void UR3GameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
//{
//	// PluginName을 활용하여, PluginRootPath를 계산
//	const FString PluginRootPath = TEXT("/") + PluginName;
//
//	// GameFeatureData의 Action 순회
//	// - 예로들어, ShooterCore의 GameFeatureData에 Action을 추가했다면, 그 추가된 Action의 대상이 됨
//	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
//	{
//		// 그중에 우리는 AddGameplayCuePath를
//		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
//		{
//			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
//
//			// GameplayCueManager를 가져와서, GFA에 등록된 DirsToAdd를 추가해서 GCM의 데이터가 업데이트 되도록 진행
//			if (ULyraCloneGameplayCueManager* GCM = ULyraCloneGameplayCueManager::Get())
//			{
//				// RuntimeCueSet을 가져옴
//				UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
//				const int32 PreInitalizeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//
//				for (const FDirectoryPath& Directory : DirsToAdd)
//				{
//					FString MutablePath = Directory.Path;
//
//					// PluginPackagePath를 한번 보정해 줌 -> 보정된 효과는 MutablePath로 들어옴
//					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
//
//					// GCM의 RuntimeGameplayCueObjectLibrary의 Path에 추가
//					GCM->AddGameplayCueNotifyPath(MutablePath, /*bShouldRescanCueAssets*/false);
//				}
//
//				// 일단 초기화 (새 경로 추가 되었으니 호출해줘야함)
//				if (!DirsToAdd.IsEmpty())
//				{
//					GCM->InitializeRuntimeObjectLibrary();
//				}
//
//				// Cue의 에셋이 pre와 post랑 비교하여 갯수가 달라져있다면, 명시적으로 RefreshGameplayCuePrimaryAsset 호출
//				const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//
//				if (PreInitalizeNumCues != PostInitializeNumCues)
//				{
//					// RefreshGameplayCuePrimaryAsset 함수는 추가된 GCN을 AssetManager에 등록해주는 역할을 한다
//					GCM->RefreshGameplayCuePrimaryAsset();
//				}
//			}
//		}
//	}
//}
//
//void UR3GameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
//{
//	const FString PluginRootPath = TEXT("/") + PluginName;
//	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
//	{
//		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(GameFeatureData))
//		{
//			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
//			if (ULyraCloneGameplayCueManager* GCM = ULyraCloneGameplayCueManager::Get())
//			{
//				int32 NumRemoved = 0;
//				for (const FDirectoryPath& Directory : DirsToAdd)
//				{
//					FString MutablePath = Directory.Path;
//					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
//					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, /** bShouldRescanCueAssets*/false);
//				}
//
//				ensure(NumRemoved == DirsToAdd.Num());
//
//				if (NumRemoved > 0)
//				{
//					GCM->InitializeRuntimeObjectLibrary();
//				}
//			}
//		}
//	}
//}
