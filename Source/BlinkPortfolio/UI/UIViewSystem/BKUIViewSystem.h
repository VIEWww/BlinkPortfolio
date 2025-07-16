// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIViewSystem.h"
#include "Containers/LruCache.h"
#include "BKUIViewSystem.generated.h"

enum class EUIViewLayerType : uint8;
enum class EBKCUIViewLayerType : uint8;

struct FStreamableHandle;

class UBKCUIViewLayer;
class UUIViewBase;

UENUM()
enum class EBKCUIViewLayerType : uint8
{
	None = 0,

	HUD,				// 메인 HUD UI 레이어.
	Page,				// HUD 위에 화면 전체를 덮는 UI 레이어.
	Popup,				// 팝업 UI 레이어.
	Tooltip,			// 툴팁 UI 레이어.
	SystemMsg,			// 시스템 메시지 레이어.
	
	MAX,
	
	Begin = None + 1,
};

UCLASS()
class BLINKPORTFOLIO_API UBKUIViewSystem : public UUIViewSystem
{
	GENERATED_BODY()

protected:
	virtual void OnPostInstall() override;

public:
	static UBKUIViewSystem* Get();
	
	// 위젯 관련 편의성 함수 ======================================================================================================================================================================

	static FName GetLayerNameByType(const EBKCUIViewLayerType& InLayerType);
};
