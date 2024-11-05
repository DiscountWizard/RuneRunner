// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/RunnerLevelGM.h"
#include "Track/LaneManager.h"
#include "Track/BaseLaneSegment.h"
#include "PlayerVehicle.h"

void ARunnerLevelGM::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ARunnerLevelGM::StartPlay()
{
	Super::StartPlay();
}

void ARunnerLevelGM::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);
}

void ARunnerLevelGM::RegisterLaneManager(ALaneManager* InLaneManager)
{
	LaneManager = InLaneManager;
}

ALaneManager* ARunnerLevelGM::GetLaneManager()
{
	return LaneManager;
}
