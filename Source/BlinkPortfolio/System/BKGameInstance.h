// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BKGameInstance.generated.h"

/**
 * 
 */
class UGlobalManagerSystem;
class FWidgetInputPreProcessor;

UCLASS()
class BLINKPORTFOLIO_API UBKGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	static UBKGameInstance* Get();

private:
	virtual void Init() override;
	virtual void Shutdown() override;

private:
	inline static UBKGameInstance* ThisInstance = nullptr;
};
