// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUIPageBase.h"
#include "BKCUIPage_SubMenu.generated.h"

/**
 * 
 */
class UCommonTextBlock;
class UCommonButtonBase;

UCLASS()
class BLINKPORTFOLIO_API UBKCUIPage_SubMenu : public UBKCUIPageBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void OnViewActivated() override;

	virtual void OnDisplayedWidgetChangedFromStack() override;

	virtual void OnShow() override;

	UFUNCTION()
	void OnButtonClicked_ShowMainMenu();

	UFUNCTION()
	void OnButtonClicked_ReplaceMainMenu();

	UFUNCTION()
	void OnButtonClicked_ShowSystemMsg();

public:
	void RefreshTitle();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* Text_Title = nullptr; 
	
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_ShowMainMenu = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
    UCommonButtonBase* CButton_ReplaceMainMenu = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_ShowSystemMsg = nullptr;
};
