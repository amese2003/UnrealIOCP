#include "R3ExperienceManagerComponent.h"
#include "R3ExperienceDefinition.h"
#include "R3ExperienceActionSet.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Net/UnrealNetwork.h"
#include "R3/System/R3AssetManager.h"
#include "GameFeatureAction.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3ExperienceManagerComponent)

UR3ExperienceManagerComponent::UR3ExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR3ExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnR3ExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		/**
		*	����� delegate ��ü�� ���캸��, ���������� �ʿ��� �������� �޸� �Ҵ��س��´�.
		*	TArray<int> a = {1,2,3,4};
		*	delegate_type delegate [a](){
		*	return a.Num();
		*	}
		*	a�� delegate_type ���ο� new�� �Ҵ� �Ǿ��ִ�. ���� ����� ���߱� ���� Movve�� ���� �ϴ°��� ��������.
		*/
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UR3ExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UR3AssetManager& AssetManager = UR3AssetManager::Get();

	TSubclassOf<UR3ExperienceDefinition> AssetClass;
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad());
	}

	// �� CDO?
	const UR3ExperienceDefinition* Experience = GetDefault<UR3ExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr)
	{
		// �׸��� CDO�� CurrentExperience�� �����Ѵ�.
		// � �ǵ��� �̷��� �ڵ带 �ۼ��ߴ����� �ڵ带 �� �о�� (StartExperienceLoaded���� �о��..) �ٽ� �����غ���.
		CurrentExperience = Experience;
	}

	StartExperienceLoaded();
}

void UR3ExperienceManagerComponent::StartExperienceLoaded()
{
	check(CurrentExperience);
	check(LoadState == ER3ExperienceLoadState::Unloaded);

	LoadState = ER3ExperienceLoadState::Loading;
	UR3AssetManager& AssetManager = UR3AssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;

	// �ռ�, ServerSetCurrentExperience���� �츮�� ExperienceID�� �Ѱ��ִµ�, ���⼭ CDO�� Ȱ���ؼ� GetPrimaryAssetId�� �ε��� ������� �ִ´�.
	//	- �� �̷��� �ϴ°ɱ�?
	//	- GetPrimaryAssetId�� �� �� �ڼ��� ����.
	//	- GetPrimaryAssetId�� ���캽���ν�, �츮�� �Ʒ� �ΰ����� ��´�.
	//		1. �츮�� BP_LyraCloneDefaultExperience�� BP�� ���� ����
	//		2. CDO�� �����ͼ�, GetPrimaryAssetId�� ȣ���� ����.

	// �츮�� �ռ� �̹� CDO�� �ε��Ͽ�, CDO�� ������� �ʰ�, CDO�� ����Ͽ� �ε��� ������ �����Ͽ�, BundleAssetList�� �߰��Ѵ�!


	/**
	*	UPrimaryDataAsset�� ���� ������ Ŭ���� �������� �ּ��� �޷��ִ�. �ڸ�Ʈ�ε� ������ �� �Ұ����ѵ�, GetPrimaryAssetId()�� ���� �����ϸ�...
	*	- UPrimaryDataAsset�� GetPrimaryAssetId() ������ AssetBundle�� ���� ������ �߰��� DataAsset�̴�.
	*		- �ش� ����� AssetManager�� ���� load/Unload ���� �޼ҵ� ȣ���� �����ϴ�.
	*	- GetPrimaryAssetId()�� ���� �Ʒ��� �ΰ��� ������ ������ �����ϴ�.
	*		1. UPrimaryDataAsset�� ��� ���� Native(C++) Ŭ������ �����ϸ�, �� ���ǵ� Ŭ������ PrimaryAssetType���ν� ��밡���ϴ�. (ULyraCloneExperienceDefinition)
	*		2. UPrimaryDataAsset�� ��� ���� Blueprint�� �Ʒ� �ΰ��� ��Ŀ� ���� PrimaryAssetType�� ���ǵȴ�.
	*			- �θ�(Parent class)�� �Ž��� �ö󰡸�, ���� ó������ ������ Native(C++) Ŭ����
	*			- �Ǵ� ���� �θ��� (the most ancestor) BP Ŭ���� (�Ƹ� �ش� BP Ŭ������ UPrimaryDataAsset�� �ٷ� ��� �޾�����...)
	*	- �̿� ���� ���� ����� �ٲٰ� ������, GetPrimaryAssetId�� �̹� virtual�� ���ǵǾ� �ְ�, �̸� override�� �ϸ�ȴ�.
	*/

	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	// ExperienceActionSet�� ��ȸ�ϸ�, BundleAssetList�� �߰�����
	for (const TObjectPtr<UR3ExperienceActionSet>& ActionSet : CurrentExperience->ActionsSets)
	{
		if (ActionSet)
		{
			// �ռ�, �츮�� ������ LCAS_Shooter_SharedHUD�� �߰��Ǿ��� (���� �߰����� LCAS_Shooter_***�� �߰��ɰŴ�)
			// - BundleAssetList�� Bundle�� ����� Root�� PrimaryDataAsset�� �߰��ϴ� �����̴�
			//	 (->??? �̰� �������ΰ� ������ ChangeBundleStateForPrimaryAssets�� ���캸�� ��������)
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// load assets asscociated with the experience
	// �Ʒ��� �츮�� ���� GameFeatures�� ����Ͽ�, Experience�� ���ε��� GameFeature Plugin�� �ε��� Bundle �̸��� �߰��Ѵ�.
	//	- Bundle�̶���, �츮�� ���� �ε��� ������ ī�װ� �̸��̶�� �����ϸ� �ȴ� ( �ϴ� �н�! )
	TArray<FName> BundlesToLoad;
	{
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);

		//// ***�߰�!
		//// �������� �ǹ̸� �˰� �ɰ��̴�!
		// - LoadStateClient = "Client, LoadStateServer = "Server
		if (bLoadClient)
		{
			//UGameFeaturesSubsystemSettings
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}

		if (bLoadServer)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
		}
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	// �Ʒ���, ���� Bundle�� �츮�� GameFeature�� �����ϸ鼭 �� ��� �˾ƺ���� �ϰ�, ������ BP_r3DefaultExperience�� �ε����ִ�  �Լ��� �����غ���.
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority
	);



	if (Handle.IsValid() == false || Handle->HasLoadCompleted())
	{
		// �ε��� ��������, ExecuteDelegate�� ���� OnAssetsLoadedDelegate�� ȣ������.
		// - �Ʒ� �Լ��� Ȯ���غ���.
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}

	// FrameNumber�� �ָ��ؼ� ����..
	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;
}

void UR3ExperienceManagerComponent::OnExperienceLoadComplete()
{
	// FrameNumber�� �ָ��ؼ� ����..
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	check(LoadState == ER3ExperienceLoadState::Loading);
	check(CurrentExperience);

	// ���� Ȱ��ȭ �� GameFeature Plugin�� URL�� Ȱ��ȭ �� �ش�
	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
	{
		// FeaturePluginList�� ��ȸ�ϸ�, PluginURL�� ExperienceManagerComponent�� GameFeaturePluginURLs�� �߰��Ѵ�.
		for (const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
			{
				This->GameFeaturePluginURLs.AddUnique(PluginURL);
			}
		}
	};

	// GameFeatureToEnable�� �ִ� Plugin�� �ϴ� Ȱ��ȭ�� GameFeature Plugin �ĺ������� ���
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	// GameFeaturePluginURLs�� ��ϵ� Plugin �ε� �� Ȱ��ȭ
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();

	if (NumGameFeaturePluginsLoading)
	{
		LoadState = ER3ExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			// �� Plugin�� �ε� �� Ȱ��ȭ �� ���, OnGameFeaturePluginLoadComplete �ݹ� �Լ� ���
			// �ش� �Լ��� ���캸��.
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		// �ش� �Լ��� �Ҹ��� ���� �ռ� ���Ҵ� StreamableDelegateDelayHelper�� ���� �Ҹ�
		OnExperienceFullLoadCompleted();
	}
}

void UR3ExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// �� GameFeature Plugin�� �ε��� ��, �ش� �Լ��� �ݹ����� �����Ѵ�
	NumGameFeaturePluginsLoading--;
	if (NumGameFeaturePluginsLoading == 0)
	{
		// GameFeaturePlugins �ε��� �� ������ ���, ������� Loaded�μ�, OnExperienceFullLoadCompleted ȣ���Ѵ�.
		// GameFeaturePlugins �ε��� Ȱ��ȭ�� �����ٸ�? UGameFeatureAction�� Ȱ��ȭ�ؾ��Ѵ� (���̵���...)
		OnExperienceFullLoadCompleted();
	}
}

void UR3ExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ER3ExperienceLoadState::Loaded);

	// GameFeature Plugin�� �ε��� Ȱ��ȭ ����, GameFeature Action���� Ȱ��ȭ ��Ű��..
	{
		LoadState = ER3ExperienceLoadState::ExecutingActions;

		// GameFeatureAction Ȱ��ȭ�� ���� Context �غ�
		FGameFeatureActivatingContext Context;
		{
			// ������ �ڵ��� �����Ѵ�
			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
			if (ExistingWorldContext)
			{
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
			}
		}

		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				// ��������� GameFeatureAction�� ���� Registering -> Loading -> Activating ������ ȣ���Ѵ�
				if (Action)
				{
					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

		// 1. Experience�� Actions
		ActivateListOfActions(CurrentExperience->Actions);

		// 2. Experience�� ActionSets
		for (const TObjectPtr<UR3ExperienceActionSet>& ActionSet : CurrentExperience->ActionsSets)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}



	LoadState = ER3ExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const UR3ExperienceDefinition* UR3ExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == ER3ExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}
