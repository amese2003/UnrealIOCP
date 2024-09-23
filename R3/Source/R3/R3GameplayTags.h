#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"


// ���漱��
class UGameplayTagsManager;

/**
* LyraCloneGameplayTags
*	- singleton containing native gameplay tags
*/

struct FR3GameplayTags
{
	/**
	* static methods
	*/

public:
	static const FR3GameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	/**
	*  member methods
	*/
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);

	/** �Ʒ� GameplayTags�� �ʱ�ȭ ���� �ܰ踦 �ǹ��Ѵ�.
	*	- GameplayInstance�� �ʱ�ȭ ������ UGameFrameworkComponentManager�� RegisterInitState�� ��ϵǾ� ������(Linear)���� ������Ʈ �ȴ�
	*	- �� �ʱ�ȭ GameplayTag�� ������ Actor ���� �����Ǹ�, GameFrameworkInitStateInterface ��ӹ��� Ŭ������ �ʱ�ȭ ����(Init State)�� ���¸ӽŰ� ���� ��������
	*/
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;


	/**
	*  Enhanced Input Tags
	*/
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;


private:
	// static ���� �ʱ�ȭ�� .cpp���� ���ִ°� ��������.
	static FR3GameplayTags GameplayTags;
};