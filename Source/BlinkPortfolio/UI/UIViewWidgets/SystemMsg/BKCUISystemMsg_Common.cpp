// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/SystemMsg/BKCUISystemMsg_Common.h"

#include "CommonTextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "BlinkPortfolio/Utility/BKLibrary.h"

void UBKCUISystemMsg_Common::Show(const FText& InMsg, float InCustomHoldTime)
{
	UBKUIViewSystem::ReplaceView<UBKCUISystemMsg_Common>([=](TWeakObjectPtr<UUIViewBase> InWidget)
	{
		TWeakObjectPtr<UBKCUISystemMsg_Common> InSystemMsg_Common = Cast<UBKCUISystemMsg_Common>(InWidget.Get());
		if (InSystemMsg_Common.IsValid())
		{
			InSystemMsg_Common->MsgHoldTime = InCustomHoldTime > 0.0f ? InCustomHoldTime : InSystemMsg_Common->MsgHoldTime;
			InSystemMsg_Common->SetMsgText(InMsg);
			InSystemMsg_Common->PlayMsgShowAnim();
		}
	});
}

void UBKCUISystemMsg_Common::Show(const FString& InMsg, float InCustomHoldTime)
{
	UBKUIViewSystem::ReplaceView<UBKCUISystemMsg_Common>([=](TWeakObjectPtr<UUIViewBase> InWidget)
	{
		TWeakObjectPtr<UBKCUISystemMsg_Common> InSystemMsg_Common = Cast<UBKCUISystemMsg_Common>(InWidget.Get());
		if (InSystemMsg_Common.IsValid())
		{
			InSystemMsg_Common->MsgHoldTime = InCustomHoldTime > 0.0f ? InCustomHoldTime : InSystemMsg_Common->MsgHoldTime;
			InSystemMsg_Common->SetMsgText(FText::FromString(InMsg));
			InSystemMsg_Common->PlayMsgShowAnim();
		}
	});
}

void UBKCUISystemMsg_Common::NativeConstruct()
{
	Super::NativeConstruct();

	UI_BIND_ANIM_FINISHED(UIAnim_ShowSystemMsg, &UBKCUISystemMsg_Common::OnAnimFinished_Show);
	UI_BIND_ANIM_FINISHED(UIAnim_HideSystemMsg, &UBKCUISystemMsg_Common::OnAnimFinished_Hide);
}

void UBKCUISystemMsg_Common::RefreshUI()
{
	if (IsValid(Text_Msg) == false)
		return;

	Text_Msg->SetText(MsgText);
}

void UBKCUISystemMsg_Common::PlayMsgShowAnim()
{
	BKLib::ClearTimer(TimerHandle_Hold);
	
	PlayAnimation(UIAnim_ShowSystemMsg);
}

void UBKCUISystemMsg_Common::PlayMsgHideAnim()
{
	PlayAnimation(UIAnim_HideSystemMsg);
}

void UBKCUISystemMsg_Common::OnAnimFinished_Show()
{
	bool bSuccessRegister = BKLib::ReRegisterTimer_Lambda(TimerHandle_Hold, [this]()
	{
		PlayMsgHideAnim();
	}, MsgHoldTime);

	if (bSuccessRegister == false)
	{
		ensure(TEXT("Error!!! : Failed to register timer."));
		Hide();
	}
}

void UBKCUISystemMsg_Common::OnAnimFinished_Hide()
{
	Hide();
}
