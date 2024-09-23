#include "R3UserFacingExperienceDefinition.h"
#include "CommonSessionSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(R3UserFacingExperienceDefinition)

UCommonSession_HostSessionRequest* UR3UserFacingExperienceDefinition::CreateHostingRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

	// ��� �����غ���
	// - UCommonSession_HostSessionRequest�� UObject�� �����س���, �տ��� GC�� �ȴ�
	// - �ش� ��ü�� ���� �����ӿ��� ���Ǳ� ������, GC�� ���� ������ ����.
	//	 ���� ���� �������̵�, �߰����� ������ �󿡼� �ش� ��ü�� ����� ���, LifeTime ������ �ʿ��ϴ�
	//		- �׷��� ������ dangling�� ��������!

	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

	return Result;
}
