// Fill out your copyright notice in the Description page of Project Settings.


#include "BlinkPortfolio/UI/UIViewWidgets/Base/BKCUIHUDBase.h"

TOptional<FUIInputConfig> UBKCUIHUDBase::GetDesiredInputConfig() const
{
	FUIInputConfig NewInputConfig(ECommonInputMode::All, EMouseCaptureMode::CaptureDuringMouseDown, false);
	return NewInputConfig;
}
