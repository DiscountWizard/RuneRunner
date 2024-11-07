// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <list>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ObjectPoolComponent.h"
#include "GameModes/RunnerLevelGM.h"
#include "LaneManager.generated.h"

class ABaseLaneSegment;
class UBoxComponent;

UCLASS()
class RUNERUNNER_API ALaneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaneManager();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetLaneSpeed(float NewSpeed);

	int GetNumberOfLanes() const;

	TTuple<FVector, FVector> GetLaneLineVectors(int LaneIndex);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	AActor* StaticCamera;

	UPROPERTY(VisibleAnywhere, Category = "Lane Information")
	TArray<FVector> TrackPacers;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Lane Segments")
	TArray<FVector> LanePositions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lane Configuration")
	int NumOfLanes = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lane Configuration")
	int TrackLengthInSegments = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lane Configuration")
	double DistanceBetweenLaneSpawns = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lane Segments")
	TSoftClassPtr<ABaseLaneSegment> StandardLaneSegment;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lane Segments")
	TSoftClassPtr<ABaseLaneSegment> GapHazardLaneSegment;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lane Segments")
	TSoftClassPtr<ABaseLaneSegment> RoughTerrainHazardLaneSegment;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lane Segments")
	TSoftClassPtr<ABaseLaneSegment> WallHazardLaneSegment;

	UFUNCTION()
	void SpawnLaneSegment();

	UFUNCTION()
	void SpawnSingleLaneSegment(ABaseLaneSegment* PreviousSegmentRef);

	UPROPERTY(VisibleAnywhere, Category = "Object Pool")
	UObjectPoolComponent* ObjectPool_StandardLane;

	UPROPERTY(VisibleAnywhere, Category = "Object Pool")
	UObjectPoolComponent* ObjectPool_GapHazard;

	UPROPERTY(VisibleAnywhere, Category = "Object Pool")
	UObjectPoolComponent* ObjectPool_RoughTerrainHazard;

	UPROPERTY(VisibleAnywhere, Category = "Object Pool")
	UObjectPoolComponent* ObjectPool_WallHazard;

private:

	UPROPERTY(EditAnywhere, Category = "Player Reference")
	int PlayerCurrentLaneIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Lane Configuration")
	float LaneSpawnDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Lane Configuration")
	float LaneSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Lane Configuration")
	int SegmentsBetweenObstacleSpawns = 10;

	int SegmentsSinceLastSpawn = 0;

	float ElapsedTimeSinceSegmentSpawn = 0.f;

	float StandardSegmentLength;
	
	FSegmentTrackIndexIncreased SegmentTrackIndexIncreasedDelegate;

};
