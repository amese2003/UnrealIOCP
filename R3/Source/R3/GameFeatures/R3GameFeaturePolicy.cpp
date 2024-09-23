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
//	// GameFeature_AddGameplayCuePath�� ���
//	Observers.Add(NewObject<UR3GameFeature_AddGameplayCuePaths>());
//
//	// Observers�� ��ȸ�ϸ�, GameFeatureSubsystem�� Observer�� ��ȸ�ϸ� ���
//	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
//	for (UObject* Observer : Observers)
//	{
//		Subsystem.AddObserver(Observer);
//	}
//
//	// �ռ� Subsystem�� �̹� Observer�� ��������Ƿ�, GameFeatureManager�� �ʱ�ȭ�Ǵ� ������ �ݿ��� ����
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
//	// PluginName�� Ȱ���Ͽ�, PluginRootPath�� ���
//	const FString PluginRootPath = TEXT("/") + PluginName;
//
//	// GameFeatureData�� Action ��ȸ
//	// - ���ε��, ShooterCore�� GameFeatureData�� Action�� �߰��ߴٸ�, �� �߰��� Action�� ����� ��
//	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
//	{
//		// ���߿� �츮�� AddGameplayCuePath��
//		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
//		{
//			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
//
//			// GameplayCueManager�� �����ͼ�, GFA�� ��ϵ� DirsToAdd�� �߰��ؼ� GCM�� �����Ͱ� ������Ʈ �ǵ��� ����
//			if (ULyraCloneGameplayCueManager* GCM = ULyraCloneGameplayCueManager::Get())
//			{
//				// RuntimeCueSet�� ������
//				UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
//				const int32 PreInitalizeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//
//				for (const FDirectoryPath& Directory : DirsToAdd)
//				{
//					FString MutablePath = Directory.Path;
//
//					// PluginPackagePath�� �ѹ� ������ �� -> ������ ȿ���� MutablePath�� ����
//					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
//
//					// GCM�� RuntimeGameplayCueObjectLibrary�� Path�� �߰�
//					GCM->AddGameplayCueNotifyPath(MutablePath, /*bShouldRescanCueAssets*/false);
//				}
//
//				// �ϴ� �ʱ�ȭ (�� ��� �߰� �Ǿ����� ȣ���������)
//				if (!DirsToAdd.IsEmpty())
//				{
//					GCM->InitializeRuntimeObjectLibrary();
//				}
//
//				// Cue�� ������ pre�� post�� ���Ͽ� ������ �޶����ִٸ�, ��������� RefreshGameplayCuePrimaryAsset ȣ��
//				const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//
//				if (PreInitalizeNumCues != PostInitializeNumCues)
//				{
//					// RefreshGameplayCuePrimaryAsset �Լ��� �߰��� GCN�� AssetManager�� ������ִ� ������ �Ѵ�
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
