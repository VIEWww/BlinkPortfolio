// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUIPageBase.h"

#include "CommonButtonBase.h"
#include "Components/Button.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BlinkPortfolio/Utility/BKLibrary.h"


void UBKCUIPageBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	UI_BIND_BUTTON_CLICK_EVENT(Button_Close, &UBKCUIPageBase::OnButtonClicked_Close);
	UI_BIND_BUTTON_CLICK_EVENT(Button_CloseAll, &UBKCUIPageBase::OnButtonClicked_CloseAll);

	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_Close, &UBKCUIPageBase::OnButtonClicked_Close);
	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_CloseAll, &UBKCUIPageBase::OnButtonClicked_CloseAll);
}

void UBKCUIPageBase::OnButtonClicked_Close()
{
	OnClickClose();
}

void UBKCUIPageBase::OnButtonClicked_CloseAll()
{
	OnClickCloseAll();
}

void UBKCUIPageBase::OnClickClose()
{
	Hide();
}

void UBKCUIPageBase::OnClickCloseAll()
{
	UBKUIViewSystem::Get()->CleanUpLayerByExcludingView(this);

	Hide();
}

void UBKCUIPageBase::OnBringBack()
{
	Super::OnBringBack();

	OnShow();
}