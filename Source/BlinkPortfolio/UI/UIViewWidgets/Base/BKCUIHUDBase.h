// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIViewBase.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BKCUIHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class BLINKPORTFOLIO_API UBKCUIHUDBase : public UUIViewBase
{
	GENERATED_BODY()
	
public:
	UBKCUIHUDBase() : UUIViewBase(UBKUIViewSystem::GetLayerNameByType(EBKCUIViewLayerType::HUD)) {}

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
};
