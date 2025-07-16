// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIViewBase.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BKCUISystemMsgBase.generated.h"

/**
 * 
 */
UCLASS()
class BLINKPORTFOLIO_API UBKCUISystemMsgBase : public UUIViewBase
{
	GENERATED_BODY()

public:
	UBKCUISystemMsgBase() : UUIViewBase(UBKUIViewSystem::GetLayerNameByType(EBKCUIViewLayerType::SystemMsg)) {}
};
