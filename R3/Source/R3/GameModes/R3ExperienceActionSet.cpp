#include "R3ExperienceActionSet.h"
#include "GameFeatureAction.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3ExperienceActionSet)

UR3ExperienceActionSet::UR3ExperienceActionSet()
	: Super()
{
}

#if WITH_EDITORONLY_DATA
void UR3ExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);

		}
	}

}
#endif