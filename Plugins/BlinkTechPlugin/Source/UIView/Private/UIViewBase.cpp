// Fill out your copyright notice in the Description page of Project Settings.


#include "UIViewBase.h"

#include "UIView.h"
#include "UIViewLayerPanel.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "UIViewSystem.h"


class UImage;

void UUIViewBase::NativeConstruct()
{
	Super::NativeConstruct();

	UI_BIND_ANIM_STRATED(UIAnim_Show, &UUIViewBase::OnShowAinmStarted);
	UI_BIND_ANIM_STRATED(UIAnim_Hide, &UUIViewBase::OnHideAinmStarted);

	ShowAnimFinishEvent.BindDynamic(this, &UUIViewBase::OnShowAinmFinished);
	HideAnimFinishEvent.BindDynamic(this, &UUIViewBase::OnHideAinmFinished);
}

void UUIViewBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(Widget_TouchBlock))
	{
		Widget_TouchBlock->SetRenderOpacity(0.0f);
		Widget_TouchBlock->SetVisibility(ESlateVisibility::Collapsed);
		
		TWeakObjectPtr<UCanvasPanelSlot> PanelSlotPtr = Cast<UCanvasPanelSlot>(Widget_TouchBlock->Slot);
		if (PanelSlotPtr.IsValid())
		{
			PanelSlotPtr->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			PanelSlotPtr->SetOffsets(FMargin(0.0f, 0.0f));
			PanelSlotPtr->SetAlignment(FVector2D(0.0f, 0.0f));
			PanelSlotPtr->SetZOrder(99999999);
		}
	}
}

void UUIViewBase::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	CheckAndExcuteBringBack();

	OnVisibleChanged(true);
}

void UUIViewBase::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	OnVisibleChanged(false);
}

bool UUIViewBase::NativeOnHandleBackAction()
{
	return IsPossibleBackAction() ? Super::NativeOnHandleBackAction() : false;
}

void UUIViewBase::OnVisibleChanged(bool bInNewVisible)
{
	bInNewVisible ? OnViewActivated() : OnViewDeactivated();
}

void UUIViewBase::OnViewActivated()
{
	CheckAndExcuteBringBack();
}

void UUIViewBase::OnViewDeactivated()
{
	if (IsAnimationPlaying(UIAnim_Hide))
	{
		// UI Hide시 애니메이션을 무시하고 강제로 가려지는 상황은 언제든 발생할 수 있음 (ex)레벨 변경 시, 시퀀스 재생 시)
		// 그런 경우엔, 재생중인 Hide 애니메이션을 강제로 취소 시켜주어야 콜백관련 이슈가 방지될 것.
		StopAnimation(UIAnim_Hide);
	}
}

const EUIViewLayerPanelType UUIViewBase::GetLayerPanelType() const
{
	TWeakObjectPtr<UUIViewLayerPanel> LayerPanel = UUIViewSystem::Get()->GetUIViewLayerPanel(this);
	if (LayerPanel.IsValid() == false)
		return EUIViewLayerPanelType::Overlay;
	return LayerPanel->GetLayerPanelType();
}

bool UUIViewBase::ShouldUseInputMode() const
{
	TWeakObjectPtr<UUIViewLayerPanel> LayerPanel = UUIViewSystem::Get()->GetUIViewLayerPanel(this);
	if (LayerPanel.IsValid() == false)
		return false;

	return LayerPanel->GetLayerPanelType() == EUIViewLayerPanelType::CommonActivatableWidgetStack;
}

void UUIViewBase::OnShowAinmStarted()
{
	if (bTouchBlockWhenShowAnimPlaying)
	{
		// Hide 애니메이션 종료 시 터치블록 비활성화.
		SetTouchBlock(true);
	}
}

void UUIViewBase::OnShowAinmFinished()
{
	if (bTouchBlockWhenShowAnimPlaying)
	{
		// Show 애니메이션 종료 시 터치블록 비활성화.
		SetTouchBlock(false);
	}

	if (Callback_ShowFinished)
	{
		// Show 완료 콜백 호출
		Callback_ShowFinished();
	}
}

void UUIViewBase::OnHideAinmStarted()
{
	if (bTouchBlockWhenHideAnimPlaying)
	{
		// Hide 애니메이션 시작 시 터치블록 활성화.
		SetTouchBlock(true);
	}
}

void UUIViewBase::OnHideAinmFinished()
{
	SetVisibility(ESlateVisibility::Collapsed);
	
	if (bTouchBlockWhenHideAnimPlaying)
	{
		// Hide 애니메이션 종료 시 터치블록 비활성화.
		SetTouchBlock(false);
	}

	if (Callback_HideFinished)
	{
		// Hide 완료 콜백 호출
		Callback_HideFinished();
	}
}

void UUIViewBase::SetTouchBlock(bool bEnable)
{
	if (IsValid(Widget_TouchBlock) == false)
		return;

	ESlateVisibility NewVisibility = bEnable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	Widget_TouchBlock->SetVisibility(NewVisibility);
}

bool UUIViewBase::IsTouchBlock() const
{
	if (IsValid(Widget_TouchBlock) == false)
		return  false;

	return Widget_TouchBlock->IsVisible();
}

void UUIViewBase::Hide()
{
	UUIViewSystem::Hide(this);
}

bool UUIViewBase::IsBoundAnimationEvent(const TWeakObjectPtr<UWidgetAnimation> InWidgetAnim, const FWidgetAnimationDynamicEvent& InWidgetAnimEvent, const EWidgetAnimationEvent& AnimEventType)
{
	for(auto CurCallback : AnimationCallbacks)
	{
		if (CurCallback.Animation == InWidgetAnim.Get() && CurCallback.Delegate == InWidgetAnimEvent && CurCallback.AnimationEvent == AnimEventType)
			return true;
	}
	return false;
}

bool UUIViewBase::IsPlayingHideAnim() const
{
	return IsAnimationPlaying(UIAnim_Hide);
}

bool UUIViewBase::IsPlayingShowAnim() const
{
	return IsAnimationPlaying(UIAnim_Show);
}

bool UUIViewBase::IsClosable() const
{
	// Closable 조건 추가가 필요한 경우 재정의 하여 사용할 것.
	if (IsTouchBlock())
		return false;

	// 이미 Hide 애니메이션이 출력중이라면, Hide가 진행중인 것으로 간주. - 그대로 리턴.
	if (IsPlayingHideAnim())
		return false;
	
	return true;
}

bool UUIViewBase::IsUsingWidget() const
{
	// 디버깅을 위해 각 조건을 변수에 할당.

	// Hide 애니메이션이 출력중이라면, 사용중이지 않은 위젯으로 판정.
	if (IsPlayingHideAnim())
		return false;

	// 보여지지 않고 있다면, 사용중이지 않은 것.
	if (IsVisible() == false)
		return false;

	// 추가적인 조건이 필요한 경우 여기에 추가 or 재정의 하여 사용.
	
	return true;
}

bool UUIViewBase::IsStackedWidget() const
{
	return UUIViewSystem::IsStackedView(this);
}


void UUIViewBase::OnShow()
{
	PlayShowAnimation();
	
	// 여기에 UI 출력시 수행해야할 기능 추가. - 필요한 경우 재정의 하여 사용할 것.
}

void UUIViewBase::PlayShowAnimation()
{
	// Hide 애니메이션이 출력 중이라면 중단 처리.
	if (IsAnimationPlaying(UIAnim_Hide))
	{
		// ※ StopAnimation으로 중단 처리를 진행하면, AnimFinished 델리게이트가 호출될 것임. 이를 막기위해 Finished 애니매이션을 Unbind.
		UnbindFromAnimationFinished(UIAnim_Hide, HideAnimFinishEvent);
		StopAnimation(UIAnim_Hide);
	}
	
	if (IsValid(UIAnim_Show))
	{
		// IsBoundAnimationEvent로 체크를 하지 않으면, AnimationCallback이 계속 늘어나 메모리누수 및 오버플로 가능성이 있으므로 유의할 것.
		if (IsBoundAnimationEvent(UIAnim_Show, ShowAnimFinishEvent, EWidgetAnimationEvent::Finished) == false)
			BindToAnimationFinished(UIAnim_Show, ShowAnimFinishEvent);
		
		PlayAnimation(UIAnim_Show);
	}
}

void UUIViewBase::OnHide()
{
	PlayHideAnimation();
	
	// 여기에 UI Hide시 수행해야할 기능 추가. - 필요한 경우 재정의 하여 사용할 것.
}

void UUIViewBase::PlayHideAnimation()
{
	// Show 애니메이션이 출력 중이라면 중단 처리.
	if (IsAnimationPlaying(UIAnim_Show))
	{
		// ※ StopAnimation으로 중단 처리를 진행하면, AnimFinished 델리게이트가 호출될 것임. 이를 막기위해 Finished 애니매이션을 Unbind.
		UnbindFromAnimationFinished(UIAnim_Show, ShowAnimFinishEvent);
		StopAnimation(UIAnim_Show);
	}
	
	if (IsValid(UIAnim_Hide))
	{
		// IsBoundAnimationEvent로 체크를 하지 않으면, AnimationCallback이 계속 늘어나 메모리누수 및 오버플로 가능성이 있으므로 유의할 것.
		if (IsBoundAnimationEvent(UIAnim_Hide, HideAnimFinishEvent, EWidgetAnimationEvent::Finished) == false)
			BindToAnimationFinished(UIAnim_Hide, HideAnimFinishEvent);
		
		PlayAnimation(UIAnim_Hide);
	}
}

bool UUIViewBase::IsValidLayer()
{
	return UUIViewSystem::Get()->IsValidLayer(this);
}

void UUIViewBase::OnShowFromViewLayer(TFunction<void()> InShowFinishedFunction)
{
	OnShow();

	if (IsValid(UIAnim_Show))
	{
		// Show 애니메이션이 있는 경우 -> Show 애니메이션이 완료된 이후 콜백 호출.
		Callback_ShowFinished = InShowFinishedFunction;
		return;
	}

	// Show 애니메이션이 없는 경우 -> 즉시 콜백 호출.
	if (InShowFinishedFunction != nullptr)
		InShowFinishedFunction();
}

void UUIViewBase::OnHideFromViewLayer(TFunction<void()> InHideFinishedFunction)
{
	if (IsUsingWidget())
	{
		OnHide();
		
		if (IsValid(UIAnim_Hide))
		{
			// Hide 애니메이션이 있는 경우 -> Show 애니메이션이 완료된 이후 콜백 호출.
			Callback_HideFinished = InHideFinishedFunction;
			return;
		}
		else if (GetLayerPanelType() == EUIViewLayerPanelType::Overlay)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	
		// Hide 애니메이션이 없는 경우 -> 즉시 콜백 호출.
		if (InHideFinishedFunction != nullptr)
			InHideFinishedFunction();
	}
	else if (InHideFinishedFunction != nullptr)
	{
		// 사용 중이지 않은, 레이어 스택에 존재하고 있을 뿐인 위젯이라면, 즉시 콜백 호출.
		InHideFinishedFunction();
	}
}

void UUIViewBase::CheckAndExcuteBringBack()
{
	if (bBringBackFlag)
	{
		OnBringBack();
		bBringBackFlag = false;
	}
}

void UUIViewBase::OnBringBack()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
