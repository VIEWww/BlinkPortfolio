// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UIViewLayer.generated.h"

class UCanvasPanel;

class UCommonActivatableWidget;
class UUIViewBase;
class UCommonActivatableWidgetStack;
class UUIViewLayerPanel;

enum class EUIViewLayerType : uint8;

UCLASS(meta = (DisableNativeTick), Abstract)
class UIVIEW_API UUIViewLayer : public UCommonUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

private:
	void LoadLayerPanelMap();

public:
	UUIViewLayerPanel* GetLayerPanel(const FName& InLayerType) const;
	
	void ShowWidgetInLayer(UClass* InWidgetClass, TFunction<void(TWeakObjectPtr<UUIViewBase> InWidget)> InFunction = nullptr);
	void HideWidgetInLayer(TWeakObjectPtr<UUIViewBase> InHideWidget, TFunction<void()> InFunction = nullptr);

	// 해당 레이어에 존재하는 모든 위젯 클리어.
	void CleanUpLayer(const FName& InLayerName);
	// 해당 위젯을 제외한, 레이어에 존재하는 모든 위젯 클리어.
	void CleanUpLayerByExcludingView(const TWeakObjectPtr<UUIViewBase> InExcludingView);

	// 레이어에 위젯 추가.
	void AddWidgetInLayer(UClass* InLayerWidgetClass, TFunction<void(TWeakObjectPtr<UUIViewBase> InWidget)> InFunction = nullptr);
	// 레이어에 위젯 삭제.
	void RemoveWidgetInLayer(TWeakObjectPtr<UUIViewBase> InHideWidget);
	
	// BringBack 플래그 설정.
	void SetFlagBringBackPrevView(TWeakObjectPtr<UUIViewBase> InWidget);


	TArray<TWeakObjectPtr<UUIViewBase>> GetAllStackedViews(UClass* InWidgetClass) const;

protected:
	UPROPERTY()
	TMap<FName, UUIViewLayerPanel*> LayerPanelMap;
};
