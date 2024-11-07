// Fill out your copyright notice in the Description page of Project Settings.

#include "Track/LaneManager.h"
#include "Track/BaseLaneSegment.h"
#include "GameModes/RunnerLevelGM.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ALaneManager::ALaneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// Lane Object Pools
	ObjectPool_StandardLane = CreateDefaultSubobject<UObjectPoolComponent>(FName("Standard Lane Object Pool"));
	ObjectPool_StandardLane->PoolSize = 500;

	ObjectPool_GapHazard = CreateDefaultSubobject<UObjectPoolComponent>(FName("Gap Hazard Object Pool"));
	ObjectPool_GapHazard->PoolSize = 100;

	ObjectPool_RoughTerrainHazard = CreateDefaultSubobject<UObjectPoolComponent>(FName("Rough Terrain Object Pool"));
	ObjectPool_RoughTerrainHazard->PoolSize = 100;

	ObjectPool_WallHazard = CreateDefaultSubobject<UObjectPoolComponent>(FName("Wall Object Pool"));
	ObjectPool_WallHazard->PoolSize = 100;
	
}

void ALaneManager::OnConstruction(const FTransform& Transform)
{
	ObjectPool_StandardLane->PooledActorClass = StandardLaneSegment;
	ObjectPool_GapHazard->PooledActorClass = GapHazardLaneSegment;
	ObjectPool_RoughTerrainHazard->PooledActorClass = RoughTerrainHazardLaneSegment;
	ObjectPool_WallHazard->PooledActorClass = WallHazardLaneSegment;

	LanePositions.Empty();
	FVector SpawnLocation = GetActorLocation();

	for (int i = 0; i < NumOfLanes; i++)
	{
		if (i > 0)
			SpawnLocation.Y -= DistanceBetweenLaneSpawns;
		LanePositions.Emplace(SpawnLocation);
	}

	if (StandardLaneSegment)
	{
		ABaseLaneSegment* StandardSegment = Cast<ABaseLaneSegment>(StandardLaneSegment->GetDefaultObject());
		StandardSegmentLength = StandardSegment->GetSegmentLength();
		LaneSpawnDelay = StandardSegmentLength / LaneSpeed;
		LaneSpawnDelay *= 2.0f;
	}
}

// Called when the game starts or when spawned
void ALaneManager::BeginPlay()
{
	Super::BeginPlay();
	ABaseLaneSegment* StandardSegment = Cast<ABaseLaneSegment>(StandardLaneSegment->GetDefaultObject());
	StandardSegmentLength = StandardSegment->GetSegmentLength();
	auto GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
	GameModeRef->RegisterLaneManager(this);

	//GameModeRef->SpawnLaneSegmentDelegate.AddDynamic(this, &ALaneManager::SpawnSingleLaneSegment);
	SegmentTrackIndexIncreasedDelegate = GameModeRef->SegmentTrackIndexIncreasedDelegate;


	//ActiveSegments.SetNum(TrackLengthInSegments);
	for (int s = 0; s < TrackLengthInSegments; s++)
	{
		//ActiveSegments[s].SetNum(NumOfLanes);

		FVector pacerLocation = GetActorLocation();
		pacerLocation.X += StandardSegmentLength * s;
		
		TrackPacers.Add(pacerLocation);
		
		int i = 0;
		for (FVector& SpawnPoint : LanePositions)
		{
			FVector AdjustedSpawnPoint = SpawnPoint;
			AdjustedSpawnPoint.X = pacerLocation.X;
			UE_LOG(LogTemp, Log, TEXT("Adjusted Spawn Point: %s"), *AdjustedSpawnPoint.ToString());
			ABaseLaneSegment* NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), AdjustedSpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
			NewSegment->SetLaneIndex(i);
			NewSegment->SetTrackIndex(s);

			//ActiveSegments[s][i] = NewSegment;
			
			i++;
		}
	}

}



void ALaneManager::SpawnLaneSegment()
{
	//int currentLaneIndex = 0;
	//for (FVector& SpawnPoint : LanePositions)
	//{
	//	ABaseLaneSegment* NewSegment = nullptr;

	//	if (ABaseLaneSegment* previousSegment = *LastSpawnedSegmentPerLane.Find(currentLaneIndex))
	//	{
	//		SpawnPoint.X = previousSegment->GetBackAttachPointLocationInWorldSpace().X - (previousSegment->GetSegmentLength() * 2.0f);
	//	}

	//	switch (FMath::RandRange(0, 3))
	//	{
	//		case 0:
	//			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//			break;
	//		case 1:
	//			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_GapHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//			break;
	//		case 2:
	//			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_RoughTerrainHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//			break;
	//		case 3:
	//			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_WallHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//			break;
	//		default:
	//			break;
	//	}
	//	if (NewSegment)
	//	{
	//		LastSpawnedSegmentPerLane.Emplace(currentLaneIndex, NewSegment);
	//		NewSegment->SetSpeed(LaneSpeed);
	//	}

	//	currentLaneIndex++;
	//}
}

void ALaneManager::SpawnSingleLaneSegment(ABaseLaneSegment* PreviousSegmentRef)
{
	//int32 SegmentLaneIndex = PreviousSegmentRef->GetLaneIndex();
	///*UE_LOG(LogTemp, Log, TEXT("Event Calls: %i"), SegmentLaneIndex);*/
	//FVector SpawnPoint = PreviousSegmentRef->GetBackAttachPointLocationInWorldSpace();
	//SpawnPoint.X -= PreviousSegmentRef->GetSegmentLength();

	//ABaseLaneSegment* NewSegment = nullptr;
	//switch (FMath::RandRange(0, 3))
	//{
	//case 0:
	//	NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//	break;
	//case 1:
	//	NewSegment = Cast<ABaseLaneSegment>(ObjectPool_GapHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//	break;
	//case 2:
	//	NewSegment = Cast<ABaseLaneSegment>(ObjectPool_RoughTerrainHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//	break;
	//case 3:
	//	NewSegment = Cast<ABaseLaneSegment>(ObjectPool_WallHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
	//	break;
	//default:
	//	break;
	//}

	//if (NewSegment)
	//{
	//	NewSegment->SetSpeed(LaneSpeed);
	//	NewSegment->SetLaneIndex(SegmentLaneIndex);
	//	NewSegment->SetSpawnPoint(LanePositions[PreviousSegmentRef->GetLaneIndex()]);
	//}
}

// Called every frame
void ALaneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector ProposedNewPosition = TrackPacers[0];
	ProposedNewPosition.X += LaneSpeed * DeltaTime;
	float DistanceTraveled = FVector::Distance(ProposedNewPosition, GetActorLocation());
	
	if (DistanceTraveled >= StandardSegmentLength)
	{
		for (int s = 0; s < TrackLengthInSegments; s++)
		{
			FVector pacerLocation = GetActorLocation();
			pacerLocation.X += (StandardSegmentLength * s) + (DistanceTraveled - StandardSegmentLength);
			TrackPacers[s] = pacerLocation;
		}

		int i = 0;
		for (FVector& SpawnPoint : LanePositions)
		{
			FVector AdjustedSpawnPoint = SpawnPoint;
			AdjustedSpawnPoint.X = TrackPacers[0].X;
			ABaseLaneSegment* NewSegment = nullptr;
			if (SegmentsSinceLastSpawn > SegmentsBetweenObstacleSpawns)
			{
				switch (FMath::RandRange(1, 3))
				{
				case 0:
					NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), AdjustedSpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
					break;
				case 1:
					NewSegment = Cast<ABaseLaneSegment>(ObjectPool_GapHazard->SpawnFromPool(FTransform(GetActorRotation(), AdjustedSpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
					break;
				case 2:
					NewSegment = Cast<ABaseLaneSegment>(ObjectPool_RoughTerrainHazard->SpawnFromPool(FTransform(GetActorRotation(), AdjustedSpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
					break;
				case 3:
					NewSegment = Cast<ABaseLaneSegment>(ObjectPool_WallHazard->SpawnFromPool(FTransform(GetActorRotation(), AdjustedSpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
					break;
				default:
					break;
				}
			}
			else {
				NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), AdjustedSpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
			}
			//ABaseLaneSegment* NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), AdjustedSpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
			
			if (SegmentsSinceLastSpawn == SegmentsBetweenObstacleSpawns)
			{
				NewSegment->LaneType = ETrackType::ETT_PreHazard;
				//NewSegment->BlockPlayerMovement = true;
			}
			NewSegment->SetLaneIndex(i);
			NewSegment->SetTrackIndex(0);
			
			i++;
		}

		if (SegmentsSinceLastSpawn > SegmentsBetweenObstacleSpawns)
		{
			SegmentsSinceLastSpawn = 0;
		}
		else {
			SegmentsSinceLastSpawn++;
		}
		
		UE_LOG(LogTemp, Log, TEXT("Broadcasting Delegate"));
		SegmentTrackIndexIncreasedDelegate.Broadcast();
	}
	else {
		for (FVector& pacePoint : TrackPacers)
		{
			pacePoint.X += LaneSpeed * DeltaTime;

		}
	}

	for (FVector& pacePoint : TrackPacers)
	{
		DrawDebugSphere(GetWorld(), pacePoint, 100.0f, 32, FColor::Red);       
	}
}

void ALaneManager::SetLaneSpeed(float NewSpeed)
{
	LaneSpeed = NewSpeed;
}

int ALaneManager::GetNumberOfLanes() const
{
	return NumOfLanes;
}

TTuple<FVector, FVector> ALaneManager::GetLaneLineVectors(int LaneIndex)
{
	FVector OffsetLanePosition = LanePositions[LaneIndex];
	OffsetLanePosition.X += 1.0f;
	return TTuple<FVector, FVector>(LanePositions[LaneIndex], OffsetLanePosition);
}


