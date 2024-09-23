#pragma once

#include "UObject/Object.h"
#include "UObject/UObjectGlobals.h"
#include "GameUIPolicy.generated.h"

class UPrimaryGameLayout;
class UCommonLocalPlayer;

USTRUCT()
struct FRootViewportLayoutInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
		TObjectPtr<ULocalPlayer> LocalPlayer = nullptr;

	UPROPERTY(Transient)
		TObjectPtr<UPrimaryGameLayout> RootLayout = nullptr;

	UPROPERTY(Transient)
		bool bAddedToViewport = false;

	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }

	FRootViewportLayoutInfo() {}
	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, UPrimaryGameLayout* InRootLayout, bool bIsInViewport)
		: LocalPlayer(InLocalPlayer)
		, RootLayout(InRootLayout)
		, bAddedToViewport(bIsInViewport)
	{}

};

/**
*  UGameUIPolicy가 Abstract라는 것만 기억하자
*/
UCLASS(Abstract, Blueprintable)
class COMMONGAME_API UGameUIPolicy : public UObject
{
	GENERATED_BODY()

public:
	UPrimaryGameLayout* GetRootLayout(const UCommonLocalPlayer* LocalPlayer) const;

	TSubclassOf<UPrimaryGameLayout> GetLayoutWidgetClass(UCommonLocalPlayer* LocalPlayer);
	void CreateLayoutWidget(UCommonLocalPlayer* LocalPlayer);

	void AddLayoutToViewport(UCommonLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout);
	void RemoveLayoutFromViewport(UCommonLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout);


	void NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer);

	/** LocalPlayer에 바인딩 된 UI의 Layout으로 생각하면 된다 (아직 의미를 모를수도 있는데, 하나씩 구현해보자. 어떤 느낌인지 와닫을 것이다) */
	UPROPERTY(EditAnywhere)
		TSoftClassPtr<UPrimaryGameLayout> LayoutClass;

	/** 보통 싱글 게임에서는 LocalPlayer->PrimaryGameLayout 하나만 있겠지만, 멀티플레이의 경우, 복수개가 가능하다. (리플레이라던가?) */
	UPROPERTY(Transient)
		TArray<FRootViewportLayoutInfo> RootViewportLayouts;


};