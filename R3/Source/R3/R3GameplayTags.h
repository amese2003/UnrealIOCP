#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"


// 전방선언
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

	/** 아래 GameplayTags는 초기화 과정 단계를 의미한다.
	*	- GameplayInstance의 초기화 과정에 UGameFrameworkComponentManager의 RegisterInitState로 등록되어 선형적(Linear)으로 업데이트 된다
	*	- 이 초기화 GameplayTag는 게임의 Actor 사이 공유되며, GameFrameworkInitStateInterface 상속받은 클래스는 초기화 상태(Init State)를 상태머신과 같이 관리가능
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
	// static 변수 초기화는 .cpp에서 해주는걸 잊지말자.
	static FR3GameplayTags GameplayTags;
};