// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUIHUDBase.h"
#include "BKCUIHUD_InGameMain.generated.h"

/**
 * 
 */
class UCommonButtonBase;

UCLASS()
class BLINKPORTFOLIO_API UBKCUIHUD_InGameMain : public UBKCUIHUDBase
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	virtual void OnViewActivated() override;

	UFUNCTION()
	void OnButtonClicked_Menu();


public:
	void RefreshUI();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonButtonBase* CButton_Menu = nullptr;
};
