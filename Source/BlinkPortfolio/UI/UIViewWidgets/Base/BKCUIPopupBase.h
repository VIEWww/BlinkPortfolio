// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIViewBase.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BKCUIPopupBase.generated.h"

/**
 * 
 */
UCLASS()
class BLINKPORTFOLIO_API UBKCUIPopupBase : public UUIViewBase
{
	GENERATED_BODY()

public:
	UBKCUIPopupBase() : UUIViewBase(UBKUIViewSystem::GetLayerNameByType(EBKCUIViewLayerType::Popup)) {}

};
