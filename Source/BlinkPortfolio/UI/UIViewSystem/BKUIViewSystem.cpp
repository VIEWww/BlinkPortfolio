// Fill out your copyright notice in the Description page of Project Settings.
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BlinkPortfolio/Utility/BKLibrary.h"


void UBKUIViewSystem::OnPostInstall()
{
	Super::OnPostInstall();

	ClearAllLayerInfo();
	for (int32 idx = (int32)EBKCUIViewLayerType::Begin; idx < (int32)EBKCUIViewLayerType::MAX; idx++)
	{
		EBKCUIViewLayerType CurLayerType = EBKCUIViewLayerType(idx);
		FName CurLayerName = GetLayerNameByType(CurLayerType);
		AddLayerInfo(CurLayerName);
	}
}

UBKUIViewSystem* UBKUIViewSystem::Get()
{
	return Cast<UBKUIViewSystem>(UUIViewSystem::Get());
}

FName UBKUIViewSystem::GetLayerNameByType(const EBKCUIViewLayerType& InLayerType)
{
	FName LayerName = *ENUM_TO_STRING(EBKCUIViewLayerType, InLayerType);
	return LayerName;
}
