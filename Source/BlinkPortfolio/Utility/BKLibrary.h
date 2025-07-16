// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
class UBKGameSingleton;
class UBKGameInstance;
class UBKGameMode;
class UTextBlock;
class UImage;

#define ENUM_TO_STRING(ENUM_TYPE, ENUM_VALUE) BKLib::TEnumToString<ENUM_TYPE>(#ENUM_TYPE, ENUM_VALUE, FString(TEXT("InValid")))
#define STRING_TO_ENUM(ENUM_TYPE, STRING_VALUE) BKLib::TStringToEnum<ENUM_TYPE>(#ENUM_TYPE, STRING_VALUE)

#define UI_BIND_BUTTON_CLICK_EVENT(BUTTON, DELEGATE) if(IsValid(BUTTON)) { BUTTON->OnClicked.Clear(); BUTTON->OnClicked.AddDynamic(this, DELEGATE); }

#define UI_BIND_COMMONBUTTON_CLICK_EVENT(BUTTON, DELEGATE) if(IsValid(BUTTON)) { BUTTON->OnClicked().Clear(); BUTTON->OnClicked().AddUObject(this, DELEGATE); } 

#define UI_BIND_ANIM_STRATED(ANIM, DELEGATE) if(IsValid(ANIM)) \
	{\
		FWidgetAnimationDynamicEvent AnimStartEvent;\
		AnimStartEvent.BindDynamic(this, DELEGATE);\
		BindToAnimationStarted(ANIM, AnimStartEvent);\
	}\

#define UI_BIND_ANIM_FINISHED(ANIM, DELEGATE) if(IsValid(ANIM)) \
	{\
		FWidgetAnimationDynamicEvent AnimFinishEvent;\
		AnimFinishEvent.BindDynamic(this, DELEGATE);\
		BindToAnimationFinished(ANIM, AnimFinishEvent);\
	}\

namespace BKLib
{
	// 게임 시스템 관련 ========================================================================================================================

	TWeakObjectPtr<UBKGameInstance> GetGameInstance();

	void ClearTimer(FTimerHandle& InTimer);
	bool ReRegisterTimer_Lambda(FTimerHandle& InTimer, TFunction<void()> InFunc, float InRate, bool InbLoop = false);


	// Common UI View Layer 관련 ========================================================================================================================
	int32 GetStackChildIndex(const UCommonActivatableWidgetStack* InWidgetStack, const UCommonActivatableWidget* InActivatableWidget);
	
	
	// String 관련 ========================================================================================================================
	
	template<typename T>
	FString TEnumToString(const FString& InEnumName, const T& InEnumValue, const FString& InDefaultValue)
	{
		UEnum* EnumPtr = FindFirstObjectSafe<UEnum>(*InEnumName);
		return *(IsValid(EnumPtr) ? EnumPtr->GetNameStringByValue(static_cast<uint32>(InEnumValue)) : InDefaultValue);
	}
	template<typename T>
	T TStringToEnum(const FString& InEnumName, const FString& InStringValue)
	{
		UEnum* EnumPtr = FindFirstObjectSafe<UEnum>(*InEnumName);
		return T(IsValid(EnumPtr) ? EnumPtr->GetValueByNameString(InStringValue) : INDEX_NONE);
	}


	// 위젯 컨트롤 관련 ========================================================================================================================

	void SetVisible(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide = false);
	void SetSelfHitTest(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide = false);
	void SetHitTest(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide = false);
	
	
	// 위젯 트랜스 폼 관련 ========================================================================================================================

	// 위젯의 ScreenPosition Get.
	FVector2D GetWidgetScreenPosition(const UWidget* InWidget, const FVector2D& InNormalCoord);

	// 위젯의 ScreenRect Get.
	FSlateRect GetWidgetScreenRect(const UWidget* InWidget);

	// 위젯의 ScreenPosition을 반영한 ScreenRect Get.
	FSlateRect GetWidgetRectAtPosition(const UWidget* InWidget, const FVector2D& InPosition);

	// 위젯의 일부가 ViewPort 범위를 벗어난 경우, ViewPort 안으로 들어올 수 있게끔 조정된 위치를 Get. 
	FVector2D GetAdjustedWidgetPosition(const UWidget* InWidget, const FVector2D& InPosition, const FSlateRect& InContainerRect);


	// 뷰포트 및 스크린 관련 ========================================================================================================================

	//
	float GetViewPortScale();

	FVector2D GetViewPortSize();
	
	FVector2D GetScreenSize();
	
	FSlateRect GetScreenRect();
	

	// 인게임 UI 세팅 관련 ========================================================================================================================
	
	void SetUIItemIcon(UImage* InImage, int32 InItemID);

	FText GetUIItemName(int32 InItemID);
	void SetUIItemName(UTextBlock* InTextBlock, int32 InItemID);
}
