// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UIViewSettings.generated.h"

class UUIViewLayer;




UCLASS(config = UIView, DefaultConfig, meta = (DisplayName = "LayerSettings"))
class UIVIEW_API UUIViewSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UUIViewSettings();

public:
	const TSoftClassPtr<UUIViewLayer>& GetLayerWidgetClass() const { return LayerWidgetClass; };
	
	const FString& GetClassPathFormat() const { return ClassPathFormat; };

protected:
	
	UPROPERTY(EditAnywhere, Config, Category="UIView", meta = (AllowAbstract = false))
	TSoftClassPtr<UUIViewLayer> LayerWidgetClass = nullptr;
	
	UPROPERTY(EditAnywhere, Config, Category="UIView")
	FString ClassPathFormat = TEXT("/Game/UI/UIView/{0}/bp_{1}.bp_{1}_C");
};



