// Fill out your copyright notice in the Description page of Project Settings.


#include "../HUD/BKHUD_InGame.h"

#include "BlinkPortfolio/UI/UIViewSystem/BKUIViewSystem.h"
#include "BlinkPortfolio/UI/UIViewWidgets/HUD/BKCUIHUD_InGameMain.h"

void ABKHUD_InGame::BeginPlay()
{
	Super::BeginPlay();

	// 인게임 메인 UI 출력.
	UBKUIViewSystem::Show<UBKCUIHUD_InGameMain>();
}

void ABKHUD_InGame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UBKUIViewSystem::CleanUp();
}
