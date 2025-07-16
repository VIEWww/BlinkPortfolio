// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIViewBase.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BKCUIPageBase.generated.h"

/**
 * 
 */
class UButton;
class UCommonButtonBase;

UCLASS()
class BLINKPORTFOLIO_API UBKCUIPageBase : public UUIViewBase
{
	GENERATED_BODY()
	
public:
	UBKCUIPageBase() : UUIViewBase(UBKUIViewSystem::GetLayerNameByType(EBKCUIViewLayerType::Page)) {}

protected:
	virtual void NativeConstruct() override;

	virtual void OnBringBack() override;

	virtual void OnClickClose();
	virtual void OnClickCloseAll();

private:
	UFUNCTION()
	void OnButtonClicked_Close();
	UFUNCTION()
	void OnButtonClicked_CloseAll();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_Close = nullptr;
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Button_Close = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_CloseAll = nullptr;
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Button_CloseAll = nullptr;
};
