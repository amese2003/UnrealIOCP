#include "R3GameplayTags.h"
#include "R3LogChannel.h"
#include "GameplayTagsManager.h"

FR3GameplayTags FR3GameplayTags::GameplayTags;

void FR3GameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FR3GameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

void FR3GameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	/**
	*	GameFrameworkComponentManager init state tags
	*/

	AddTag(InitState_Spawned, "InitState.Spawned", "1: Actor/Component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is ready for active gameplay");

	/**
	*	Enhanced Input Tags
	*/
	AddTag(InputTag_Move, "InputTag.Move", "");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "");

}
