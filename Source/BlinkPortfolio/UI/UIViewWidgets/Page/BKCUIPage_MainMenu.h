// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUIPageBase.h"
#include "BKCUIPage_MainMenu.generated.h"

/**
 * 
 */
class UCommonTextBlock;
class UCommonButtonBase;

UCLASS()
class BLINKPORTFOLIO_API UBKCUIPage_MainMenu : public UBKCUIPageBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void OnViewActivated() override;

	virtual void OnDisplayedWidgetChangedFromStack() override;

	virtual void OnShow() override;
	virtual void OnHide() override;
	
	UFUNCTION()
	void OnButtonClicked_SubMenu();

	UFUNCTION()
	void OnButtonClicked_Popup();

public:
	void RefreshTitle();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* Text_Title = nullptr; 
	
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_SubMenu = nullptr;
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_Popup = nullptr;
};
