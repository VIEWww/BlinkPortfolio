// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"


#define ENUM_TO_STRING(ENUM_TYPE, ENUM_VALUE) SJLib::TEnumToString<ENUM_TYPE>(#ENUM_TYPE, ENUM_VALUE, FString(TEXT("InValid")))
#define STRING_TO_ENUM(ENUM_TYPE, STRING_VALUE) SJLib::TStringToEnum<ENUM_TYPE>(#ENUM_TYPE, STRING_VALUE)

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

	BLINKTECHPLUGIN_API void SetVisible(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide = false);
	BLINKTECHPLUGIN_API void SetSelfHitTest(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide = false);
	BLINKTECHPLUGIN_API void SetHitTest(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide = false);
	
	
	// 위젯 트랜스 폼 관련 ========================================================================================================================

	// 위젯의 ScreenPosition Get.
	BLINKTECHPLUGIN_API FVector2D GetWidgetScreenPosition(const UWidget* InWidget, const FVector2D& InNormalCoord);

	// 위젯의 ScreenRect Get.
	BLINKTECHPLUGIN_API FSlateRect GetWidgetScreenRect(const UWidget* InWidget);

	// 위젯의 ScreenPosition을 반영한 ScreenRect Get.
	BLINKTECHPLUGIN_API FSlateRect GetWidgetRectAtPosition(const UWidget* InWidget, const FVector2D& InPosition);

	// 위젯의 일부가 ViewPort 범위를 벗어난 경우, ViewPort 안으로 들어올 수 있게끔 조정된 위치를 Get. 
	BLINKTECHPLUGIN_API FVector2D GetAdjustedWidgetPosition(const UWidget* InWidget, const FVector2D& InPosition, const FSlateRect& InContainerRect);


	// 뷰포트 및 스크린 관련 ========================================================================================================================

	BLINKTECHPLUGIN_API float GetViewPortScale(UObject* WorldContextObject);

	BLINKTECHPLUGIN_API FVector2D GetViewPortSize(UObject* WorldContextObject);
	
	BLINKTECHPLUGIN_API FVector2D GetScreenSize(UObject* WorldContextObject);
	
	BLINKTECHPLUGIN_API FSlateRect GetScreenRect(UObject* WorldContextObject);
	
}
