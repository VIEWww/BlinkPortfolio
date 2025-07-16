// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/HUD/BKCUIHUD_InGameMain.h"

#include "CommonButtonBase.h"
#include "BlinkPortfolio/UI/UIViewWidgets/Page/BKCUIPage_MainMenu.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BlinkPortfolio/Utility/BKLibrary.h"

void UBKCUIHUD_InGameMain::NativeConstruct()
{
	Super::NativeConstruct();

	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_Menu, &UBKCUIHUD_InGameMain::OnButtonClicked_Menu);
}

void UBKCUIHUD_InGameMain::OnViewActivated()
{
	Super::OnViewActivated();
	
	SetConsumePointerInput(false);
}

void UBKCUIHUD_InGameMain::OnButtonClicked_Menu()
{
	UBKUIViewSystem::ReplaceView<UBKCUIPage_MainMenu>();
}

void UBKCUIHUD_InGameMain::RefreshUI()
{
}
