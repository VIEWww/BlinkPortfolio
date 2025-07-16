// Fill out your copyright notice in the Description page of Project Settings.
#include "BlinkPortfolio/System/BKGameInstance.h"
#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"

UBKGameInstance* UBKGameInstance::Get()
{
	return ThisInstance;
}

void UBKGameInstance::Init()
{
	Super::Init();

	ThisInstance = this;

	UBKUIViewSystem::Install<UBKUIViewSystem>(this);
}

void UBKGameInstance::Shutdown()
{
	Super::Shutdown();

	UBKUIViewSystem::UnInstall();

	ThisInstance = nullptr;
}