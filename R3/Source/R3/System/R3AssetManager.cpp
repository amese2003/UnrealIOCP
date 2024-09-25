#include "R3AssetManager.h"
#include "R3/R3LogChannel.h"
#include "R3/R3GameplayTags.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3AssetManager)


UR3AssetManager::UR3AssetManager() : Super()
{
}

UR3AssetManager& UR3AssetManager::Get()
{
	check(GEngine);

	if (UR3AssetManager* Singleton = Cast<UR3AssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogR3, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to R3AssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UR3AssetManager>();
}

void UR3AssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}

bool UR3AssetManager::TestClone()
{
	static bool bTest = ShouldLogAssetLoads();
	return bTest;
}

bool UR3AssetManager::ShouldLogAssetLoads()
{
	// 알아두면 좋은 것들이 두가지가 있다.
	// 1. FParse::Param과 FCommandLine::Get()을 통해, 우리가 원하는 속성 값을 CommandLine으로 가져올 수 있다.
	//		- TestClone을 통해 실습해보자.
	// 2. 함수 안에 static 변수 선언이다.
	//		- 언리얼에서 자주 보는 패턴으로, 보통 글로벌 변수를 선언하고 싶은 경우, 우리에게는 두가지 옵션이 있다.
	//		 1. .h 파일에 extern을 선언하여, 헤더파일에 포함하거나
	//		 2. static 함수를 선언하여, 해당 함수 안에 static 변수를 선언하여, 반환하는 방법. (대표적으로 singletone 패턴이 있다.)

	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoad"));
	return bLogAssetLoads;
}

UObject* UR3AssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	// 해당 함수를 따로 만든 이유는 'Synchronous Load Asset이 불필요하게 하는 것이 없는지 확인하기 위함!
	if (AssetPath.IsValid())
	{
		// FScopeLogTime을 확인해보자.
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			// 단순히 로깅하면서 초단위로 로깅진행
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}
		// 여기서 두가지 분기.
		// 1. AssetManager가 있으면, AssetManager의 StreamableManager를 통해 정적 로딩.
		// 2. 아니면 FSoftObjectPath를 통해 바로 정적 로딩

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// if asset manager is not ready, use LoadObject()
		// - 즉, StaticLoadObject가 보인다.
		// - 참고로, 항상 StaticLoadObject하기 전에 StaticFindObject를 통해 확인하고 실패하면, 진짜 로딩해버림
		return AssetPath.TryLoad();

	}

	return nullptr;
}
void UR3AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UE_LOG(LogR3, Display, TEXT("FR3GameplayTags::StartInitialLoading"));


	// FLyraCloneGameplayTags 초기화
	//	- Lyra와 차이점을 한번 보자...
	//		- STARTUP_JOB() 매크로도 선택짖였으나, 지금은 로딩과 관계 없으므로 간단하게
	FR3GameplayTags::InitializeNativeTags();

}