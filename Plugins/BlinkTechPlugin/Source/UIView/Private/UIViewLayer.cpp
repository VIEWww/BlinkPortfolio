// Fill out your copyright notice in the Description page of Project Settings.


#include "UIViewLayer.h"

#include "UIView.h"
#include "UIViewLayerPanel.h"
#include "UIViewBase.h"
#include "UIViewSettings.h"
#include "UIViewSystem.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UUIViewLayer::NativeConstruct()
{
	Super::NativeConstruct();

	LoadLayerPanelMap();
}

void UUIViewLayer::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UUIViewLayer::NativeDestruct()
{
	Super::NativeDestruct();

	UUIViewSystem::Get()->OnDestructViewLayer();
}

void UUIViewLayer::LoadLayerPanelMap()
{
	TWeakObjectPtr<UUIViewSystem> UIViewSystem = UUIViewSystem::Get();

	LayerPanelMap.Reset();
	for (auto CurLayerInfo : UIViewSystem->GetAllLayerInfo())
	{
		FName LayerName = CurLayerInfo.Key;
		TWeakObjectPtr<UUIViewLayerPanel> CurStackLayer = Cast<UUIViewLayerPanel>(GetWidgetFromName(LayerName));
		if (CurStackLayer.IsValid())
		{
			TWeakObjectPtr<UCanvasPanelSlot> CurPanelSlot = Cast<UCanvasPanelSlot>(CurStackLayer->Slot);
			if (CurPanelSlot.IsValid())
			{
				CurPanelSlot->SetZOrder(CurLayerInfo.Value.LayerOrder);
			}
			LayerPanelMap.Add(LayerName, CurStackLayer.Get());
		}
	}
}

UUIViewLayerPanel* UUIViewLayer::GetLayerPanel(const FName& InLayerName) const
{
	return LayerPanelMap.FindRef(InLayerName);
}


void UUIViewLayer::ShowWidgetInLayer(UClass* InWidgetClass, TFunction<void(TWeakObjectPtr<UUIViewBase> InWidget)> InFunction)
{
	FName LayerName = UUIViewSystem::Get()->DetermineLayerName(InWidgetClass);
	TWeakObjectPtr<UUIViewLayerPanel> LayerWidgetStack = GetLayerPanel(LayerName);
	if (LayerWidgetStack.IsValid() == false)
		return;

	TWeakObjectPtr<UUIViewBase> CurActiveWidget = Cast<UUIViewBase>(LayerWidgetStack->GetTopWidget());
	if (CurActiveWidget.IsValid() && CurActiveWidget->IsUsingWidget())
	{
		// [1]. 현재 보여지고 있는 위젯이 있는 경우, 해당 위젯의 HideAnimation을 재생하고, 애니메이션이 완료된 이후에 새 UI를 출력해준다.
		TWeakObjectPtr<UUIViewLayer> WeakThis = this;
		CurActiveWidget->OnHideFromViewLayer([WeakThis, InWidgetClass, InFunction]()
		{
			if (WeakThis.IsValid() == false)
				return;

			WeakThis->AddWidgetInLayer(InWidgetClass, InFunction);
		});
		return;
	}

	// [2]. 현재 보여지고 있는 위젯이 없는 경우, 즉시 새 UI를 출력.
	AddWidgetInLayer(InWidgetClass, InFunction);
}

void UUIViewLayer::HideWidgetInLayer(TWeakObjectPtr<UUIViewBase> InHideWidget, TFunction<void()> InFunction)
{
	if (InHideWidget.IsValid() == false)
		return;
	
	// 이전에 보여지던 View가 있는 경우, 해당 View에 BringBack 플래그 활성화.
	SetFlagBringBackPrevView(InHideWidget);

	InHideWidget->OnHideFromViewLayer([InFunction, InHideWidget]()
	{
		if (InFunction != nullptr)
			InFunction();
	});
}

void UUIViewLayer::CleanUpLayer(const FName& InLayerName)
{
	if (LayerPanelMap.Contains(InLayerName) == false)
		return;

	TWeakObjectPtr<UUIViewLayerPanel> LayerStack = LayerPanelMap.FindChecked(InLayerName);
	if (LayerStack.IsValid() == false)
		return;

	LayerStack->ClearWidgets();
}

void UUIViewLayer::CleanUpLayerByExcludingView(const TWeakObjectPtr<UUIViewBase> InExcludingView)
{
	FName InLayerName = UUIViewSystem::Get()->DetermineLayerName(InExcludingView);
	if (LayerPanelMap.Contains(InLayerName) == false)
		return;

	TWeakObjectPtr<UUIViewLayerPanel> LayerStack = LayerPanelMap.FindChecked(InLayerName);
	if (LayerStack.IsValid() == false)
		return;

	const TArray<UUIViewBase*> WidgetList = LayerStack->GetWidgetList();
	for (auto &Widget : WidgetList)
	{
		if (Widget == InExcludingView)
			continue;

		LayerStack->RemoveWidget(Widget);
	}
}

void UUIViewLayer::AddWidgetInLayer(UClass* InLayerWidgetClass, TFunction<void(TWeakObjectPtr<UUIViewBase> InWidget)> InFunction)
{
	if (IsValid(InLayerWidgetClass) == false)
		return;

	FName InLayerName = UUIViewSystem::Get()->DetermineLayerName(InLayerWidgetClass);
	TWeakObjectPtr<UUIViewLayerPanel> LayerPanel = GetLayerPanel(InLayerName);
	if (LayerPanel.IsValid() == false)
		return;

	LayerPanel->AddWidget(InLayerWidgetClass, [InFunction](TWeakObjectPtr<UUIViewBase> InWidget)
	{
		if (InWidget.IsValid() == false)
			return;
		
		if (InFunction)
			InFunction(InWidget);

		InWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	});
}

void UUIViewLayer::RemoveWidgetInLayer(TWeakObjectPtr<UUIViewBase> InHideWidget)
{
	if (InHideWidget.IsValid() == false)
		return;
	
	FName InLayerName = UUIViewSystem::Get()->DetermineLayerName(InHideWidget);
	TWeakObjectPtr<UUIViewLayerPanel> LayerPanel = GetLayerPanel(InLayerName);
	if (LayerPanel.IsValid() == false)
		return;
	
	LayerPanel->RemoveWidget(InHideWidget.Get());
}

void UUIViewLayer::SetFlagBringBackPrevView(TWeakObjectPtr<UUIViewBase> InWidget)
{
	if (InWidget.IsValid() == false)
		return;
	
	FName InLayerName = UUIViewSystem::Get()->DetermineLayerName(InWidget);
	TWeakObjectPtr<UUIViewLayerPanel> LayerPanel = GetLayerPanel(InLayerName);
	if (LayerPanel.IsValid() == false)
		return;
	
	const TArray<UUIViewBase*> WidgetList = LayerPanel->GetWidgetList();
	if (WidgetList.Num() <= 0 || InWidget != WidgetList.Last())
		return;
	
	int32 BringBackWidgetIndex = WidgetList.Num() - 2;
	if (WidgetList.IsValidIndex(BringBackWidgetIndex) == false)
		return;

	TWeakObjectPtr<UUIViewBase> BringBackWidgetPtr = Cast<UUIViewBase>(WidgetList[BringBackWidgetIndex]);
	if (BringBackWidgetPtr.IsValid() == false)
		return;
	
	BringBackWidgetPtr->SetBringBackFlag(true);
}

TArray<TWeakObjectPtr<UUIViewBase>> UUIViewLayer::GetAllStackedViews(UClass* InWidgetClass) const
{
	TArray<TWeakObjectPtr<UUIViewBase>> RetViews;
	if (InWidgetClass == nullptr)
		return RetViews;
	
	FName InLayerName = UUIViewSystem::Get()->DetermineLayerName(InWidgetClass);
	TWeakObjectPtr<UUIViewLayerPanel> LayerPanel = GetLayerPanel(InLayerName);
	if (LayerPanel.IsValid() == false)
		return RetViews;

	
	FName InWidgetName = InWidgetClass->GetFName();
	for (UUIViewBase* CurWidget : LayerPanel->GetWidgetList())
	{
		if (IsValid(CurWidget) == false)
			continue;

		UClass* CurParentClass = GetParentNativeClass(CurWidget->GetClass());
		if (IsValid(CurParentClass) == false)
			continue;
		
		FName CurViewName = CurParentClass->GetFName();
		if (CurViewName == InWidgetName)
			RetViews.Add(CurWidget);
	}
	
	return RetViews;
}