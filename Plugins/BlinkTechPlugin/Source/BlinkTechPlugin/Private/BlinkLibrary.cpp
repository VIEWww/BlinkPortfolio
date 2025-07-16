// Fill out your copyright notice in the Description page of Project Settings.
#include "BlinkLibrary.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"


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

float BKLib::GetViewPortScale(UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject) == false)
		return 1.0f;

	TWeakObjectPtr pWorld = WorldContextObject->GetWorld();
	if (pWorld.IsValid() == false)
		return 1.0f;

	return UWidgetLayoutLibrary::GetViewportScale(pWorld.Get());
}

FVector2D BKLib::GetViewPortSize(UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject) == false)
		return FVector2D::ZeroVector;

	TWeakObjectPtr<UWorld> pWorld = WorldContextObject->GetWorld();
	if (pWorld.IsValid() == false)
		return FVector2D::ZeroVector;
	
	TWeakObjectPtr<UGameViewportClient> pGameViewport = pWorld->GetGameViewport();
	if (pGameViewport.IsValid() == false)
		return FVector2D::ZeroVector;

	FVector2D ViewportSize;
	pGameViewport->GetViewportSize(ViewportSize);
	return ViewportSize;
}

FVector2D BKLib::GetScreenSize(UObject* WorldContextObject)
{
	return GetViewPortSize(WorldContextObject) / GetViewPortScale(WorldContextObject);
}

FSlateRect BKLib::GetScreenRect(UObject* WorldContextObject)
{
	FVector2D ScreenSize = GetScreenSize(WorldContextObject);
	FSlateRect RetRect = FSlateRect(0, 0, ScreenSize.X, ScreenSize.Y);
	return RetRect;
}