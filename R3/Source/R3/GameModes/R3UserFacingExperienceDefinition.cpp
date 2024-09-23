#include "R3UserFacingExperienceDefinition.h"
#include "CommonSessionSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3UserFacingExperienceDefinition)

UCommonSession_HostSessionRequest* UR3UserFacingExperienceDefinition::CreateHostingRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

	// 잠깐 생각해보자
	// - UCommonSession_HostSessionRequest는 UObject로 생성해놓고, 앞에서 GC가 된다
	// - 해당 객체는 현재 프레임에서 사용되기 때문에, GC에 대한 염려가 없다.
	//	 만약 다음 프레임이든, 추가적인 프레임 상에서 해당 객체를 사용할 경우, LifeTime 관리가 필요하다
	//		- 그렇지 않으면 dangling이 나버린다!

	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

	return Result;
}
