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
	// �˾Ƶθ� ���� �͵��� �ΰ����� �ִ�.
	// 1. FParse::Param�� FCommandLine::Get()�� ����, �츮�� ���ϴ� �Ӽ� ���� CommandLine���� ������ �� �ִ�.
	//		- TestClone�� ���� �ǽ��غ���.
	// 2. �Լ� �ȿ� static ���� �����̴�.
	//		- �𸮾󿡼� ���� ���� ��������, ���� �۷ι� ������ �����ϰ� ���� ���, �츮���Դ� �ΰ��� �ɼ��� �ִ�.
	//		 1. .h ���Ͽ� extern�� �����Ͽ�, ������Ͽ� �����ϰų�
	//		 2. static �Լ��� �����Ͽ�, �ش� �Լ� �ȿ� static ������ �����Ͽ�, ��ȯ�ϴ� ���. (��ǥ������ singletone ������ �ִ�.)

	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoad"));
	return bLogAssetLoads;
}

UObject* UR3AssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	// �ش� �Լ��� ���� ���� ������ 'Synchronous Load Asset�� ���ʿ��ϰ� �ϴ� ���� ������ Ȯ���ϱ� ����!
	if (AssetPath.IsValid())
	{
		// FScopeLogTime�� Ȯ���غ���.
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			// �ܼ��� �α��ϸ鼭 �ʴ����� �α�����
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}
		// ���⼭ �ΰ��� �б�.
		// 1. AssetManager�� ������, AssetManager�� StreamableManager�� ���� ���� �ε�.
		// 2. �ƴϸ� FSoftObjectPath�� ���� �ٷ� ���� �ε�

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// if asset manager is not ready, use LoadObject()
		// - ��, StaticLoadObject�� ���δ�.
		// - �����, �׻� StaticLoadObject�ϱ� ���� StaticFindObject�� ���� Ȯ���ϰ� �����ϸ�, ��¥ �ε��ع���
		return AssetPath.TryLoad();

	}

	return nullptr;
}
void UR3AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UE_LOG(LogR3, Display, TEXT("FR3GameplayTags::StartInitialLoading"));


	// FLyraCloneGameplayTags �ʱ�ȭ
	//	- Lyra�� �������� �ѹ� ����...
	//		- STARTUP_JOB() ��ũ�ε� ����¢������, ������ �ε��� ���� �����Ƿ� �����ϰ�
	FR3GameplayTags::InitializeNativeTags();

}