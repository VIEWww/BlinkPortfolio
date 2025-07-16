// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BKGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLINKPORTFOLIO_API ABKGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	virtual void Tick(float DeltaSeconds) override;
	
public:
	UFUNCTION(BlueprintCallable)
	void OnStartGame();
	void OnEndGame();

protected:
    // 최대 플레이 시간.
	double MaxPlayTime = 600.0f;
	// 현재까지 플레이 시간.
	double PlayTime = 0.0f;
	// 이전에 스폰시켰던 시간. (5 - 10 - 15 - 20 - ...)
	double LastSpawnTime = 0.0f;
	// 스폰 주기
	double SpawnCycle = 5.0f;
	
	bool bIsPlayingGame = false;
};