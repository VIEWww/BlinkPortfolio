// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUIPopupBase.h"
#include "BKCUIPopup_Common.generated.h"

/**
 * 
 */
class UCommonButtonBase;

UCLASS()
class BLINKPORTFOLIO_API UBKCUIPopup_Common : public UBKCUIPopupBase
{
	GENERATED_BODY()

public:
	UBKCUIPopup_Common();
	
	static void Show(TFunction<void()> InOkCallback, TFunction<void()> InCancelCallback = nullptr);
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked_Ok();

	UFUNCTION()
	void OnButtonClicked_Cancel();

public:
	TFunction<void()> OkCallback = nullptr;
	TFunction<void()> CancelCallback = nullptr;

protected:
	
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_Ok = nullptr;
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_Cancel = nullptr;
};
