// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUITooltipBase.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "BlinkPortfolio/Utility/BKLibrary.h"

void UBKCUITooltipBase::NativeConstruct()
{
	Super::NativeConstruct();

	Slot_Contents = nullptr;	
	if (IsValid(Panel_Contents))
	{
		Slot_Contents = Cast<UCanvasPanelSlot>(Panel_Contents->Slot);
		if (Slot_Contents.IsValid() == false)
		{
			ensure(TEXT("Error!!! : Panel_Root is not son of 'CanvasPanel'."));
		}
	}
}

void UBKCUITooltipBase::OnShow()
{
	Super::OnShow();
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UBKCUITooltipBase::OnUpdateLayout);
	}
}

void UBKCUITooltipBase::OnUpdateLayout()
{
	if (Slot_Contents.IsValid() == false || IsValid(Panel_Contents) == false)
		return;
	
	Slot_Contents->SetAlignment(ShowAlignment);
	
	auto AdjustPosition = BKLib::GetAdjustedWidgetPosition(Panel_Root, ShowPosition, BKLib::GetScreenRect());
	Panel_Contents->SetRenderTranslation(AdjustPosition);
	Panel_Contents->ForceLayoutPrepass();
}

void UBKCUITooltipBase::SetTransform(const FVector2D& InPosition, const FVector2D& InAlignment)
{
	ShowPosition = InPosition;
	ShowAlignment = InAlignment;
}
