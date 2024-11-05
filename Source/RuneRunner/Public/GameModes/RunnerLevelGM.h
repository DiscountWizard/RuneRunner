// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RunnerLevelGM.generated.h"

class ALaneManager;
class ABaseLaneSegment;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnLaneSegmentDelegate, ABaseLaneSegment*, PreviousSegmentRef);

UCLASS()
class RUNERUNNER_API ARunnerLevelGM : public AGameModeBase
{
	GENERATED_BODY()

public:

	ALaneManager* LaneManager;

	// Overrides
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);

	virtual void StartPlay() override;

	virtual void SetPlayerDefaults(APawn* PlayerPawn);

	// Public Methods
	void RegisterLaneManager(ALaneManager* InLaneManager);

	ALaneManager* GetLaneManager();

	FSpawnLaneSegmentDelegate SpawnLaneSegmentDelegate;
};
