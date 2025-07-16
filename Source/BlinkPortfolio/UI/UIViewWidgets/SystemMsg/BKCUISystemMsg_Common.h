// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUISystemMsgBase.h"
#include "BKCUISystemMsg_Common.generated.h"

class UCommonTextBlock;
/**
 * 
 */
class UWidgetAnimation;

UCLASS()
class BLINKPORTFOLIO_API UBKCUISystemMsg_Common : public UBKCUISystemMsgBase
{
	GENERATED_BODY()

public:
	static void Show(const FText& InMsg, float InCustomHoldTime = -1.0f);
	static void Show(const FString& InMsg, float InCustomHoldTime = -1.0f);

protected:
	virtual void NativeConstruct() override;

public:
	void RefreshUI();

	void PlayMsgShowAnim();
	void PlayMsgHideAnim();

	UFUNCTION()
	void OnAnimFinished_Show();
	UFUNCTION()
	void OnAnimFinished_Hide();

	void SetMsgText(const FText& InMsg) { MsgText = InMsg; RefreshUI(); }

protected:
	FTimerHandle TimerHandle_Hold;
	
	UPROPERTY(EditAnywhere, Category=SystemMsg)
	float MsgHoldTime = 3.0f;
	UPROPERTY(VisibleAnywhere, Category=SystemMsg)
	FText MsgText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Text_Msg = nullptr;

	UPROPERTY(meta = (BindWidgetAnimOptional), Transient)
	UWidgetAnimation* UIAnim_ShowSystemMsg = nullptr;
	UPROPERTY(meta = (BindWidgetAnimOptional), Transient)
	UWidgetAnimation* UIAnim_HideSystemMsg = nullptr;
};
