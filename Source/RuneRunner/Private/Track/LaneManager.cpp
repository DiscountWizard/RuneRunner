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

	SpawnCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("Spawn Collision Box"));
	SpawnCollisionBox->SetBoxExtent(FVector(20.0f, 4000.f, 4000.f));
	SpawnCollisionBox->bHiddenInGame = false;
	SpawnCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SpawnCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	SpawnCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ALaneManager::OnOverlapEnd);
	SetRootComponent(SpawnCollisionBox);

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
		LaneSpawnDelay = StandardSegment->GetSegmentLength() / LaneSpeed;
		LaneSpawnDelay *= 2.0f;
	}
}

// Called when the game starts or when spawned
void ALaneManager::BeginPlay()
{
	Super::BeginPlay();
	auto GameModeRef = GetWorld()->GetAuthGameMode<ARunnerLevelGM>();
	GameModeRef->RegisterLaneManager(this);

	//FTimerDelegate TimerDelegate;
	//FTimerHandle TimerHandle;

	//TimerDelegate.BindUFunction(this, FName("SpawnLaneSegment"));
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, LaneSpawnDelay, true);

	LastSpawnedSegmentPerLane.Empty();
	//LastSpawnedSegmentPerLane.Init(nullptr, NumOfLanes);
	for (int i = 0; i < NumOfLanes; i++)
	{
		LastSpawnedSegmentPerLane.Emplace(i, nullptr);
	}

	for (FVector& SpawnPoint : LanePositions)
	{
		ABaseLaneSegment* NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
		NewSegment->SetSpeed(LaneSpeed);
	}

}



void ALaneManager::SpawnLaneSegment()
{
	int currentLaneIndex = 0;
	for (FVector& SpawnPoint : LanePositions)
	{
		ABaseLaneSegment* NewSegment = nullptr;

		if (ABaseLaneSegment* previousSegment = *LastSpawnedSegmentPerLane.Find(currentLaneIndex))
		{
			SpawnPoint.X = previousSegment->GetBackAttachPointLocationInWorldSpace().X - previousSegment->GetSegmentLength();
		}

		switch (FMath::RandRange(0, 3))
		{
			case 0:
				NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
				break;
			case 1:
				NewSegment = Cast<ABaseLaneSegment>(ObjectPool_GapHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
				break;
			case 2:
				NewSegment = Cast<ABaseLaneSegment>(ObjectPool_RoughTerrainHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
				break;
			case 3:
				NewSegment = Cast<ABaseLaneSegment>(ObjectPool_WallHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
				break;
			default:
				break;
		}
		if (NewSegment)
		{
			LastSpawnedSegmentPerLane.Emplace(currentLaneIndex, NewSegment);
			NewSegment->SetSpeed(LaneSpeed);
		}

		currentLaneIndex++;
	}
}

void ALaneManager::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseLaneSegment* ExitingSegment = Cast<ABaseLaneSegment>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Overlap End: %s"), *OtherActor->GetName());
		ABaseLaneSegment* NewSegment = nullptr;
		FVector SpawnPoint = ExitingSegment->GetBackAttachPointLocationInWorldSpace();
		SpawnPoint.X -= ExitingSegment->GetSegmentLength();

		switch (FMath::RandRange(0, 3))
		{
		case 0:
			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_StandardLane->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
			break;
		case 1:
			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_GapHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
			break;
		case 2:
			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_RoughTerrainHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
			break;
		case 3:
			NewSegment = Cast<ABaseLaneSegment>(ObjectPool_WallHazard->SpawnFromPool(FTransform(GetActorRotation(), SpawnPoint, FVector(1.0f, 1.0f, 1.0f))));
			break;
		default:
			break;
		}

		if (NewSegment)
		{
			NewSegment->SetSpeed(LaneSpeed);
		}
	}
}

// Called every frame
void ALaneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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


