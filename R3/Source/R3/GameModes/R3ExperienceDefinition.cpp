#include "R3ExperienceDefinition.h"
#include "GameFeatureAction.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3ExperienceDefinition)

UR3ExperienceDefinition::UR3ExperienceDefinition(const FObjectInitializer& ObjectInitializer)
{
}

#if WITH_EDITORONLY_DATA
void UR3ExperienceDefinition::UpdateAssetBundleData()
{
	// �츮�� UpdateAssetBundleData() �ڵ带 �ѹ� �����Ѵ�
	Super::UpdateAssetBundleData();

	//for (UGameFeatureAction* Action : Actions)
	//{
	//	if (Action)
	//	{
	//		// AddAdditionalAssetBundleData()�� UGameFeatureAction�� �޼����̴�
	//		// - �츮�� ���������� ȣ���� ���� AssetBundleData�� �߰����ش�
	//		Action->AddAdditionalAssetBundleData(AssetBundleData);

	//	}
	//}

}
#endif