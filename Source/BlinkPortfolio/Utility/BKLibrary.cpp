// Fill out your copyright notice in the Description page of Project Settings.
#include "BlinkPortfolio/Utility/BKLibrary.h"

#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "BlinkPortfolio/System/BKGameInstance.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

TWeakObjectPtr<UBKGameInstance> BKLib::GetGameInstance()
{
	return UBKGameInstance::Get();
}

void BKLib::ClearTimer(FTimerHandle& InTimer)
{
	TWeakObjectPtr<UGameInstance> pGameInstance = GetGameInstance();
	if (pGameInstance.IsValid() == false || InTimer.IsValid())
		return;

	pGameInstance->GetTimerManager().ClearTimer(InTimer);
}

bool BKLib::ReRegisterTimer_Lambda(FTimerHandle& InHandle, TFunction<void()> InFunc, float InRate, bool bInLoop)
{
	TWeakObjectPtr<UGameInstance> pGameInstance = GetGameInstance();
	if (pGameInstance.IsValid() == false)
		return false;

	pGameInstance->GetTimerManager().SetTimer(InHandle, [=]()
	{
		if (InFunc)
		{
			InFunc();
		}
	}, InRate, bInLoop);
	return true;
}

int32 BKLib::GetStackChildIndex(const UCommonActivatableWidgetStack* InWidgetStack, const UCommonActivatableWidget* InActivatableWidget)
{
	if (IsValid(InWidgetStack) == false || IsValid(InActivatableWidget) == false)
		return -1;

	for (auto iter =  InWidgetStack->GetWidgetList().CreateConstIterator(); iter; ++iter)
	{
		if ((*iter) == InActivatableWidget)
			return iter.GetIndex();
	}
	
	return -1;
}

void BKLib::SetVisible(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide)
{
	if (InWidget == nullptr)
		return;
	
	ESlateVisibility WidgetVisibility = bInVisible ? ESlateVisibility::Visible : bHiddenWhenHide ? ESlateVisibility::Hidden : ESlateVisibility::Collapsed;
	InWidget->SetVisibility(WidgetVisibility);
}

void BKLib::SetSelfHitTest(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide)
{
	if (InWidget == nullptr)
		return;
	
	ESlateVisibility WidgetVisibility = bInVisible ? ESlateVisibility::SelfHitTestInvisible : bHiddenWhenHide ? ESlateVisibility::Hidden : ESlateVisibility::Collapsed;
	InWidget->SetVisibility(WidgetVisibility);
}

void BKLib::SetHitTest(UWidget* InWidget, bool bInVisible, bool bHiddenWhenHide)
{
	if (InWidget == nullptr)
		return;
	
	ESlateVisibility WidgetVisibility = bInVisible ? ESlateVisibility::HitTestInvisible : bHiddenWhenHide ? ESlateVisibility::Hidden : ESlateVisibility::Collapsed;
	InWidget->SetVisibility(WidgetVisibility);
}

FVector2D BKLib::GetWidgetScreenPosition(const UWidget* InWidget, const FVector2D& InNormalCoord)
{
	if (IsValid(InWidget) == false)
		return FVector2D::ZeroVector;

	auto AbsPos = InWidget->GetCachedGeometry().GetAbsolutePositionAtCoordinates(InNormalCoord);
	auto Geometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(InWidget->GetWorld());
	auto ScreenPos = Geometry.AbsoluteToLocal(AbsPos);

	return ScreenPos;
}

FSlateRect BKLib::GetWidgetScreenRect(const UWidget* InWidget)
{
	if (IsValid(InWidget) == false)
		return FSlateRect();

	FVector2D StartPos = GetWidgetScreenPosition(InWidget, FVector2D::ZeroVector);
	FVector2D EndPos = GetWidgetScreenPosition(InWidget, FVector2D::UnitVector);

	return FSlateRect(StartPos, EndPos);
}

FSlateRect BKLib::GetWidgetRectAtPosition(const UWidget* InWidget, const FVector2D& InPosition)
{
	if (IsValid(InWidget) == false)
		return FSlateRect();
	
	FSlateRect RetRect;
	
	FVector2D DesiredSize = InWidget->GetDesiredSize();
	TWeakObjectPtr<UCanvasPanelSlot> CanvasSlot = Cast<UCanvasPanelSlot>(InWidget->Slot);
	if (CanvasSlot.IsValid())
	{
		RetRect = FSlateRect
		(
			-CanvasSlot->GetAlignment().X * DesiredSize.X,
			-CanvasSlot->GetAlignment().Y * DesiredSize.Y,
			(1-CanvasSlot->GetAlignment().X) * DesiredSize.X,
			(1-CanvasSlot->GetAlignment().Y) * DesiredSize.Y
		);
	}

	RetRect.Left = InPosition.X;
	RetRect.Right = InPosition.X;
	RetRect.Top = InPosition.Y;
	RetRect.Bottom = InPosition.Y;
	
	return RetRect;	
}

FVector2D BKLib::GetAdjustedWidgetPosition(const UWidget* InWidget, const FVector2D& InPosition, const FSlateRect& InContainerRect)
{
	FVector2D RetPos = InPosition;
	if (IsValid(InWidget) == false)
		return RetPos;

	FSlateRect WidgetRect = GetWidgetRectAtPosition(InWidget, InPosition);
	bool bNeedAdjust = WidgetRect != WidgetRect.IntersectionWith(InContainerRect);
	if (bNeedAdjust)
	{
		if (WidgetRect.Left < InContainerRect.Left)
			RetPos.X = InContainerRect.Left - WidgetRect.Left;
		
		else if (WidgetRect.Right > InContainerRect.Right)
			RetPos.X = WidgetRect.Right - InContainerRect.Right;

		if (WidgetRect.Top < InContainerRect.Top)
			RetPos.Y = InContainerRect.Top - WidgetRect.Top;

		else if (WidgetRect.Bottom > InContainerRect.Bottom)
			RetPos.Y = WidgetRect.Bottom - InContainerRect.Bottom;
	}

	return RetPos;
}

float BKLib::GetViewPortScale()
{
	TWeakObjectPtr pGameInstance = UBKGameInstance::Get();
	if (pGameInstance.IsValid() == false)
		return 1.0f;

	TWeakObjectPtr pWorld = pGameInstance->GetWorld();
	if (pWorld.IsValid() == false)
		return 1.0f;

	return UWidgetLayoutLibrary::GetViewportScale(pWorld.Get());
}

FVector2D BKLib::GetViewPortSize()
{
	TWeakObjectPtr pGameInstance = UBKGameInstance::Get();
	if (pGameInstance.IsValid() == false)
		return FVector2D::ZeroVector;

	TWeakObjectPtr<UWorld> pWorld = pGameInstance->GetWorld();
	if (pWorld.IsValid() == false)
		return FVector2D::ZeroVector;
	
	TWeakObjectPtr<UGameViewportClient> pGameViewport = pWorld->GetGameViewport();
	if (pGameViewport.IsValid() == false)
		return FVector2D::ZeroVector;

	FVector2D ViewportSize;
	pGameViewport->GetViewportSize(ViewportSize);
	return ViewportSize;
}

FVector2D BKLib::GetScreenSize()
{
	return GetViewPortSize() / GetViewPortScale();
}

FSlateRect BKLib::GetScreenRect()
{
	FVector2D ScreenSize = GetScreenSize();
	FSlateRect RetRect = FSlateRect(0, 0, ScreenSize.X, ScreenSize.Y);
	return RetRect;
}

void BKLib::SetUIItemIcon(UImage* InImage, int32 InItemID)
{
	// todo : 여기에 아이템 아이콘 설정 기능 추가.	
}

FText BKLib::GetUIItemName(int32 InItemID)
{
	// todo : 여기에 아이템 이름 반환 기능 추가.
	return FText::GetEmpty();
}

void BKLib::SetUIItemName(UTextBlock* InTextBlock, int32 InItemID)
{
	if (IsValid(InTextBlock) == false)
		return;

	InTextBlock->SetText(GetUIItemName(InItemID));
}
