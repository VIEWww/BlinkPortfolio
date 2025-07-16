// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/Page/BKCUIPage_MainMenu.h"

#include "BKCUIPage_SubMenu.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BlinkPortfolio/UI/UIViewWidgets/Popup/BKCUIPopup_Common.h"
#include "BlinkPortfolio/Utility/BKLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UBKCUIPage_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_SubMenu, &UBKCUIPage_MainMenu::OnButtonClicked_SubMenu);
	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_Popup, &UBKCUIPage_MainMenu::OnButtonClicked_Popup);
}

void UBKCUIPage_MainMenu::OnViewActivated()
{
	Super::OnViewActivated();

}

void UBKCUIPage_MainMenu::OnDisplayedWidgetChangedFromStack()
{
	Super::OnDisplayedWidgetChangedFromStack();

	if (IsValid(Text_Title))
	{
		int32 ObjectNum = UBKUIViewSystem::Get()->GetAllStackedViews<UBKCUIPage_MainMenu>().Num();
		FString TitleString = FString::Printf(TEXT("Page_MainMenu_%d"), ObjectNum);

		Text_Title->SetText(FText::FromString(TitleString));
	}
}

void UBKCUIPage_MainMenu::OnShow()
{
	Super::OnShow();

}

void UBKCUIPage_MainMenu::OnHide()
{
	Super::OnHide();

}

void UBKCUIPage_MainMenu::OnButtonClicked_SubMenu()
{
	if (IsTouchBlock())
		return;
	
	UBKUIViewSystem::ReplaceView<UBKCUIPage_SubMenu>();
}

void UBKCUIPage_MainMenu::OnButtonClicked_Popup()
{
	if (IsTouchBlock())
		return;
	
	auto OkCallback = [=]()
	{
		UWorld* pWorld = GetWorld();
		if (IsValid(pWorld) == false)
			return;

		APlayerController* pPlayerController = pWorld->GetFirstPlayerController();
		if (IsValid(pPlayerController) == false)
			return;

		UKismetSystemLibrary::QuitGame(pWorld, pPlayerController, EQuitPreference::Quit, true);
	};

	UBKCUIPopup_Common::Show(OkCallback);

	// UBKUIViewSystem::Show<UBKCUIPopup_Common>([OkCallback](TWeakObjectPtr<UUIViewBase> InShowWidget)
	// {
	// 	TWeakObjectPtr<UBKCUIPopup_Common> Popup_Common = Cast<UBKCUIPopup_Common>( InShowWidget.Get());
	// 	if (Popup_Common.IsValid())
	// 	{
	// 		Popup_Common->OkCallback = OkCallback;
	// 	}
	// });
}
