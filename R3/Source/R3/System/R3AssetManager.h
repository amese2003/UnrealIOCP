#pragma once

#include "Engine/AssetManager.h"
#include "R3AssetManager.generated.h"

UCLASS()
class UR3AssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UR3AssetManager();

	// 싱글톤 메소드
	static UR3AssetManager& Get();

	// [Thread-safe] 메모리에 로딩된 에셋 캐싱
	void AddLoadedAsset(const UObject* Asset);

	/* 클론 코딩 과정에서 테스트를 위한 함수 */
	static bool TestClone();

	/*
	* 해당 함수는 FScopeLogTime을 사용할지 않을지 유무 판단을 위해 사용
	* -  굳이 클론한 필요는 없는데, 알아두면 좋은점이 많다!
	*
	*/
	static bool ShouldLogAssetLoads();

	/* UR3AssetManager가 로딩을 관장함을 알 수 있다.*/
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);


	/*
	*	GetAsset은 정적 로딩으로, BP Class와 Object 로딩
	*	- 주목해야 할 부분은 bKeepInMemory
	*/

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	/**
	*
	*/

	virtual void StartInitialLoading() final;

	/**
	*	멤버변수
	*/


	// Assets loaded and tracked by the asset manager.
	// GC의 대상
	UPROPERTY()
		TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Object 단위 locking
	FCriticalSection SyncObject;;
};


template<typename AssetType>
AssetType* UR3AssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// 여기서 AddLoadAsset은 메모리에 상주시키기 위한 장치라고 생각하면 됨.
			// - 한번 등록되면 직접 내리지 않는한은 Unload가 되지 않음 (== 케싱)
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template<typename AssetType>
TSubclassOf<AssetType> UR3AssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}
