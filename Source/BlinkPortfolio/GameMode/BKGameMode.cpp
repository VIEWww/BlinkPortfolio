// Fill out your copyright notice in the Description page of Project Settings.
#include "../GameMode/BKGameMode.h"

void ABKGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsPlayingGame)
	{
		PlayTime += DeltaSeconds;
		
		if (MaxPlayTime <= PlayTime)
		{
			OnEndGame();
		}

		int64 CurrentSpawnCycle = PlayTime / SpawnCycle;
		int64 LastSpawnCycle = LastSpawnTime / SpawnCycle;
		if (CurrentSpawnCycle > LastSpawnCycle)
		{
			LastSpawnTime = PlayTime;
			if (PlayTime < 60.0f)
			{
				// 0초 ~ 1분
			}
			else if (PlayTime < 120.0f)
			{
				// 1분 ~ 2분
			}
			else if (PlayTime < 180.0f)
			{
				// 2분 ~ 3분
			}
		}
	}
}

void ABKGameMode::OnStartGame()
{
	PlayTime = 0.0f;
	bIsPlayingGame = true;
}

void ABKGameMode::OnEndGame()
{
	PlayTime = 0.0f;
	bIsPlayingGame = false;
}
