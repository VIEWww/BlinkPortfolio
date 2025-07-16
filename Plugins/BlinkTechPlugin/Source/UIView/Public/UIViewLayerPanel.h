// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UIViewLayerPanel.generated.h"


class UOverlay;
class UWidgetSwitcher;
class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

class UUIViewBase;

UENUM()
enum class EUIViewLayerPanelType : uint8
{
	CommonActivatableWidgetStack,
	Overlay,
};

UCLASS()
class UIVIEW_API UUIViewLayerPanel : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativePreConstruct() override;

	UFUNCTION()
	void OnDisplayedWidgetChanged(UCommonActivatableWidget* AddedWidget);

public:
	void RefreshUI();

	void ClearWidgets();
	
	UUIViewBase* GetTopWidget() const;
	TArray<UUIViewBase*> GetWidgetList() const;

	void AddWidget(UClass* InLayerWidgetClass, TFunction<void(TWeakObjectPtr<UUIViewBase> InWidget)> InFunction);
	void RemoveWidget(TWeakObjectPtr<UCommonActivatableWidget> InWidget);

	const EUIViewLayerPanelType& GetLayerPanelType() const { return LayerPanelType; }


protected:
	UPROPERTY(EditAnywhere, Category = UIViewLayer)
	EUIViewLayerPanelType LayerPanelType = EUIViewLayerPanelType::CommonActivatableWidgetStack;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* Switcher_Root = nullptr;
	UPROPERTY(meta = (BindWidget))
    UCommonActivatableWidgetStack* Layer_Stack = nullptr;
	UPROPERTY(meta = (BindWidget))
	UOverlay* Layer_Overlay = nullptr;
};