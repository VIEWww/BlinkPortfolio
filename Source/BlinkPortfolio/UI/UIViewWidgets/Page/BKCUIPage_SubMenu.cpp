// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/Page/BKCUIPage_SubMenu.h"

#include "CommonButtonBase.h"
#include "BKCUIPage_MainMenu.h"
#include "CommonTextBlock.h"
#include "UIViewLayerPanel.h"
#include "BlinkPortfolio/Utility/BKLibrary.h"
#include "UIViewSystem.h"
#include "BlinkPortfolio/UI/UIViewWidgets/SystemMsg/BKCUISystemMsg_Common.h"


void UBKCUIPage_SubMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_ShowMainMenu, &UBKCUIPage_SubMenu::OnButtonClicked_ShowMainMenu);
	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_ReplaceMainMenu, &UBKCUIPage_SubMenu::OnButtonClicked_ReplaceMainMenu);
	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_ShowSystemMsg, &UBKCUIPage_SubMenu::OnButtonClicked_ShowSystemMsg);
}

void UBKCUIPage_SubMenu::OnViewActivated()
{
	Super::OnViewActivated();

	
}

void UBKCUIPage_SubMenu::OnDisplayedWidgetChangedFromStack()
{
	Super::OnDisplayedWidgetChangedFromStack();
	
	RefreshTitle();
}

void UBKCUIPage_SubMenu::OnShow()
{
	Super::OnShow();

	if (GetLayerPanelType() == EUIViewLayerPanelType::Overlay)
	{
		RefreshTitle();
	}
}

void UBKCUIPage_SubMenu::OnButtonClicked_ShowMainMenu()
{
	if (IsTouchBlock())
		return;
	
	UBKUIViewSystem::Show<UBKCUIPage_MainMenu>();
}

void UBKCUIPage_SubMenu::OnButtonClicked_ReplaceMainMenu()
{
	if (IsTouchBlock())
		return;
	
	UBKUIViewSystem::ReplaceView<UBKCUIPage_MainMenu>();
}

void UBKCUIPage_SubMenu::OnButtonClicked_ShowSystemMsg()
{
	if (IsTouchBlock())
		return;
	
	UBKCUISystemMsg_Common::Show(TEXT("이 메시지는 테스트용 시스템 메시지 입니다."));
}

void UBKCUIPage_SubMenu::RefreshTitle()
{
	if (IsValid(Text_Title))
	{
		int32 ObjectNum = UBKUIViewSystem::Get()->GetAllStackedViews<UBKCUIPage_SubMenu>().Num();
		FString TitleString = FString::Printf(TEXT("Page_SubMenu\nStack Index : %d"), ObjectNum);

		Text_Title->SetText(FText::FromString(TitleString));
	}
}

