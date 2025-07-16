// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UIViewBase.generated.h"

enum class EUIViewLayerPanelType : uint8;

UINTERFACE()
class UIVIEW_API UIsSyncLoadUIWidget : public UInterface { GENERATED_BODY() };
class UIVIEW_API IIsSyncLoadUIWidget : public IInterface { GENERATED_BODY() };


UCLASS()
class UIVIEW_API UUIViewBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UUIViewBase() : LayerName(NAME_None) { }
	UUIViewBase(const FName& InLayerName) : LayerName(InLayerName) { }

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	virtual bool NativeOnHandleBackAction() override;

	// 위젯의 Visibility가 변경된 경우 호출.
	virtual void OnVisibleChanged(bool bInNewVisible);
	// 위젯이 Visible 상태로 바뀐 경우 호출.
	virtual void OnViewActivated();
	// 위젯이 InVisible 상태로 바뀐 경우 호출.
	virtual void OnViewDeactivated();


	// BackAction 수행을 할 수 있는 상태인지 체크 - 필요에 따라 파생 클래스에서 재정의하여 사용.
	virtual bool IsPossibleBackAction() { return true; }


	// Show 애니메이션 시작/종료 이벤트.
	UFUNCTION()
	virtual void OnShowAinmStarted();
	UFUNCTION()
	virtual void OnShowAinmFinished();

	// Hide 애니메이션 시작/종료 이벤트.
	UFUNCTION()
	virtual void OnHideAinmStarted();
	UFUNCTION()
	virtual void OnHideAinmFinished();


	// 로컬 영역 터치블록 활성화 제어 및 상태 체크.
	virtual void SetTouchBlock(bool bEnable);
	bool IsTouchBlock() const;
	
	virtual void OnShow();
	virtual void PlayShowAnimation();

	virtual void OnHide();
	virtual void PlayHideAnimation();
	
public:
	virtual void OnDisplayedWidgetChangedFromStack() {}
	
	virtual bool IsValidLayer();
	
	virtual bool IsClosable() const;
	virtual bool IsUsingWidget() const;
	virtual bool IsStackedWidget() const;

	virtual void OnShowFromViewLayer(TFunction<void()> InShowFinishedFunction);
	virtual void OnHideFromViewLayer(TFunction<void()> InHideFinishedFunction);

	virtual void CheckAndExcuteBringBack();
	virtual void OnBringBack();

	virtual const FName& GetLayerName() const { return LayerName; }
	virtual const EUIViewLayerPanelType GetLayerPanelType() const;

	// 해당 위젯이 활성화 중일 때 사용할 인풋 모드 Get.
	virtual bool ShouldUseInputMode() const;


protected:
	bool IsBoundAnimationEvent(const TWeakObjectPtr<UWidgetAnimation> InWidgetAnim, const FWidgetAnimationDynamicEvent& InWidgetAnimEvent, const EWidgetAnimationEvent& AnimEventType);

public:
	void Hide();

	bool IsPlayingHideAnim() const;
	bool IsPlayingShowAnim() const;
	
	FORCEINLINE void SetBringBackFlag(const bool& InFlag) { bBringBackFlag = InFlag; }

protected:
	bool bBringBackFlag = false;
	
	TFunction<void()> Callback_ShowFinished;
	TFunction<void()> Callback_HideFinished;

	// LayerName은 생성자에서 초기화 하는 것으로만 설정 가능하게 하여, RunTime 도중 레이어가 변경되는 것을 막고, CDO 생성 시점부터 값을 가질 수 있도록 구현.
	const FName LayerName;

	// 로컬 영역 터치 블록 위젯
	UPROPERTY(meta = (BindWidgetOptional), Transient)
	UWidget* Widget_TouchBlock = nullptr;

	// Show / Hide 애니메이션 재생중 터치 블록 활성화.
	UPROPERTY(EditAnywhere)
	bool bTouchBlockWhenShowAnimPlaying = true;
	UPROPERTY(EditAnywhere)
	bool bTouchBlockWhenHideAnimPlaying = true;

	// Show / Hide 애니메이션 종료 이벤트 - Finished만 따로 멤버변수로 관리하는 이유는 StopAnimation으로 애니메이션 중단 시 AnimFinished 콜백이 발생하는 문제를 대응하기 위함.
	UPROPERTY()
	FWidgetAnimationDynamicEvent ShowAnimFinishEvent;
	UPROPERTY()
	FWidgetAnimationDynamicEvent HideAnimFinishEvent;
	
	UPROPERTY(meta = (BindWidgetAnimOptional), Transient)
	UWidgetAnimation* UIAnim_Show = nullptr;
	UPROPERTY(meta = (BindWidgetAnimOptional), Transient)
	UWidgetAnimation* UIAnim_Hide = nullptr;
};
