#pragma once

#include "Engine/AssetManager.h"
#include "R3AssetManager.generated.h"

UCLASS()
class UR3AssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UR3AssetManager();

	// �̱��� �޼ҵ�
	static UR3AssetManager& Get();

	// [Thread-safe] �޸𸮿� �ε��� ���� ĳ��
	void AddLoadedAsset(const UObject* Asset);

	/* Ŭ�� �ڵ� �������� �׽�Ʈ�� ���� �Լ� */
	static bool TestClone();

	/*
	* �ش� �Լ��� FScopeLogTime�� ������� ������ ���� �Ǵ��� ���� ���
	* -  ���� Ŭ���� �ʿ�� ���µ�, �˾Ƶθ� �������� ����!
	*
	*/
	static bool ShouldLogAssetLoads();

	/* UR3AssetManager�� �ε��� �������� �� �� �ִ�.*/
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);


	/*
	*	GetAsset�� ���� �ε�����, BP Class�� Object �ε�
	*	- �ָ��ؾ� �� �κ��� bKeepInMemory
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
	*	�������
	*/


	// Assets loaded and tracked by the asset manager.
	// GC�� ���
	UPROPERTY()
		TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Object ���� locking
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
			// ���⼭ AddLoadAsset�� �޸𸮿� ���ֽ�Ű�� ���� ��ġ��� �����ϸ� ��.
			// - �ѹ� ��ϵǸ� ���� ������ �ʴ����� Unload�� ���� ���� (== �ɽ�)
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
