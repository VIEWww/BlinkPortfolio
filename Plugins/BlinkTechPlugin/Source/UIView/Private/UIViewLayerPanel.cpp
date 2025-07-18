// Fill out your copyright notice in the Description page of Project Settings.


#include "UIViewLayerPanel.h"

#include "UIViewSystem.h"
#include "UIViewBase.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UUIViewLayerPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Layer_Stack))
	{
		Layer_Stack->OnDisplayedWidgetChanged().AddUObject(this, &UUIViewLayerPanel::OnDisplayedWidgetChanged);
	}

	RefreshUI();
}

void UUIViewLayerPanel::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	RefreshUI();
}

void UUIViewLayerPanel::OnDisplayedWidgetChanged(UCommonActivatableWidget* AddedWidget)
{
	TWeakObjectPtr<UUIViewBase> CastedWidget = Cast<UUIViewBase>(AddedWidget);
	if (CastedWidget.IsValid())
	{
		CastedWidget->OnDisplayedWidgetChangedFromStack();
	}
}

void UUIViewLayerPanel::RefreshUI()
{
	if (IsValid(Switcher_Root) == false)
		return;
	
	switch (LayerPanelType)
	{
	case EUIViewLayerPanelType::CommonActivatableWidgetStack:
		Switcher_Root->SetActiveWidget(Layer_Stack);
		break;
		
	case EUIViewLayerPanelType::Overlay:
		Switcher_Root->SetActiveWidget(Layer_Overlay);
		break;
		
	default:
		ensure(TEXT("Error : Not Set Layer Panel Type. Please Set Your Layer Panel Type."));
		break;
	}
}

void UUIViewLayerPanel::ClearWidgets()
{
	switch (LayerPanelType)
	{
	case EUIViewLayerPanelType::CommonActivatableWidgetStack:
		Layer_Stack->ClearWidgets();
		break;
		
	case EUIViewLayerPanelType::Overlay:
		Layer_Overlay->ClearChildren();
		break;
		
	default:
		return;
	}
}

UUIViewBase* UUIViewLayerPanel::GetTopWidget() const
{
	switch (LayerPanelType)
	{
	case EUIViewLayerPanelType::CommonActivatableWidgetStack:
		return Cast<UUIViewBase>(Layer_Stack->GetActiveWidget());
		
	case EUIViewLayerPanelType::Overlay:
		{
			int32 LastChildIndex = Layer_Overlay->GetAllChildren().FindLastByPredicate([](UWidget* InWidget)->bool
			{
				TWeakObjectPtr<UUIViewBase> CurWidgetPtr = Cast<UUIViewBase>(InWidget);
				return CurWidgetPtr.IsValid();
			});
			return Cast<UUIViewBase>(Layer_Overlay->GetChildAt(LastChildIndex));
		}
		
	default:
		return nullptr;
	}
}

TArray<UUIViewBase*> UUIViewLayerPanel::GetWidgetList() const
{
	TArray<UUIViewBase*> WidgetList;

	switch (LayerPanelType)
	{
	case EUIViewLayerPanelType::CommonActivatableWidgetStack:
		for (auto &iter : Layer_Stack->GetWidgetList())
		{
			TWeakObjectPtr<UUIViewBase> CastedWidget = Cast<UUIViewBase>(iter);
			if (CastedWidget.IsValid())
				WidgetList.Add(CastedWidget.Get());
		}
		break;
		
	case EUIViewLayerPanelType::Overlay:
		for (auto &iter : Layer_Overlay->GetAllChildren())
		{
			TWeakObjectPtr<UUIViewBase> CastedWidget = Cast<UUIViewBase>(iter);
			if (CastedWidget.IsValid())
				WidgetList.Add(CastedWidget.Get());
		}
		break;
		
	default:
		return WidgetList;
	}
	
	return WidgetList;
}

void UUIViewLayerPanel::AddWidget(UClass* InLayerWidgetClass, TFunction<void(TWeakObjectPtr<UUIViewBase> InWidget)> InFunction)
{
	switch (LayerPanelType)
	{
	case EUIViewLayerPanelType::CommonActivatableWidgetStack:
		{
			if (IsValid(Layer_Stack) == false)
				return;

			Layer_Stack->AddWidget<UUIViewBase>(InLayerWidgetClass, [InFunction](UUIViewBase& InWidgetRef)
				{
					TWeakObjectPtr<UUIViewBase> InWidgetPtr = &InWidgetRef;
					InFunction(InWidgetPtr); // IsValid 체크는 InFunction 내부에서
				});
		}
		break;
		
	case EUIViewLayerPanelType::Overlay:
		{
			if (IsValid(Layer_Overlay) == false)
				return;
			
			UClass* InParentClass = GetParentNativeClass(InLayerWidgetClass);
			if (IsValid(InParentClass) == false)
				return;
			
			FName WidgetName = InParentClass->GetFName();
			TWeakObjectPtr<UUIViewBase> CreatedWidgetPtr = UUIViewSystem::Get()->GetOrCreateUIViewWidget(WidgetName);
			if (CreatedWidgetPtr.IsValid())
			{
				int32 ChildIndex = Layer_Overlay->GetChildIndex(CreatedWidgetPtr.Get());
				if (ChildIndex >= 0)
				{
					Layer_Overlay->RemoveChildAt(ChildIndex);
				}
				
				Layer_Overlay->AddChildToOverlay(CreatedWidgetPtr.Get());
				TWeakObjectPtr<UOverlaySlot> OverlaySlot = Cast<UOverlaySlot>(CreatedWidgetPtr->Slot);
				if (OverlaySlot.IsValid())
				{
					OverlaySlot->SetPadding(FMargin());
					OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
					OverlaySlot->SetVerticalAlignment(VAlign_Fill);
				}

				CreatedWidgetPtr->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				InFunction(CreatedWidgetPtr);
			}
		}
		break;
		
	default:
		break;
	}
	
	
}

void UUIViewLayerPanel::RemoveWidget(TWeakObjectPtr<UCommonActivatableWidget> InWidget)
{
	if (InWidget.IsValid() == false)
		return;
	
	switch (LayerPanelType)
    {
    case EUIViewLayerPanelType::CommonActivatableWidgetStack:
	    {
    		Layer_Stack->RemoveWidget(*InWidget.Get());
	    }
		break;
    	
    case EUIViewLayerPanelType::Overlay:
	    {
    		bool bNeedBringBack = InWidget == GetTopWidget();
    		
    		int32 ChildIndex = Layer_Overlay->GetChildIndex(InWidget.Get());
    		if (ChildIndex >= INDEX_NONE)
    		{
    			InWidget->SetVisibility(ESlateVisibility::Collapsed);
    			Layer_Overlay->RemoveChildAt(ChildIndex);
    		}

    		if (bNeedBringBack)
    		{
    			int32 BringBackWidgetIndex = ChildIndex - 1;
    			TWeakObjectPtr<UUIViewBase> BringBackWidget = Cast<UUIViewBase>(Layer_Overlay->GetChildAt(BringBackWidgetIndex));
    			if (BringBackWidget.IsValid())
    			{
    				BringBackWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    				BringBackWidget->OnBringBack();
    			}
    		}
	    }
    	break;
    	
    default:
    	return;
    }
	
}
