// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/Popup/BKCUIPopup_Common.h"

#include "CommonButtonBase.h"
#include "UIView.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"

UBKCUIPopup_Common::UBKCUIPopup_Common()
{
	OkCallback.Reset();
	CancelCallback.Reset();
}

void UBKCUIPopup_Common::Show(TFunction<void()> InOkCallback, TFunction<void()> InCancelCallback)
{
	UBKUIViewSystem::Show<UBKCUIPopup_Common>([=](TWeakObjectPtr<UUIViewBase> InShowWidget)
	{
		TWeakObjectPtr<UBKCUIPopup_Common> Popup_Common = Cast<UBKCUIPopup_Common>(InShowWidget.Get());
		if (Popup_Common.IsValid() == false)
			return;

		Popup_Common->OkCallback = InOkCallback;
		Popup_Common->CancelCallback = InCancelCallback;
	});
}

void UBKCUIPopup_Common::NativeConstruct()
{
	Super::NativeConstruct();

	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_Ok, &UBKCUIPopup_Common::OnButtonClicked_Ok);

	UI_BIND_COMMONBUTTON_CLICK_EVENT(CButton_Cancel, &UBKCUIPopup_Common::OnButtonClicked_Cancel);
}

void UBKCUIPopup_Common::OnButtonClicked_Ok()
{
	if (OkCallback)
		OkCallback();

	Hide();
}

void UBKCUIPopup_Common::OnButtonClicked_Cancel()
{
	if (CancelCallback)
		CancelCallback();
	
	Hide();
}
