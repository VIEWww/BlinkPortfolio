// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIViewBase.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BKCUITooltipBase.generated.h"

class UPanelWidget;
class UCanvasPanel;
class UCanvasPanelSlot;
/**
 * 
 */
UCLASS()
class BLINKPORTFOLIO_API UBKCUITooltipBase : public UUIViewBase
{
	GENERATED_BODY()

public:
	UBKCUITooltipBase() : UUIViewBase(UBKUIViewSystem::GetLayerNameByType(EBKCUIViewLayerType::Tooltip)) {}

	template <typename T, typename = typename TEnableIf<TIsDerivedFrom<T, UBKCUITooltipBase>::Value>::Type>
	static void Show(const FVector2D& InPosition, const FVector2D& InAlignment = FVector2D(0.5f, 1.0f), TFunction<void(TWeakObjectPtr<T> InTooltipWidget)> InShowFunc = nullptr)
	{
		UBKUIViewSystem::Show<T>([=](auto View)
		{
			TWeakObjectPtr<T> CastedView = Cast<T>(View);
			if (CastedView.IsValid() == false)
				return;

			CastedView->SetTransform(InPosition, InAlignment);
			
			if (InShowFunc)
				InShowFunc(CastedView);
		});
	}

	template <typename T, typename = typename TEnableIf<TIsDerivedFrom<T, UBKCUITooltipBase>::Value>::Type>
	static void ReplaceView(const FVector2D& InPosition, const FVector2D& InAlignment = FVector2D(0.5f, 1.0f), TFunction<void(TWeakObjectPtr<T> InTooltipWidget)> InShowFunc = nullptr)
	{
		UBKUIViewSystem::ReplaceView<T>([=](auto View)
		{
			TWeakObjectPtr<T> CastedView = Cast<T>(View);
			if (CastedView.IsValid() == false)
				return;

			CastedView->SetTransform(InPosition, InAlignment);

			if (InShowFunc)
				InShowFunc(CastedView);
		});
	}

protected:
	virtual void NativeConstruct() override;
	
	virtual void OnShow() override;

	UFUNCTION()
	void OnUpdateLayout();

public:
	
	void SetTransform(const FVector2D& InPosition, const FVector2D& InAlignment);

protected:
	FVector2D ShowPosition = FVector2D::ZeroVector;
	
	FVector2D ShowAlignment = FVector2D::ZeroVector;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Panel_Root = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* Panel_Contents = nullptr;
	TWeakObjectPtr<UCanvasPanelSlot> Slot_Contents = nullptr;
}; 
