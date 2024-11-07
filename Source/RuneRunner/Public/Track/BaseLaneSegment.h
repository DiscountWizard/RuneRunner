// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PooledActor.h"
#include "Track/LaneTypes.h"
#include "GameModes/RunnerLevelGM.h"
#include "BaseLaneSegment.generated.h"

class UBoxComponent;
class UArrowComponent;
class ALaneManager;


UCLASS()
class RUNERUNNER_API ABaseLaneSegment : public APooledActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	ABaseLaneSegment();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetPoolActorInUse(bool NewValue) override;

	void MoveLane(FVector MoveAmount);
	void SetSpeed(float NewSpeed);
	void SetLaneIndex(int32 NewIndex);
	void SetTrackIndex(int32 NewIndex);

	UFUNCTION()
	void IncreaseTrackIndex();
	int32 GetLaneIndex();
	void SetSpawnPoint(FVector SpawnPoint);
	float GetSegmentLength() const;
	FORCEINLINE ETrackType GetTrackType() { return LaneType; }

	bool BlockPlayerMovement = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Segment Config")
	ETrackType LaneType = ETrackType::ETT_Standard;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, Category = "Lane Segment Config")
	UBoxComponent* SegmentCollisionBox;

	UPROPERTY(BlueprintReadOnly, Category = "Lane Segment Config")
	float SegmentSpeed = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Segment Config")
	float SegmentLength = 200.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Segment Config")
	float SegmentWidth = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lane Information")
	UArrowComponent* RootPoint;

	//FSpawnLaneSegmentDelegate SpawnLaneDelegate;

	FVector StartingLocation;
	bool SpawnedNewActor = false;
	
	UPROPERTY(VisibleAnywhere, Category = "Lane Information")
	int32 LaneIndex;

	UPROPERTY(VisibleAnywhere, Category = "Lane Information")
	int32 TrackIndex;
private:

	ALaneManager* LaneManagerRef;

};
