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
		*	참고로 delegate 객체를 살펴보면, 내부적으로 필요한 변수들은 메모리 할당해놓는다.
		*	TArray<int> a = {1,2,3,4};
		*	delegate_type delegate [a](){
		*	return a.Num();
		*	}
		*	a는 delegate_type 내부에 new로 할당 되어있다. 복사 비용을 낮추기 위해 Movve를 통해 하는것을 잊지말자.
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

	// 왜 CDO?
	const UR3ExperienceDefinition* Experience = GetDefault<UR3ExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr)
	{
		// 그리고 CDO로 CurrentExperience를 설정한다.
		// 어떤 의도로 이렇게 코드를 작성했는지는 코드를 쭉 읽어보고 (StartExperienceLoaded까지 읽어보자..) 다시 생각해보자.
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

	// 앞서, ServerSetCurrentExperience에서 우리는 ExperienceID를 넘겨주는데, 여기서 CDO를 활용해서 GetPrimaryAssetId를 로딩할 대상으로 넣는다.
	//	- 왜 이렇게 하는걸까?
	//	- GetPrimaryAssetId를 좀 더 자세히 보자.
	//	- GetPrimaryAssetId를 살펴봄으로써, 우리는 아래 두가지를 얻는다.
	//		1. 우리가 BP_LyraCloneDefaultExperience를 BP로 만든 이유
	//		2. CDO를 가져와서, GetPrimaryAssetId를 호출한 이유.

	// 우리는 앞서 이미 CDO를 로딩하여, CDO를 사용하지 않고, CDO를 사용하여 로딩할 에셋을 지정하여, BundleAssetList에 추가한다!


	/**
	*	UPrimaryDataAsset에 대한 설명이 클래스 원본위에 주석이 달려있다. 코멘트로도 설명은 다 불가능한데, GetPrimaryAssetId()를 보고 정리하면...
	*	- UPrimaryDataAsset은 GetPrimaryAssetId() 구현과 AssetBundle에 대한 구현이 추가된 DataAsset이다.
	*		- 해당 기능은 AssetManager를 통해 load/Unload 관련 메소드 호출이 가능하다.
	*	- GetPrimaryAssetId()를 통해 아래의 두가지 패턴이 구현이 가능하다.
	*		1. UPrimaryDataAsset를 상속 받은 Native(C++) 클래스를 정의하면, 그 정의된 클래스를 PrimaryAssetType으로써 사용가능하다. (ULyraCloneExperienceDefinition)
	*		2. UPrimaryDataAsset를 상속 받은 Blueprint는 아래 두가지 방식에 따라 PrimaryAssetType이 정의된다.
	*			- 부모(Parent class)를 거슬러 올라가며, 가장 처음으로 만나는 Native(C++) 클래스
	*			- 또는 가장 부모인 (the most ancestor) BP 클래스 (아마 해당 BP 클래스는 UPrimaryDataAsset를 바로 상속 받았을것...)
	*	- 이와 같은 동적 방식을 바꾸고 싶으면, GetPrimaryAssetId는 이미 virtual로 정의되어 있고, 이를 override를 하면된다.
	*/

	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	// ExperienceActionSet을 순회하며, BundleAssetList로 추가하자
	for (const TObjectPtr<UR3ExperienceActionSet>& ActionSet : CurrentExperience->ActionsSets)
	{
		if (ActionSet)
		{
			// 앞서, 우리가 생성한 LCAS_Shooter_SharedHUD가 추가되었다 (물론 추가적인 LCAS_Shooter_***도 추가될거다)
			// - BundleAssetList는 Bundle로 등록할 Root의 PrimaryDataAsset을 추가하는 과정이다
			//	 (->??? 이게 무슨말인가 싶은데 ChangeBundleStateForPrimaryAssets를 살펴보며 이해하자)
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// load assets asscociated with the experience
	// 아래는 우리가 후일 GameFeatures를 사용하여, Experience에 바인딩한 GameFeature Plugin를 로딩할 Bundle 이름을 추가한다.
	//	- Bundle이란게, 우리가 후일 로딩할 에셋의 카테고리 이름이라고 생각하면 된다 ( 일단 패스! )
	TArray<FName> BundlesToLoad;
	{
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);

		//// ***추가!
		//// 이제서야 의미를 알게 될것이다!
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

	// 아래도, 후일 Bundle을 우리가 GameFeature에 연동하면서 더 깊게 알아보기로 하고, 지금은 BP_r3DefaultExperience를 로딩해주는  함수로 생각해보자.
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority
	);



	if (Handle.IsValid() == false || Handle->HasLoadCompleted())
	{
		// 로딩이 끝났으면, ExecuteDelegate를 통해 OnAssetsLoadedDelegate를 호출하자.
		// - 아래 함수를 확인해보자.
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

	// FrameNumber를 주목해서 보자..
	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;
}

void UR3ExperienceManagerComponent::OnExperienceLoadComplete()
{
	// FrameNumber를 주목해서 보자..
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	check(LoadState == ER3ExperienceLoadState::Loading);
	check(CurrentExperience);

	// 이전 활성화 된 GameFeature Plugin의 URL를 활성화 해 준다
	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
	{
		// FeaturePluginList를 순회하며, PluginURL를 ExperienceManagerComponent의 GameFeaturePluginURLs에 추가한다.
		for (const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
			{
				This->GameFeaturePluginURLs.AddUnique(PluginURL);
			}
		}
	};

	// GameFeatureToEnable에 있는 Plugin만 일단 활성화할 GameFeature Plugin 후보군으로 등록
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	// GameFeaturePluginURLs에 등록된 Plugin 로딩 및 활성화
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();

	if (NumGameFeaturePluginsLoading)
	{
		LoadState = ER3ExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			// 매 Plugin이 로딩 및 활성화 될 경우, OnGameFeaturePluginLoadComplete 콜백 함수 등록
			// 해당 함수를 살펴보자.
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		// 해당 함수가 불리는 것은 앞서 보았던 StreamableDelegateDelayHelper에 의해 불림
		OnExperienceFullLoadCompleted();
	}
}

void UR3ExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// 매 GameFeature Plugin에 로딩할 떄, 해당 함수가 콜백으로 동작한다
	NumGameFeaturePluginsLoading--;
	if (NumGameFeaturePluginsLoading == 0)
	{
		// GameFeaturePlugins 로딩이 다 끝났을 경우, 기존대로 Loaded로서, OnExperienceFullLoadCompleted 호출한다.
		// GameFeaturePlugins 로딩과 활성화가 끝났다면? UGameFeatureAction을 활성화해야한다 (좀이따가...)
		OnExperienceFullLoadCompleted();
	}
}

void UR3ExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ER3ExperienceLoadState::Loaded);

	// GameFeature Plugin의 로딩과 활성화 이후, GameFeature Action들을 활성화 시키자..
	{
		LoadState = ER3ExperienceLoadState::ExecutingActions;

		// GameFeatureAction 활성화를 위한 Context 준비
		FGameFeatureActivatingContext Context;
		{
			// 월드의 핸들을 세팅한다
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
				// 명시적으로 GameFeatureAction에 대해 Registering -> Loading -> Activating 순으로 호출한다
				if (Action)
				{
					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

		// 1. Experience의 Actions
		ActivateListOfActions(CurrentExperience->Actions);

		// 2. Experience의 ActionSets
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
