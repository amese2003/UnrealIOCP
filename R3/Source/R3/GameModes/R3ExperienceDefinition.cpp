#include "R3ExperienceDefinition.h"
#include "GameFeatureAction.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3ExperienceDefinition)

UR3ExperienceDefinition::UR3ExperienceDefinition(const FObjectInitializer& ObjectInitializer)
{
}

#if WITH_EDITORONLY_DATA
void UR3ExperienceDefinition::UpdateAssetBundleData()
{
	// 우리는 UpdateAssetBundleData() 코드를 한번 봐야한다
	Super::UpdateAssetBundleData();

	//for (UGameFeatureAction* Action : Actions)
	//{
	//	if (Action)
	//	{
	//		// AddAdditionalAssetBundleData()는 UGameFeatureAction의 메서드이다
	//		// - 우리가 임의적으로 호출을 통해 AssetBundleData에 추가해준다
	//		Action->AddAdditionalAssetBundleData(AssetBundleData);

	//	}
	//}

}
#endif